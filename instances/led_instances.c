#include "led_instances.h"
#include "board_pins_stm32g0.h"
#include "led_gpio.h"

static led_gpio_stm32_hal_t g_led0_ctrl;

static const led_gpio_cfg_t g_led0_cfg = {
    .port = LED0_GPIO_Port,
    .pin = LED0_Pin,
    .active_high = true,
};

led_instance_t g_led0 = {
    .p_ctrl = (led_ctrl_t *) &g_led0_ctrl,
    .p_cfg = (const led_cfg_t *) &g_led0_cfg,
    .p_api = &g_led_gpio_on_hal,
};
