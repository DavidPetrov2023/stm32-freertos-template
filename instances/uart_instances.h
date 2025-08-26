#pragma once
#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Init instances layer: registers RX callback, creates StreamBuffer, binds to &huart2 */
void uart_instances_init(void);

/* Thin wrappers for app layer (RTOS-friendly) */
size_t uart_instances_write(const uint8_t *data, size_t len);

/* Receive up to maxlen bytes, waits up to `to` ticks, returns count */
size_t uart_instances_read(uint8_t *dst, size_t maxlen, TickType_t to);

/* Helper for single char with timeout; returns true if got 1 byte */
static inline bool uart_instances_getc(uint8_t *ch, TickType_t to)
{
    return uart_instances_read(ch, 1, to) == 1;
}
