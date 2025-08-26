#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Clean UART API (no HAL/FreeRTOS types here). Timeouts in milliseconds. */
typedef struct
{
    /* Initialize the UART instance (binds driver, sets up buffers, callbacks). */
    void (*init)(void);

    /* Non-blocking write, returns bytes accepted to TX queue. */
    size_t (*write)(const uint8_t *data, size_t len);

    /* Receive up to maxlen bytes, waits up to timeout_ms. Returns count. */
    size_t (*read)(uint8_t *dst, size_t maxlen, uint32_t timeout_ms);

    /* Receive one byte with timeout. Returns true if a byte was read. */
    bool (*getc)(uint8_t *ch, uint32_t timeout_ms);
} uart_api_t;

/* Public instance (like g_led0). */
extern const uart_api_t g_uart2;
