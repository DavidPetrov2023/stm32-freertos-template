cmake_minimum_required(VERSION 3.20)

# -------------------------------------------------
# Project settings
# -------------------------------------------------
project(stm32_freertos_template C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -------------------------------------------------
# Toolchain flags (expected by all targets)
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
# Cube + HAL + FreeRTOS library (from /boards/${BOARD}/cube)
# -------------------------------------------------
add_subdirectory(boards/${BOARD})

# -------------------------------------------------
# Interfaces (header-only)
# -------------------------------------------------
add_library(interfaces INTERFACE)
target_include_directories(interfaces INTERFACE
    ${CMAKE_SOURCE_DIR}/interfaces
)

# -------------------------------------------------
# Drivers
# -------------------------------------------------
add_library(led_gpio STATIC
    drivers/led/led_gpio.c
)
target_include_directories(led_gpio PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(led_gpio PUBLIC interfaces cube_fw)

# Další drivery (i2c_stm32, temp_lm75, …) přidáš obdobně:
# add_library(i2c_stm32 STATIC drivers/i2c/i2c_stm32.c)
# target_link_libraries(i2c_stm32 PUBLIC interfaces cube_fw)

# -------------------------------------------------
# Instances (dependency injection layer)
# -------------------------------------------------
add_library(instances STATIC
    instances/led_instances.c
    # instances/i2c_instances.c
    # instances/temp_instances.c
)
target_include_directories(instances PUBLIC
    ${CMAKE_SOURCE_DIR}
)
target_link_libraries(instances PUBLIC interfaces led_gpio)

# -------------------------------------------------
# Application
# -------------------------------------------------
add_executable(app.elf
    app/src/main_app.c
)

target_include_directories(app.elf PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/app/include
    ${CMAKE_SOURCE_DIR}/boards/${BOARD}
)

target_compile_options(app.elf PRIVATE
    ${MCU_FLAGS} ${COMMON_C_FLAGS}
    $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
)

# Link application with board FW (Cube+HAL+FreeRTOS) and OOP instances
target_link_libraries(app.elf PRIVATE
    cube_fw
    instances
)

# Linker flags from board
get_property(_CUBE_LINK_FLAGS GLOBAL PROPERTY CUBE_LINK_FLAGS)
target_link_options(app.elf PRIVATE
    ${_CUBE_LINK_FLAGS}
    -Wl,-Map,${CMAKE_BINARY_DIR}/app.map
    -Wl,--print-memory-usage
)

# If board publishes linker script path, ensure relink on change
get_property(_CUBE_LINKER_SCRIPT GLOBAL PROPERTY CUBE_LINKER_SCRIPT)
if(_CUBE_LINKER_SCRIPT)
    set_property(TARGET app.elf APPEND PROPERTY LINK_DEPENDS ${_CUBE_LINKER_SCRIPT})
endif()

# -------------------------------------------------
# Post-build artifacts
# -------------------------------------------------
find_program(CMAKE_SIZE size)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)

add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.bin
    COMMENT "Generating HEX and BIN files"
    VERBATIM
)

# -------------------------------------------------
# Optional: Code coverage (for host-based tests)
# -------------------------------------------------
option(ENABLE_COVERAGE "Enable code coverage flags" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# -------------------------------------------------
# Tests (host build, optional)
# -------------------------------------------------
# enable_testing()
# add_subdirectory(tests)
