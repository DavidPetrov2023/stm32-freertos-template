// interfaces/app_err.h
#pragma once
#include <stdint.h>

/* Unified app error type: 0 == success, <0 == error */
typedef int32_t app_err_t;

#define APP_SUCCESS (0)

/* Generic errors used v UART driveru */
#define APP_ERR_INVALID_ARG (-1)
#define APP_ERR_TIMEOUT (-2)
#define APP_ERR_NO_RESOURCE (-3)
#define APP_ERR_IO (-4)
