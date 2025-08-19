// drivers/led/led_gpio.c
#include "led_gpio.h"

// Cast generic controller to specific STM32 HAL-based handle
static inline led_gpio_stm32_hal_t *as_gpio(led_ctrl_t *ctrl)
{
    return (led_gpio_stm32_hal_t *) ctrl;
}

// Initialize LED driver with configuration
static app_err_t led_open(led_ctrl_t *ctrl, const led_cfg_t *cfg)
{
    if (!ctrl || !cfg)
        return APP_ERROR_INVALID_ARG;

    const led_gpio_cfg_t *config = (const led_gpio_cfg_t *) cfg;
    led_gpio_stm32_hal_t *handle = as_gpio(ctrl);

    handle->port = config->port;
    handle->pin = config->pin;
    handle->active_high = config->active_high;
    handle->is_open = true;

    return APP_SUCCESS;
}

// Deinitialize LED driver
static app_err_t led_close(led_ctrl_t *ctrl)
{
    if (!ctrl)
        return APP_ERROR_INVALID_ARG;

    led_gpio_stm32_hal_t *handle = as_gpio(ctrl);
    handle->is_open = false;

    return APP_SUCCESS;
}

// Turn LED on
static app_err_t led_on(led_ctrl_t *ctrl)
{
    if (!ctrl)
        return APP_ERROR_INVALID_ARG;

    led_gpio_stm32_hal_t *handle = as_gpio(ctrl);
    if (!handle->is_open)
        return APP_ERROR_NOT_OPEN;

    HAL_GPIO_WritePin(handle->port,
                      handle->pin,
                      handle->active_high ? GPIO_PIN_SET : GPIO_PIN_RESET);

    return APP_SUCCESS;
}

// Turn LED off
static app_err_t led_off(led_ctrl_t *ctrl)
{
    if (!ctrl)
        return APP_ERROR_INVALID_ARG;

    led_gpio_stm32_hal_t *handle = as_gpio(ctrl);
    if (!handle->is_open)
        return APP_ERROR_NOT_OPEN;

    HAL_GPIO_WritePin(handle->port,
                      handle->pin,
                      handle->active_high ? GPIO_PIN_RESET : GPIO_PIN_SET);

    return APP_SUCCESS;
}

// Toggle LED state
static app_err_t led_toggle(led_ctrl_t *ctrl)
{
    if (!ctrl)
        return APP_ERROR_INVALID_ARG;

    led_gpio_stm32_hal_t *handle = as_gpio(ctrl);
    if (!handle->is_open)
        return APP_ERROR_NOT_OPEN;

    HAL_GPIO_TogglePin(handle->port, handle->pin);

    return APP_SUCCESS;
}

// Export driver API
const led_api_t g_led_gpio_on_hal = {
    .open = led_open,
    .close = led_close,
    .on = led_on,
    .off = led_off,
    .toggle = led_toggle,
};
