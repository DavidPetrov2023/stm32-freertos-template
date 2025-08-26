#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"

#include "uart/uart_dma_stm32.h" // low-level driver (HAL-only)
#include "uart_api.h"            // clean API exposed to app
#include "usart.h"               // extern UART_HandleTypeDef huart2

#ifndef RX_STREAM_STORAGE_SIZE
#define RX_STREAM_STORAGE_SIZE 512u
#endif

/* --- Per-instance storage --- */
static StaticStreamBuffer_t s_rxStreamCtrl;
static uint8_t s_rxStreamStorage[RX_STREAM_STORAGE_SIZE];
static StreamBufferHandle_t s_rxStream;

/* RX callback invoked by the low-level driver (ISR-safe). */
static void rx_cb(const uint8_t *data, size_t len, bool from_isr, void *ctx)
{
    (void) ctx;
    if (!data || len == 0)
        return;

    if (from_isr) {
        BaseType_t hpw = pdFALSE;
        (void) xStreamBufferSendFromISR(s_rxStream, data, len, &hpw);
        portYIELD_FROM_ISR(hpw);
    } else {
        (void) xStreamBufferSend(s_rxStream, data, len, 0);
    }
}

/* ---- API methods ---- */
static void api_init(void)
{
    /* Create StreamBuffer statically (no heap). */
    s_rxStream = xStreamBufferCreateStatic(RX_STREAM_STORAGE_SIZE,
                                           1, /* trigger level */
                                           s_rxStreamStorage,
                                           &s_rxStreamCtrl);
    configASSERT(s_rxStream != NULL);

    /* Bind driver to a specific UART handle and register the RX callback. */
    uart_dma_init(&huart2, rx_cb, NULL);
}

static size_t api_write(const uint8_t *data, size_t len)
{
    return uart_dma_write(data, len);
}

static size_t api_read(uint8_t *dst, size_t maxlen, uint32_t timeout_ms)
{
    TickType_t to = (timeout_ms == (uint32_t) portMAX_DELAY) ? portMAX_DELAY
                                                             : pdMS_TO_TICKS(timeout_ms);
    return xStreamBufferReceive(s_rxStream, dst, maxlen, to);
}

static bool api_getc(uint8_t *ch, uint32_t timeout_ms)
{
    return api_read(ch, 1, timeout_ms) == 1;
}

/* ---- Public API instance ---- */
const uart_api_t g_uart2 = {
    .init = api_init,
    .write = api_write,
    .read = api_read,
    .getc = api_getc,
};
