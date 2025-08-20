// boards/nucleo_g070rb/board_error.c
#include "stm32g0xx_hal.h"

// Weak fallback used when Cube's main.c is excluded from the build.
// Keep it minimal and safe from any context (ISR-safe, no blocking).
__attribute__((weak)) void Error_Handler(void)
{
    __disable_irq();
    for (;;) {
    }
}

#ifdef USE_FULL_ASSERT
// Optional weak assert handler (kept minimal)
__attribute__((weak)) void assert_failed(uint8_t *file, uint32_t line)
{
    (void) file;
    (void) line;
    __disable_irq();
    for (;;) {
    }
}
#endif
