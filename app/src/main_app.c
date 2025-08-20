#include "FreeRTOS.h"
#include "board_config.h"
#include "task.h"

#include "led_api.h"
#include "led_instances.h"

#include "usart.h"  // CubeMX: MX_USART2_UART_Init() + huart2
#include <string.h> // strlen

#ifndef APP_TASK_STACK
#define APP_TASK_STACK 256
#endif

#ifndef APP_TASK_PRIO
#define APP_TASK_PRIO (tskIDLE_PRIORITY + 1)
#endif

#ifndef BLINK_DELAY_MS
#define BLINK_DELAY_MS 100u
#endif

#ifndef UART_TASK_STACK
#define UART_TASK_STACK 256
#endif

#ifndef UART_TASK_PRIO
#define UART_TASK_PRIO (tskIDLE_PRIORITY + 2)
#endif

extern UART_HandleTypeDef huart2;

/* --- Blink --- */
static void BlinkTask(void *arg)
{
    (void) arg;
    (void) g_led0.p_api->open(g_led0.p_ctrl, g_led0.p_cfg);

    for (;;) {
        g_led0.p_api->toggle(g_led0.p_ctrl);
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}

/* --- UART test --- */
static void UARTTask(void *arg)
{
    (void) arg;
    const char *msg = "Hello from UART2\r\n";

    for (;;) {
        /* Blocking TX, short timeout to avoid long stalls */
        (void) HAL_UART_Transmit(&huart2, (uint8_t *) msg, (uint16_t) strlen(msg), 100u);
        vTaskDelay(pdMS_TO_TICKS(1000u));
    }
}

int main(void)
{
    board_init(); // HAL_Init + clocks + MX_GPIO_Init + MX_USART2_UART_Init

    /* Create tasks */
    (void) xTaskCreate(BlinkTask, "blink", APP_TASK_STACK, NULL, APP_TASK_PRIO, NULL);
    (void) xTaskCreate(UARTTask, "uart", UART_TASK_STACK, NULL, UART_TASK_PRIO, NULL);

    vTaskStartScheduler();

    /* Should never reach here */
    for (;;) {
        /* Place breakpoint here if scheduler fails to start */
    }
}
