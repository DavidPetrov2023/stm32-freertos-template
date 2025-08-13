// drivers/led/led_gpio.c
#include "drivers/led/led_gpio.h"
#include "board_config.h"

static inline led_gpio_ctrl_t *as_gpio(led_ctrl_t *p)
{
    return (led_gpio_ctrl_t *) p;
}

static app_err_t led_open(led_ctrl_t *p, const led_cfg_t *cfg)
{
    if (!p || !cfg)
        return APP_ERROR_INVALID_ARG;
    led_gpio_ctrl_t *h = as_gpio(p);
    h->port = (GPIO_TypeDef *) cfg->port;
    h->pin = cfg->pin;
    h->active_high = cfg->active_high;
    h->is_open = true;
    return APP_SUCCESS;
}

static app_err_t led_close(led_ctrl_t *p)
{
    if (!p)
        return APP_ERROR_INVALID_ARG;
    led_gpio_ctrl_t *h = as_gpio(p);
    h->is_open = false;
    return APP_SUCCESS;
}

static app_err_t led_on(led_ctrl_t *p)
{
    if (!p)
        return APP_ERROR_INVALID_ARG;
    led_gpio_ctrl_t *h = as_gpio(p);
    if (!h->is_open)
        return APP_ERROR_NOT_OPEN;
    HAL_GPIO_WritePin(h->port, h->pin, h->active_high ? GPIO_PIN_SET : GPIO_PIN_RESET);
    return APP_SUCCESS;
}

static app_err_t led_off(led_ctrl_t *p)
{
    if (!p)
        return APP_ERROR_INVALID_ARG;
    led_gpio_ctrl_t *h = as_gpio(p);
    if (!h->is_open)
        return APP_ERROR_NOT_OPEN;
    HAL_GPIO_WritePin(h->port, h->pin, h->active_high ? GPIO_PIN_RESET : GPIO_PIN_SET);
    return APP_SUCCESS;
}

static app_err_t led_toggle(led_ctrl_t *p)
{
    if (!p)
        return APP_ERROR_INVALID_ARG;
    led_gpio_ctrl_t *h = as_gpio(p);
    if (!h->is_open)
        return APP_ERROR_NOT_OPEN;
    HAL_GPIO_TogglePin(h->port, h->pin);
    return APP_SUCCESS;
}

const led_api_t g_led_gpio_on_hal = {
    .open = led_open,
    .close = led_close,
    .on = led_on,
    .off = led_off,
    .toggle = led_toggle,
};
