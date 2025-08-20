// interfaces/uart_api.h
#pragma once
#include "err.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct uart_ctrl uart_ctrl_t;

// Use unique names to avoid collision with HAL macros.
typedef enum {
    UART_PARITY_MODE_NONE = 0,
    UART_PARITY_MODE_EVEN,
    UART_PARITY_MODE_ODD,
} uart_parity_mode_t;

typedef enum {
    UART_STOP_MODE_1 = 0,
    UART_STOP_MODE_2 = 2,
} uart_stop_mode_t;

typedef struct uart_cfg
{
    uint32_t baudrate;
    uart_parity_mode_t parity;
    uart_stop_mode_t stop_bits;
} uart_cfg_t;

typedef struct uart_api
{
    app_err_t (*open)(uart_ctrl_t *p_ctrl, const uart_cfg_t *p_cfg);
    app_err_t (*close)(uart_ctrl_t *p_ctrl);
    int32_t (*write)(uart_ctrl_t *p_ctrl, const uint8_t *data, size_t len, uint32_t timeout_ms);
    int32_t (*read)(uart_ctrl_t *p_ctrl, uint8_t *out, size_t len);
    bool (*rx_available)(uart_ctrl_t *p_ctrl);
} uart_api_t;

typedef struct uart_instance
{
    uart_ctrl_t *p_ctrl;
    const uart_cfg_t *p_cfg;
    const uart_api_t *p_api;
} uart_instance_t;

/* inline wrappers (beznměnné) ... */
