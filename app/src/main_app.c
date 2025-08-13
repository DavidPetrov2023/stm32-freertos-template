#include "FreeRTOS.h"
#include "stm32g0xx_hal.h"
#include "task.h"

#include "app_config.h"
#include "board_config.h"

#include "instances/led_instances.h"
#include "interfaces/led_api.h"

// CubeMX-generated init functions (live in Core/Src/*.c)
extern void MX_GPIO_Init(void);

// Provide a weak no-op clock config so the call is always safe.
// If CubeMX generated a real SystemClock_Config(), it overrides this.
__weak void SystemClock_Config(void) {}

#ifndef APP_TASK_STACK
#define APP_TASK_STACK 256 // words (4 bytes each); 128–256 is usually enough for a small task
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
    HAL_Init();           // HAL: Systick/timebase, low-level init
    SystemClock_Config(); // System clocks (PLL/HCLK/PCLK); weak no-op if not provided
    MX_GPIO_Init();       // Must enable GPIO clock and configure LED pin as output

    // Create the blinky task and start the scheduler
    (void) xTaskCreate(BlinkTask, "blink", APP_TASK_STACK, NULL, APP_TASK_PRIO, NULL);
    vTaskStartScheduler();

    // Should never reach here (insufficient heap/RTOS failure would land here)
    for (;;) {
        // Optional: place a breakpoint here if the scheduler fails to start
    }
}
