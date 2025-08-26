#include "FreeRTOS.h"
#include "board_config.h"
#include "task.h"

#include "led_app.h"
#include "uart_app.h"

/* --- main() --- */
int main(void)
{
    board_init(); // HAL_Init + MX_GPIO + MX_DMA + MX_USART2_UART_Init
    uart_app_start();
    led_app_start();

    vTaskStartScheduler();

    while (1) {
        // Nemělo by nastat – fallback
    }
}
