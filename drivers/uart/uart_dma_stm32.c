#include "uart_dma_stm32.h"
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"
#include <string.h>

#define RX_DMA_SIZE 128u
#define RX_STREAM_STORAGE_SIZE 512u
#define TX_BUF_SIZE 256u

static uint8_t rx_dma_buf[RX_DMA_SIZE];
static StaticStreamBuffer_t rx_stream_ctrl;
static uint8_t rx_stream_storage[RX_STREAM_STORAGE_SIZE];
static StreamBufferHandle_t rx_stream;

//static uint8_t tx_buf[TX_BUF_SIZE];
static volatile size_t tx_head = 0;
static volatile size_t tx_tail = 0;
static volatile uint8_t tx_busy = 0;
static volatile size_t tx_chunk_len = 0;

static UART_HandleTypeDef *g_huart = NULL;

/* TX ring utils */
/*static size_t uart_tx_space(void)
{
    size_t h = tx_head;
    size_t t = tx_tail;
    if (t >= h)
        return (TX_BUF_SIZE - (t - h) - 1);
    else
        return (h - t - 1);
}*/

/*static void uart_tx_kick_start(void)
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
    HAL_UART_Transmit_IT(g_huart, &tx_buf[h], (uint16_t) len);
}*/

/*static size_t uart_tx_write(const uint8_t *data, size_t len)
{
    if (len == 0)
        return 0;

    size_t written = 0;
    taskENTER_CRITICAL();

    while (written < len) {
        size_t space = uart_tx_space();
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

        for (size_t i = 0; i < chunk; ++i) {
            tx_buf[t] = data[written + i];
            t = (t + 1) % TX_BUF_SIZE;
        }
        tx_tail = t;
        written += chunk;
    }

    uart_tx_kick_start();
    taskEXIT_CRITICAL();
    return written;
}*/

/* API funkce */
void uart_dma_init(UART_HandleTypeDef *huart)
{
    g_huart = huart;

    rx_stream = xStreamBufferCreateStatic(sizeof(rx_stream_storage),
                                          1,
                                          rx_stream_storage,
                                          &rx_stream_ctrl);

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_dma_buf, RX_DMA_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
}

/*void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != g_huart->Instance)
        return;

    size_t h = tx_head;
    h = (h + tx_chunk_len) % TX_BUF_SIZE;
    tx_head = h;
    tx_busy = 0;
    tx_chunk_len = 0;
    uart_tx_kick_start();
}*/

/*void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance != g_huart->Instance)
        return;

    BaseType_t xHPW = pdFALSE;
    xStreamBufferSendFromISR(rx_stream, rx_dma_buf, Size, &xHPW);

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_dma_buf, RX_DMA_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    portYIELD_FROM_ISR(xHPW);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance != g_huart->Instance)
        return;

    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_dma_buf, RX_DMA_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
}*/
