#include "uart_instances.h"
#include "uart/uart_stm32.h"
#include "usart.h" /* brings in UART_HandleTypeDef huart2 from Cube */

extern UART_HandleTypeDef huart2;

/* Concrete control object lives here */
uart_stm32_ctrl_t g_uart2_ctrl;

static const uart_cfg_t g_uart2_cfg = {
    .baudrate = 115200u,
    .parity = UART_PARITY_MODE_NONE,
    .stop_bits = UART_STOP_MODE_1,
};

const uart_instance_t g_uart2 = {
    .p_ctrl = (uart_ctrl_t *) &g_uart2_ctrl,
    .p_cfg = &g_uart2_cfg,
    .p_api = &g_uart_stm32_api,
};

void uart_instances_init(void)
{
    /* Wire HAL handle before opening */
    g_uart2_ctrl.hal = &huart2;
    (void) g_uart2.p_api->open(g_uart2.p_ctrl, g_uart2.p_cfg);
}