#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Timeouts in milliseconds. No HAL/RTOS types here. */
typedef struct
{
    void (*init)(void);                               // optional: bool (*init)(void)
    size_t (*write)(const uint8_t *data, size_t len); // non-blocking; returns accepted bytes
    size_t (*read)(uint8_t *dst, size_t maxlen, uint32_t timeout_ms);
    bool (*getc)(uint8_t *ch, uint32_t timeout_ms); // true if 1 byte read
} uart_api_t;

extern const uart_api_t g_uart2;
