cmake_minimum_required(VERSION 3.20)

# -------------------------------------------------
# Project
# -------------------------------------------------
project(stm32_freertos_template C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -------------------------------------------------
# Toolchain flags (shared)
# -------------------------------------------------
set(MCU_FLAGS
    -mcpu=cortex-m0plus
    -mthumb
)

set(COMMON_C_FLAGS
    -ffunction-sections
    -fdata-sections
    -fno-builtin
    -Wall
    -Wextra
)

set(LD_FLAGS
    -Wl,--gc-sections
)

# -------------------------------------------------
# Board selection
# -------------------------------------------------
set(BOARD nucleo_g070rb)

# -------------------------------------------------
# Subdirectories in order: board -> drivers -> instances -> app
# -------------------------------------------------
add_subdirectory(boards/${BOARD})   # creates cube_fw_* and board_bsp
add_subdirectory(drivers/led)       # led_gpio driver (links against cube_fw)
add_subdirectory(drivers/uart)
add_subdirectory(instances)         # g_led0 instance (links against led_gpio)
add_subdirectory(app)               # app.elf (links against board_bsp, instances, cube_fw)
