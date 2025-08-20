// drivers/uart/uart_stm32.c
#include "uart_stm32.h"
#include "stm32g0xx_hal.h"
#include "task.h"

// Map parity helper (keeps HAL details out of API)
static uint32_t map_parity(uart_parity_mode_t p)
{
    switch (p) {
    case UART_PARITY_MODE_EVEN:
        return UART_PARITY_EVEN;
    case UART_PARITY_MODE_ODD:
        return UART_PARITY_ODD;
    default:
        return UART_PARITY_NONE;
    }
}

static app_err_t uart_open(uart_ctrl_t *pg, const uart_cfg_t *cfg)
{
    if (!pg || !cfg)
        return APP_ERR_INVALID_ARG;
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) pg;

    // Initialize control block
    p->baudrate = cfg->baudrate;
    p->parity = cfg->parity;
    p->stop_bits = cfg->stop_bits;
    p->rx_head = 0u;
    p->rx_tail = 0u;

    // Create static TX mutex
    p->tx_mutex = xSemaphoreCreateMutexStatic(&p->tx_mutex_storage);
    if (p->tx_mutex == NULL)
        return APP_ERR_INVALID_ARG;

    // Configure HAL UART2 (CubeMX provides huart2 + MX_USART2_UART_Init)
    extern UART_HandleTypeDef huart2;
    huart2.Init.BaudRate = p->baudrate;
    huart2.Init.Parity = map_parity(p->parity);
    huart2.Init.StopBits = (p->stop_bits == UART_STOP_MODE_2) ? UART_STOPBITS_2 : UART_STOPBITS_1;

    if (HAL_UART_Init(&huart2) != HAL_OK) {
        return APP_ERR_INVALID_ARG;
    }

    // Enable RX interrupt (read RDR in IRQ)
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    p->is_open = true;
    return APP_SUCCESS;
}

static app_err_t uart_close(uart_ctrl_t *pg)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) pg;
    if (!p || !p->is_open)
        return APP_ERR_NOT_OPEN;
    extern UART_HandleTypeDef huart2;
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    p->is_open = false;
    return APP_SUCCESS;
}

static int32_t uart_write(uart_ctrl_t *pg, const uint8_t *data, size_t len, uint32_t timeout_ms)
{
    if (!pg || !data || len == 0u)
        return APP_ERR_INVALID_ARG;
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) pg;
    if (!p->is_open)
        return APP_ERR_NOT_OPEN;

    extern UART_HandleTypeDef huart2;
    if (xSemaphoreTake(p->tx_mutex, pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
        return -3; // timeout
    }
    HAL_StatusTypeDef st = HAL_UART_Transmit(&huart2, (uint8_t *) data, (uint16_t) len, timeout_ms);
    xSemaphoreGive(p->tx_mutex);

    if (st != HAL_OK)
        return -4; // TX error
    return (int32_t) len;
}

static int32_t uart_read(uart_ctrl_t *pg, uint8_t *out, size_t len)
{
    if (!pg || !out || len == 0u)
        return APP_ERR_INVALID_ARG;
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) pg;
    if (!p->is_open)
        return APP_ERR_NOT_OPEN;

    size_t n = 0u;
    while ((n < len) && (p->rx_tail != p->rx_head)) {
        out[n++] = p->rx_buf[p->rx_tail];
        p->rx_tail = (p->rx_tail + 1u) % (size_t) UART_RX_BUF_SIZE;
    }
    return (int32_t) n;
}

static bool uart_rx_available(uart_ctrl_t *pg)
{
    uart_stm32_ctrl_t *p = (uart_stm32_ctrl_t *) pg;
    return (p && p->is_open && (p->rx_tail != p->rx_head));
}

const uart_api_t g_uart_stm32_api = {
    .open = uart_open,
    .close = uart_close,
    .write = uart_write,
    .read = uart_read,
    .rx_available = uart_rx_available,
};

// Minimal ISR hook: push byte into ring buffer, no blocking, no heavy work
void uart_stm32_irq_rx_feed(uart_stm32_ctrl_t *p, uint8_t byte)
{
    size_t next = (p->rx_head + 1u) % (size_t) UART_RX_BUF_SIZE;
    if (next != p->rx_tail) {
        p->rx_buf[p->rx_head] = byte;
        p->rx_head = next;
    }
    // No callback here; app polls via read()/rx_available(). Keeps ISR minimal and deterministic.
}
