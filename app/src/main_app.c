#include "FreeRTOS.h"
#include "stm32g0xx_hal.h"
#include "task.h"

#include "app_config.h"
#include "board_config.h"

// z CubeMX (Core/Src/gpio.c)
extern void MX_GPIO_Init(void);
// z Core/Src/system_stm32g0xx.c nebo main.c (pokud vygenerováno)
extern void SystemClock_Config(void);

static void BlinkTask(void *arg)
{
    (void) arg;
    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        vTaskDelay(pdMS_TO_TICKS(100)); // ms
    }
}

int main(void)
{
    HAL_Init();

#ifdef SystemClock_Config
    SystemClock_Config(); // doporučeno: nastaví PLL/HCLK/PCLK
#endif

    MX_GPIO_Init(); // musí povolit GPIOA clock a PA5 jako output

    // stack je v "words" (4B), 128–256 obvykle stačí
#ifndef APP_TASK_STACK
#define APP_TASK_STACK 256
#endif
#ifndef APP_TASK_PRIO
#define APP_TASK_PRIO (tskIDLE_PRIORITY + 1)
#endif

    xTaskCreate(BlinkTask, "blink", APP_TASK_STACK, NULL, APP_TASK_PRIO, NULL);
    vTaskStartScheduler();

    // sem se nemáme vrátit
    for (;;) {
    }
}
