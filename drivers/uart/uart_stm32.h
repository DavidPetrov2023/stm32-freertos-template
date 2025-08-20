// drivers/uart/uart_stm32.h
#pragma once
#include "uart_api.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// FreeRTOS types (kept in driver; API stays RTOS-agnostic)
#include "FreeRTOS.h"
#include "semphr.h"

#ifndef UART_RX_BUF_SIZE
#define UART_RX_BUF_SIZE (128u)
#endif

// Opaque API control forward
struct uart_ctrl
{
    uint8_t _;
};

// Driver-specific control block for STM32 HAL backend
typedef struct
{
    // Configuration snapshot
    uint32_t baudrate;
    uart_parity_mode_t parity;
    uart_stop_mode_t stop_bits;
    // State
    volatile bool is_open;

    // RX ring buffer (static storage, no malloc)
    uint8_t rx_buf[UART_RX_BUF_SIZE];
    volatile size_t rx_head;
    volatile size_t rx_tail;

    // TX synchronization (mutex)
    SemaphoreHandle_t tx_mutex;
    StaticSemaphore_t tx_mutex_storage;
} uart_stm32_ctrl_t;

// ISR hook to push received byte into ring buffer (minimal ISR work)
void uart_stm32_irq_rx_feed(uart_stm32_ctrl_t *ctrl, uint8_t byte);

// Vtable (implementation)
extern const uart_api_t g_uart_stm32_api;
