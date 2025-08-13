# HAL + FreeRTOS library for NUCLEO-G070RB

# ---- inputs from top-level ----
# expects these to be already set in top-level CMakeLists:
#   set(MCU_FLAGS -mcpu=cortex-m0plus -mthumb)
#   set(COMMON_C_FLAGS -ffunction-sections -fdata-sections -fno-builtin -Wall -Wextra)
#   set(LD_FLAGS -Wl,--gc-sections)

set(BOARD nucleo_g070rb)
set(CUBE_DIR ${CMAKE_CURRENT_LIST_DIR}/cube)

# --- startup & linker script ---
file(GLOB STARTUP_S ${CUBE_DIR}/startup*.s)
file(GLOB LINKER_LD ${CUBE_DIR}/*FLASH*.ld)

# --- Core (bez main.c) ---
file(GLOB_RECURSE CORE_SRCS
    ${CUBE_DIR}/Core/Src/*.c
)
# vyhoď pouze main.c – zbytek (gpio.c, app_freertos.c, …) nech
list(FILTER CORE_SRCS EXCLUDE REGEX ".*/Core/Src/main\\.c$")

# --- HAL sources ---
file(GLOB_RECURSE HAL_SRCS
    ${CUBE_DIR}/Drivers/STM32G0xx_HAL_Driver/Src/*.c
)
# vyhoď RTC timebase templaty generované CubeMX (dělají problémy)
list(FILTER HAL_SRCS EXCLUDE REGEX ".*/stm32g0xx_hal_timebase_rtc_.*_template\\.c$")

# --- FreeRTOS kernel ---
file(GLOB FREERTOS_KERNEL
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/*.c
)

# --- FreeRTOS port (ARM_CM0 / ARM_CM0PLUS – vezmeme port.c v podadresáři) ---
file(GLOB FREERTOS_PORT
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/*/port.c
)

# --- FreeRTOS heap (libovolný heap_X.c, typicky heap_4.c) ---
file(GLOB FREERTOS_HEAP
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap*.c
)

# --- Volitelně CMSIS-RTOS v2 ---
file(GLOB FREERTOS_CMSISV2
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/*.c
)

# --- Knihovna desky ---
add_library(cube_fw_${BOARD} STATIC
    ${STARTUP_S}
    ${CORE_SRCS}
    ${HAL_SRCS}
    ${FREERTOS_KERNEL}
    ${FREERTOS_PORT}
    ${FREERTOS_HEAP}
    ${FREERTOS_CMSISV2}
)
add_library(cube_fw ALIAS cube_fw_${BOARD})

# --- Include paths ---
target_include_directories(cube_fw_${BOARD} PUBLIC
    ${CUBE_DIR}/Core/Inc
    ${CUBE_DIR}/Drivers/CMSIS/Include
    ${CUBE_DIR}/Drivers/CMSIS/Device/ST/STM32G0xx/Include
    ${CUBE_DIR}/Drivers/STM32G0xx_HAL_Driver/Inc
    ${CUBE_DIR}/Drivers/STM32G0xx_HAL_Driver/Inc/Legacy
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM0PLUS
    ${CUBE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
)

# --- Definice & flagy ---
target_compile_definitions(cube_fw_${BOARD} PUBLIC
    STM32G070xx
    USE_HAL_DRIVER
)
target_compile_options(cube_fw_${BOARD} PUBLIC
    ${MCU_FLAGS}
    ${COMMON_C_FLAGS}
)

# --- předáme linkovací flagy aplikaci přes GLOBAL property ---
# DŮLEŽITÉ: -T<soubor> musí být jedna položka listu, proto bez mezery po -T
set(CUBE_LINK_FLAGS
    ${MCU_FLAGS}
    ${LD_FLAGS}
    -T${LINKER_LD}
)
set_property(GLOBAL PROPERTY CUBE_LINK_FLAGS "${CUBE_LINK_FLAGS}")
