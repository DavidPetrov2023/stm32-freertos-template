// drivers/uart/uart_stm32.c
#include "uart/uart_stm32.h"
#include "stm32g0xx_hal.h"

/* Compatibility for HAL variants that don't define *_RXNE_RXFNE */
#ifndef UART_FLAG_RXNE_RXFNE
#define UART_FLAG_RXNE_RXFNE UART_FLAG_RXNE
#endif
#ifndef UART_IT_RXNE_RXFNE
#define UART_IT_RXNE_RXFNE UART_IT_RXNE
#endif

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h" // pdMS_TO_TICKS

/* Map API config to HAL defines (kept static to this TU) */
static uint32_t map_parity(uart_parity_mode_t p)
{
    switch (p) {
    case UART_PARITY_MODE_EVEN:
        return UART_PARITY_EVEN;
    case UART_PARITY_MODE_ODD:
        return UART_PARITY_ODD;
    case UART_PARITY_MODE_NONE:
    default:
        return UART_PARITY_NONE;
    }
}

static uint32_t map_stop(uart_stop_mode_t s)
{
    switch (s) {
    case UART_STOP_MODE_2:
        return UART_STOPBITS_2;
    case UART_STOP_MODE_1:
    default:
        return UART_STOPBITS_1;
    }
}

/* === RX ring === */
static inline bool rx_ring_pop(uart_stm32_ctrl_t *p, uint8_t *out)
{
    if (p->rx_head == p->rx_tail)
        return false;
    *out = p->rx_buf[p->rx_tail];
    p->rx_tail = (p->rx_tail + 1u) % (size_t) UART_RX_BUF_SIZE;
    return true;
}

static inline size_t rx_ring_count(const uart_stm32_ctrl_t *p)
{
    if (p->rx_head >= p->rx_tail)
        return p->rx_head - p->rx_tail;
    return (size_t) UART_RX_BUF_SIZE - (p->rx_tail - p->rx_head);
}

void uart_stm32_irq_rx_feed(uart_stm32_ctrl_t *p, uint8_t byte)
{
    if (!p)
        return;
    size_t next = (p->rx_head + 1u) % (size_t) UART_RX_BUF_SIZE;
    if (next != p->rx_tail) {
        p->rx_buf[p->rx_head] = byte;
        p->rx_head = next;
    } /* else: drop on overflow */
}

/* === API impl === */
static app_err_t uart_open(uart_ctrl_t *ctrl, const uart_cfg_t *cfg)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) ctrl;
    if (!p || !p->hal || !cfg)
        return APP_ERR_INVALID_ARG;

    /* Save cfg (not strictly needed for fixed setup) */
    p->cfg = *cfg;

    /* Prepare RX ring */
    p->rx_head = p->rx_tail = 0;

    /* Prepare mutex (okay before scheduler starts) */
    p->tx_mutex = xSemaphoreCreateMutexStatic(&p->tx_mutex_storage);
    if (!p->tx_mutex)
        return APP_ERR_NO_RESOURCE;

    /* Apply basic line settings (keep WordLength/Mode/HwFlowCtl from CubeMX) */
    p->hal->Init.BaudRate = cfg->baudrate;
    p->hal->Init.Parity = map_parity(cfg->parity);
    p->hal->Init.StopBits = map_stop(cfg->stop_bits);

    if (HAL_UART_Init(p->hal) != HAL_OK) {
        return APP_ERR_IO;
    }

    /* Clear any lingering status/error flags before enabling IRQs */
    __HAL_UART_CLEAR_OREFLAG(p->hal);
    __HAL_UART_CLEAR_FEFLAG(p->hal);
    __HAL_UART_CLEAR_NEFLAG(p->hal);
    __HAL_UART_CLEAR_PEFLAG(p->hal);
    __HAL_UART_CLEAR_IDLEFLAG(p->hal);

    /* Enable UART and interrupts: data RX + errors (ORE/FE/NE/PE) */
    __HAL_UART_ENABLE(p->hal);
    __HAL_UART_ENABLE_IT(p->hal, UART_IT_RXNE_RXFNE);
    __HAL_UART_ENABLE_IT(p->hal, UART_IT_ERR);
    __HAL_UART_ENABLE_IT(p->hal, UART_IT_PE);

    return APP_SUCCESS;
}

static app_err_t uart_close(uart_ctrl_t *ctrl)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) ctrl;
    if (!p || !p->hal)
        return APP_ERR_INVALID_ARG;

    /* Disable RX + error interrupts before deinit */
    __HAL_UART_DISABLE_IT(p->hal, UART_IT_RXNE_RXFNE);
    __HAL_UART_DISABLE_IT(p->hal, UART_IT_ERR);
    __HAL_UART_DISABLE_IT(p->hal, UART_IT_PE);

    (void) HAL_UART_DeInit(p->hal);

    if (p->tx_mutex) {
        vSemaphoreDelete(p->tx_mutex); /* static mutex; on some ports it's a no-op */
        p->tx_mutex = NULL;
    }
    return APP_SUCCESS;
}

static int32_t uart_write(uart_ctrl_t *ctrl, const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) ctrl;
    if (!p || !p->hal || !data)
        return APP_ERR_INVALID_ARG;
    if (len == 0)
        return 0;

    /* Serialize writers */
    if (p->tx_mutex) {
        if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
            if (xSemaphoreTake(p->tx_mutex, pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
                return APP_ERR_TIMEOUT;
            }
        } else {
            /* Scheduler not running yet: just proceed */
        }
    }

    HAL_StatusTypeDef st = HAL_UART_Transmit(p->hal,
                                             (uint8_t *) data,
                                             (uint16_t) len,
                                             (timeout_ms == 0u) ? 1u : timeout_ms);
    int32_t ret = (st == HAL_OK)        ? (int32_t) len
                  : (st == HAL_TIMEOUT) ? APP_ERR_TIMEOUT
                                        : APP_ERR_IO;

    if (p->tx_mutex && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        (void) xSemaphoreGive(p->tx_mutex);
    }
    return ret;
}

static int32_t uart_read(uart_ctrl_t *ctrl, uint8_t *out, size_t len)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) ctrl;
    if (!p || !out || len == 0)
        return 0;
    size_t n = 0;
    while (n < len && rx_ring_pop(p, &out[n])) {
        n++;
    }
    return (int32_t) n;
}

static bool uart_rx_available(uart_ctrl_t *ctrl)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) ctrl;
    return p && (rx_ring_count(p) > 0);
}

const uart_api_t g_uart_stm32_api = {
    .open = uart_open,
    .close = uart_close,
    .write = uart_write,
    .read = uart_read,
    .rx_available = uart_rx_available,
};