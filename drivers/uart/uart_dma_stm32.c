#include "uart_dma_stm32.h"
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "task.h"
#include <string.h>

#ifndef UART_TASK_STACK
#define UART_TASK_STACK 256
#endif

#ifndef UART_TASK_PRIO
#define UART_TASK_PRIO (tskIDLE_PRIORITY + 2)
#endif

/* --- UART konfigurace --- */
#define RX_DMA_SIZE 128u            // velikost DMA RX bufferu
#define RX_STREAM_STORAGE_SIZE 512u // kapacita stream bufferu (pro příjem do tasku)
#define LINE_BUF_SIZE 64u           // max délka jedné textové řádky
#define HELLO_PERIOD_MS 1000u       // perioda odeslání "ahoj\r\n"
#define TX_BUF_SIZE 256u            // TX ring buffer

/* --- RX --- */
static uint8_t rx_dma_buf[RX_DMA_SIZE];
static StreamBufferHandle_t rxStream;

static StaticStreamBuffer_t rxStreamCtrl;
static uint8_t rxStreamStorage[RX_STREAM_STORAGE_SIZE];

/* --- TX --- */
static uint8_t tx_buf[TX_BUF_SIZE];
static volatile size_t tx_head = 0;
static volatile size_t tx_tail = 0;
static volatile uint8_t tx_busy = 0;
static volatile size_t tx_chunk_len = 0;

extern UART_HandleTypeDef huart2;

/* --- TX driver (non-blocking) --- */
static inline size_t uart_tx_space(void)
{
    size_t h = tx_head;
    size_t t = tx_tail;
    return (t >= h) ? (TX_BUF_SIZE - (t - h) - 1) : (h - t - 1);
}

static void uart_tx_kick_start(void)
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

    HAL_UART_Transmit_IT(&huart2, &tx_buf[h], (uint16_t) len);
}

static size_t uart_tx_write(const uint8_t *data, size_t len)
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
}

/* --- UART Task --- */
static void UartTask(void *arg)
{
    (void) arg;
    uint8_t line[LINE_BUF_SIZE];
    size_t idx = 0;

    static const uint8_t hello_msg[] = "ahoj\r\n";
    TickType_t lastHello = xTaskGetTickCount();

    for (;;) {
        /* 1) Periodický výstup */
        if (xTaskGetTickCount() - lastHello >= pdMS_TO_TICKS(HELLO_PERIOD_MS)) {
            uart_tx_write(hello_msg, sizeof(hello_msg) - 1);
            lastHello = xTaskGetTickCount();
        }

        /* 2) Čti ze stream bufferu (timeout 20 ms) */
        uint8_t ch;
        if (xStreamBufferReceive(rxStream, &ch, 1, pdMS_TO_TICKS(20)) == 1) {
            if (ch == '\r' || ch == '\n') {
                if (idx > 0) {
                    uart_tx_write(line, idx);
                    uart_tx_write((const uint8_t *) "\r\n", 2);
                    idx = 0;
                }
            } else if (idx < sizeof(line)) {
                line[idx++] = ch;
            }
        }
    }
}

void uart_dma_init(void)
{
    xTaskCreate(UartTask, "uart", UART_TASK_STACK, NULL, UART_TASK_PRIO, NULL);

    rxStream = xStreamBufferCreateStatic(RX_STREAM_STORAGE_SIZE, 1, rxStreamStorage, &rxStreamCtrl);

    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buf, RX_DMA_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT); // vypnout half-transfer interrupt
}

/* --- Callbacks (HAL) --- */

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2) {
        BaseType_t xHPW = pdFALSE;

        xStreamBufferSendFromISR(rxStream, rx_dma_buf, Size, &xHPW);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buf, RX_DMA_SIZE);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

        portYIELD_FROM_ISR(xHPW);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_dma_buf, RX_DMA_SIZE);
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        size_t h = tx_head;
        h = (h + tx_chunk_len) % TX_BUF_SIZE;
        tx_head = h;

        tx_busy = 0;
        tx_chunk_len = 0;

        uart_tx_kick_start();
    }
}