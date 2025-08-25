#include "FreeRTOS.h"
#include "board_config.h"
#include "task.h"

#include "led_api.h"
#include "led_instances.h"

#include "usart.h" // CubeMX: MX_USART2_UART_Init() + huart2
#include <string.h>

#ifndef APP_TASK_STACK
#define APP_TASK_STACK 256
#endif

#ifndef APP_TASK_PRIO
#define APP_TASK_PRIO (tskIDLE_PRIORITY + 1)
#endif

#ifndef UART_TASK_STACK
#define UART_TASK_STACK 256
#endif

#ifndef UART_TASK_PRIO
#define UART_TASK_PRIO (tskIDLE_PRIORITY + 2)
#endif

#ifndef BLINK_DELAY_MS
#define BLINK_DELAY_MS 500u
#endif

#ifndef UART_SEND_DELAY_MS
#define UART_SEND_DELAY_MS 1000u
#endif

extern UART_HandleTypeDef huart2;

/* --- Blink Task --- */
static void BlinkTask(void *arg)
{
    (void) arg;
    (void) g_led0.p_api->open(g_led0.p_ctrl, g_led0.p_cfg);

    for (;;) {
        g_led0.p_api->toggle(g_led0.p_ctrl);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}

/* --- UART TX Task --- */
static void UartTask(void *arg)
{
    (void) arg;
    const char *msg = "Hello from UART2 (DMA)\r\n";

    for (;;) {
        // Počká, až nebude DMA zaneprázdněné
        if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY) {
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *) msg, strlen(msg));
        }

        vTaskDelay(pdMS_TO_TICKS(UART_SEND_DELAY_MS));
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        // přenos byl dokončen – můžeš zapnout LED, nebo debug výpis
    }
}

int main(void)
{
    board_init(); // HAL_Init + MX_GPIO + MX_USART2 + MX_DMA

    (void) xTaskCreate(BlinkTask, "blink", APP_TASK_STACK, NULL, APP_TASK_PRIO, NULL);
    (void) xTaskCreate(UartTask, "uart", UART_TASK_STACK, NULL, UART_TASK_PRIO, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
