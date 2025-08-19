#include "FreeRTOS.h"
#include "board_config.h"
#include "led_api.h"
#include "led_instances.h"
#include "task.h"

#ifndef APP_TASK_STACK
#define APP_TASK_STACK 256
#endif

#ifndef APP_TASK_PRIO
#define APP_TASK_PRIO (tskIDLE_PRIORITY + 1)
#endif

#ifndef BLINK_DELAY_MS
#define BLINK_DELAY_MS 100u
#endif

static void BlinkTask(void *arg)
{
    (void) arg;
    g_led0.p_api->open(g_led0.p_ctrl, g_led0.p_cfg);

    for (;;) {
        g_led0.p_api->toggle(g_led0.p_ctrl);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main(void)
{
    board_init();

    // Create the blinky task and start the scheduler
    (void) xTaskCreate(BlinkTask, "blink", APP_TASK_STACK, NULL, APP_TASK_PRIO, NULL);
    vTaskStartScheduler();

    // Should never reach here (insufficient heap/RTOS failure would land here)
    for (;;) {
        // Optional: place a breakpoint here if the scheduler fails to start
    }
}
