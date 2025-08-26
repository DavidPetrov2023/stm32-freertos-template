#include "uart_app.h"
#include "uart/uart_dma_stm32.h"

void uart_app_start(void)
{
    uart_dma_init();
}
