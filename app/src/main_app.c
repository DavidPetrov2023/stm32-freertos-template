#include "FreeRTOS.h"
#include "board_config.h"
#include "stream_buffer.h"
#include "task.h"

#include "led_api.h"
#include "led_instances.h"
#include "usart.h" // CubeMX: MX_USART2_UART_Init() + huart2

#include <string.h>

/* --- Task konfigurace --- */
#ifndef APP_TASK_STACK
#define APP_TASK_STACK 256
#endif

#ifndef APP_TASK_PRIO
#define APP_TASK_PRIO (tskIDLE_PRIORITY + 1)
#endif

#ifndef BLINK_DELAY_MS
#define BLINK_DELAY_MS 500u
#endif

#ifndef UART_SEND_DELAY_MS
#define UART_SEND_DELAY_MS 1000u
#endif

/* --- Blink Task --- */
static void BlinkTask(void *arg)
{
    (void) arg;
    g_led0.p_api->open(g_led0.p_ctrl, g_led0.p_cfg);

    for (;;) {
        g_led0.p_api->toggle(g_led0.p_ctrl);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}

/* --- main() --- */

int main(void)
{
    board_init(); // HAL_Init + MX_GPIO + MX_DMA + MX_USART2_UART_Init

    xTaskCreate(BlinkTask, "blink", APP_TASK_STACK, NULL, APP_TASK_PRIO, NULL);

    vTaskStartScheduler();

    while (1) {
        // Nemělo by nastat – fallback
    }
}
