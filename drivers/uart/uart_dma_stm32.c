#include "uart_dma_stm32.h"
#include <string.h>

#include "usart.h"

extern UART_HandleTypeDef huart2;

/* ---- Config (low-level driver owns the rings and DMA buffers) ---- */
#ifndef RX_DMA_SIZE
#define RX_DMA_SIZE 128u
#endif

#ifndef TX_BUF_SIZE
#define TX_BUF_SIZE 256u
#endif

/* ---- Private state ---- */
static UART_HandleTypeDef *s_huart = NULL;

static uart_rx_cb_t s_rx_cb = NULL;
static void *s_rx_ctx = NULL;

/* RX DMA bounce buffer (filled by HAL) */
static uint8_t rx_dma_buf[RX_DMA_SIZE];

/* TX ring buffer */
static uint8_t tx_buf[TX_BUF_SIZE];
static volatile size_t tx_head = 0;
static volatile size_t tx_tail = 0;
static volatile uint8_t tx_busy = 0;
static volatile size_t tx_chunk_len = 0;

/* ---- Helpers ---- */

/* Compute free space in TX ring. */
static inline size_t tx_space(void)
{
    size_t h = tx_head;
    size_t t = tx_tail;
    return (t >= h) ? (TX_BUF_SIZE - (t - h) - 1) : (h - t - 1);
}

/* Kick the HAL IT transmit with a linear chunk from head. */
static void tx_kick_start(void)
{
    if (tx_busy)
        return;

    size_t h = tx_head;
    size_t t = tx_tail;
    if (h == t)
        return;

    size_t len = (t > h) ? (t - h) : (TX_BUF_SIZE - h);
    if (len == 0)
        return;

    tx_busy = 1;
    tx_chunk_len = len;

    (void) HAL_UART_Transmit_IT(s_huart, &tx_buf[h], (uint16_t) len);
}

/* ---- Public API ---- */

bool uart_dma_init(UART_HandleTypeDef *huart, uart_rx_cb_t rx_cb, void *user_ctx)
{
    if (!huart)
        return false;

    /* Bind handle and user callback */
    s_huart = huart;
    s_rx_cb = rx_cb;
    s_rx_ctx = user_ctx;

    /* Reset TX state */
    tx_head = tx_tail = 0;
    tx_busy = 0;
    tx_chunk_len = 0;

    /* (Optional) Clear UART error/IDLE flags before arming RX */
    __HAL_UART_CLEAR_OREFLAG(s_huart);  /* if available on your HAL */
    __HAL_UART_CLEAR_IDLEFLAG(s_huart); /* clear IDLE before enabling */

    /* Start RX: receive-to-idle with DMA, then disable half-transfer IRQ */
    HAL_StatusTypeDef st = HAL_UARTEx_ReceiveToIdle_DMA(s_huart, rx_dma_buf, RX_DMA_SIZE);
    __HAL_DMA_DISABLE_IT(s_huart->hdmarx, DMA_IT_HT);

    return (st == HAL_OK);
}

size_t uart_dma_write(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return 0;

    size_t written = 0;

    /* Short critical section without RTOS dependency: mask IRQs. */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    while (written < len) {
        size_t space = tx_space();
        if (space == 0)
            break;

        size_t t = tx_tail;
        size_t linear = (t >= tx_head) ? (TX_BUF_SIZE - t) : (tx_head - t - 1);
        if (linear == 0)
            linear = space;

        size_t chunk = len - written;
        if (chunk > space)
            chunk = space;
        if (chunk > linear)
            chunk = linear;

        /* Copy chunk into ring */
        if (chunk) {
            size_t first = chunk;
            /* (We could split into two memcpy when wrapping; loop is fine & tiny) */
            for (size_t i = 0; i < first; ++i) {
                tx_buf[t] = data[written + i];
                t = (t + 1) % TX_BUF_SIZE;
            }
            tx_tail = t;
            written += chunk;
        }
    }

    /* Start/continue IT TX if needed */
    tx_kick_start();

    if (!primask)
        __enable_irq();
    return written;
}

/* ---- HAL Callbacks ---- */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == s_huart) {
        if (s_rx_cb && Size) {
            /* Notify user: we're in ISR context */
            s_rx_cb(rx_dma_buf, (size_t) Size, true, s_rx_ctx);
        }

        /* Re-arm receive-to-idle DMA */
        HAL_UARTEx_ReceiveToIdle_DMA(s_huart, rx_dma_buf, RX_DMA_SIZE);
        __HAL_DMA_DISABLE_IT(s_huart->hdmarx, DMA_IT_HT);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == s_huart) {
        /* On error, try to re-arm RX */
        HAL_UARTEx_ReceiveToIdle_DMA(s_huart, rx_dma_buf, RX_DMA_SIZE);
        __HAL_DMA_DISABLE_IT(s_huart->hdmarx, DMA_IT_HT);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == s_huart) {
        /* Advance head by the chunk we handed to HAL. */
        size_t h = tx_head;
        h = (h + tx_chunk_len) % TX_BUF_SIZE;
        tx_head = h;

        tx_busy = 0;
        tx_chunk_len = 0;

        /* If more data in ring, kick next chunk. */
        tx_kick_start();
    }
}
