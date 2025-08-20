// instances/uart_instances.c
#include "uart_instances.h"
#include "uart/uart_stm32.h"

// Public control block (owned by instances layer; driver holds only pointer)
uart_stm32_ctrl_t g_uart2_ctrl;

static const uart_cfg_t g_uart2_cfg = {
    .baudrate = 115200u,
    .parity = UART_PARITY_MODE_NONE,
    .stop_bits = UART_STOP_MODE_1,
};

uart_instance_t g_uart2 = {
    .p_ctrl = (uart_ctrl_t *) &g_uart2_ctrl,
    .p_cfg = &g_uart2_cfg,
    .p_api = &g_uart_stm32_api,
};
