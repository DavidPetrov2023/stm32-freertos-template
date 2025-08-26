#include "uart_instances.h"
#include "usart.h"

static void uart2_send(const uint8_t *data, size_t len);

static const uart_dma_api_t g_uart2_api = {
    .send = uart2_send,
};

typedef struct
{
    UART_HandleTypeDef *huart;
} uart_dma_ctrl_t;

static uart_dma_ctrl_t g_uart2_ctrl = {
    .huart = &huart2,
};

const uart_dma_instance_t g_uart2 = {.p_api = &g_uart2_api, .p_ctrl = &g_uart2_ctrl, .p_cfg = NULL};

static void uart2_send(const uint8_t *data, size_t len)
{
    HAL_UART_Transmit_DMA(g_uart2_ctrl.huart, (uint8_t *) data, len);
}
