#include "board_config.h"
#include "stm32g0xx_hal.h"

extern void SystemClock_Config(void);
extern void MX_GPIO_Init(void);
extern void MX_USART2_UART_Init(void);

// fallback when CubeMX clock configuration is missing
__weak void SystemClock_Config(void) {}

void board_init(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
}