// drivers/led/led_gpio.h
#pragma once
#include "interfaces/led_api.h"
#include "stm32g0xx_hal.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    bool active_high;
    bool is_open;
} led_gpio_ctrl_t;

// API table implemented led_gpio.c
extern const led_api_t g_led_gpio_on_hal;
