#pragma once

#include "stm32g0xx_hal.h"
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    void (*send)(const uint8_t *data, size_t len);
} uart_dma_api_t;

typedef struct
{
    const uart_dma_api_t *p_api;
    void *p_ctrl;
    void *p_cfg;
} uart_dma_instance_t;

void uart_dma_init(void);
