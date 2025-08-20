// app/src/error_handler.c
#include "stm32g0xx_hal.h"

// Strong definition in the app to guarantee availability at link time.
// Keep it minimal, ISR-safe, no blocking.
void Error_Handler(void)
{
    __disable_irq();
    for (;;) {
    }
}
