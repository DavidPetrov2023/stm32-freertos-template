// interfaces/led_api.h
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef int32_t app_err_t;
enum {
    APP_SUCCESS = 0,
    APP_ERROR_INVALID_ARG = -1,
    APP_ERROR_NOT_OPEN = -2,
};

typedef struct led_ctrl led_ctrl_t;

typedef struct led_cfg
{
    void *port;
    uint16_t pin;
    bool active_high;
} led_cfg_t;

typedef struct led_api
{
    app_err_t (*open)(led_ctrl_t *p_ctrl, const led_cfg_t *p_cfg);
    app_err_t (*close)(led_ctrl_t *p_ctrl);
    app_err_t (*on)(led_ctrl_t *p_ctrl);
    app_err_t (*off)(led_ctrl_t *p_ctrl);
    app_err_t (*toggle)(led_ctrl_t *p_ctrl);
} led_api_t;

typedef struct led_instance
{
    led_ctrl_t *p_ctrl;
    const led_cfg_t *p_cfg;
    const led_api_t *p_api;
} led_instance_t;
