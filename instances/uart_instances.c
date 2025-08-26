#include "uart_instances.h"
#include "stream_buffer.h"
#include "uart/uart_dma_stm32.h"
#include "usart.h" // extern UART_HandleTypeDef huart2

#ifndef RX_STREAM_STORAGE_SIZE
#define RX_STREAM_STORAGE_SIZE 512u
#endif

/* --- StreamBuffer owned by instances layer --- */
static StaticStreamBuffer_t s_rxStreamCtrl;
static uint8_t s_rxStreamStorage[RX_STREAM_STORAGE_SIZE];
static StreamBufferHandle_t s_rxStream;

/* RX callback provided to low-level driver (ISR-safe) */
static void rx_cb(const uint8_t *data, size_t len, bool from_isr, void *ctx)
{
    (void) ctx;
    if (!data || !len)
        return;

    if (from_isr) {
        BaseType_t hpw = pdFALSE;
        (void) xStreamBufferSendFromISR(s_rxStream, data, len, &hpw);
        portYIELD_FROM_ISR(hpw);
    } else {
        (void) xStreamBufferSend(s_rxStream, data, len, 0);
    }
}

void uart_instances_init(void)
{
    /* Create RX StreamBuffer (static, no heap) */
    s_rxStream = xStreamBufferCreateStatic(RX_STREAM_STORAGE_SIZE,
                                           1, /* trigger level */
                                           s_rxStreamStorage,
                                           &s_rxStreamCtrl);

    /* Bind to HAL handle &huart2 and register callback */
    uart_dma_init(rx_cb, NULL);
}

size_t uart_instances_write(const uint8_t *data, size_t len)
{
    return uart_dma_write(data, len);
}

size_t uart_instances_read(uint8_t *dst, size_t maxlen, TickType_t to)
{
    return xStreamBufferReceive(s_rxStream, dst, maxlen, to);
}
