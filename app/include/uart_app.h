#pragma once
#include "stm32g0xx_hal.h"

/* Start UART app/service:
 * - creates RX stream buffer
 * - registers RX callback into driver
 * - spawns UART task
 */
void uart_app_start(void);
