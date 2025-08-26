#pragma once
#include "stm32g0xx_hal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* User RX callback signature: delivers received chunk from ISR or thread. */
typedef void (*uart_rx_cb_t)(const uint8_t *data, size_t len, bool from_isr, void *user_ctx);

/* Initialize UART DMA/IT driver.
 * - huart: HAL UART handle (e.g., &huart2)
 * - rx_cb: user RX callback (can be NULL)
 * - user_ctx: opaque pointer passed back to rx_cb
 */
void uart_dma_init(uart_rx_cb_t rx_cb, void *user_ctx);

/* Non-blocking TX write. Copies to internal ring buffer and kicks IT transmit. */
size_t uart_dma_write(const uint8_t *data, size_t len);
