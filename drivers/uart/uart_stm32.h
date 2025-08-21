#pragma once
#include "uart_api.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward-declare HAL handle to keep header light */
typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

/* FreeRTOS (for mutex & delays in TX) */
#include "FreeRTOS.h"
#include "semphr.h"

#ifndef UART_RX_BUF_SIZE
#define UART_RX_BUF_SIZE (256u)
#endif

/* Driver control (opaque for the API user) */
typedef struct uart_stm32_ctrl
{
    /* Publicly known as uart_ctrl_t via opaque cast */
    UART_HandleTypeDef *hal; /* must be set by instances before open() */

    /* RX ring buffer (IRQ feeds bytes here) */
    uint8_t rx_buf[UART_RX_BUF_SIZE];
    volatile size_t rx_head;
    volatile size_t rx_tail;

    /* TX: protect HAL blocking transmit so multiple tasks don't interleave */
    SemaphoreHandle_t tx_mutex;
    StaticSemaphore_t tx_mutex_storage;

    /* Cached config (optional, can be used for future reconfig) */
    uart_cfg_t cfg;
} uart_stm32_ctrl_t;

/* Minimal ISR hook: push received byte into ring buffer (no RTOS calls!) */
void uart_stm32_irq_rx_feed(uart_stm32_ctrl_t *ctrl, uint8_t byte);

/* Vtable */
extern const uart_api_t g_uart_stm32_api;

/* Helper to get pointer if you prefer dynamic wiring */
static inline const uart_api_t *uart_get_stm32_api(void)
{
    return &g_uart_stm32_api;
}

#ifdef __cplusplus
}
#endif