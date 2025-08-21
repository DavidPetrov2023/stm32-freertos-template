// interfaces/uart_api.h
#pragma once
#include "app_err.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque control for API */
typedef struct uart_ctrl uart_ctrl_t;

/* UART config */
typedef enum {
    UART_PARITY_MODE_NONE = 0,
    UART_PARITY_MODE_EVEN,
    UART_PARITY_MODE_ODD,
} uart_parity_mode_t;

typedef enum {
    UART_STOP_MODE_1 = 0,
    UART_STOP_MODE_2,
} uart_stop_mode_t;

typedef struct
{
    uint32_t baudrate;
    uart_parity_mode_t parity;
    uart_stop_mode_t stop_bits;
} uart_cfg_t;

/* Return codes are app_err_t (0 == OK, <0 == error) where useful, and/or sizes. */
typedef struct
{
    app_err_t (*open)(uart_ctrl_t *p_ctrl, const uart_cfg_t *p_cfg);
    app_err_t (*close)(uart_ctrl_t *p_ctrl);

    /* Blocking write with timeout (ms). Returns bytes sent (>=0) or APP_ERR_* (<0) */
    int32_t (*write)(uart_ctrl_t *p_ctrl, const uint8_t *data, size_t len, uint32_t timeout_ms);

    /* Non-blocking read: pop up to len bytes from internal RX ring. Returns bytes read (>=0). */
    int32_t (*read)(uart_ctrl_t *p_ctrl, uint8_t *out, size_t len);

    /* True if any RX data is buffered. */
    bool (*rx_available)(uart_ctrl_t *p_ctrl);
} uart_api_t;

/* Instance bundles ctrl + cfg + vtable */
typedef struct uart_instance
{
    uart_ctrl_t *p_ctrl;
    const uart_cfg_t *p_cfg;
    const uart_api_t *p_api;
} uart_instance_t;

#ifdef __cplusplus
}
#endif