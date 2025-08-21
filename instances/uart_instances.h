#pragma once
#include "uart_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public instances */
extern const uart_instance_t g_uart2;

/* Call once at boot to wire HAL handle and open UARTs */
void uart_instances_init(void);

#ifdef __cplusplus
}
#endif