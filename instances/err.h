#pragma once
typedef enum e_app_err {
    APP_SUCCESS = 0,
    APP_ERR_ASSERTION,
    APP_ERR_INVALID_ARG,
    APP_ERR_NOT_OPEN,
    APP_ERR_ALREADY_OPEN,
    APP_ERR_IO,
    APP_ERR_TIMEOUT,
} app_err_t;
