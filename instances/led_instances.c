// instances/led_instances.c
#include "instances/led_instances.h"
#include "board_config.h"
#include "drivers/led/led_gpio.h"

// driver-specific ctrl
static led_gpio_ctrl_t g_led0_ctrl;

// common cfg (z led_api.h)
static const led_cfg_t g_led0_cfg = {
    .port = (void *) LED_GPIO_Port, // GPIOA
    .pin = LED_Pin,                 // GPIO_PIN_5
    .active_high = true,
};

// globál instance instance
led_instance_t g_led0 = {
    .p_ctrl = (led_ctrl_t *) &g_led0_ctrl,
    .p_cfg = &g_led0_cfg,
    .p_api = &g_led_gpio_on_hal,
};
