// drivers/led/led_gpio.h
#pragma once
#include "led_api.h"
#include "stm32g0xx_hal.h"
#include <stdbool.h>

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    bool active_high;
} led_gpio_cfg_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    bool active_high;
    bool is_open;
} led_gpio_stm32_hal_t;

extern const led_api_t g_led_gpio_on_hal;
