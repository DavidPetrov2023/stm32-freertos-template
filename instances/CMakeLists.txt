add_library(instances STATIC
    led_instances.c
    uart_instances.c
)

# Public headers that should be visible to other targets (e.g. app):
target_include_directories(instances PUBLIC
    ${CMAKE_SOURCE_DIR}/instances      # led_instances.h
    ${CMAKE_SOURCE_DIR}/interfaces     # led_api.h
)

# Private include for board pin definitions (board_pins_stm32g0.h) – used only when compiling instances:
target_include_directories(instances PRIVATE
    ${CMAKE_SOURCE_DIR}/boards/nucleo_g070rb
)

# Dependencies:
# - PUBLIC: interfaces (propagated further)
# - PRIVATE: led_gpio (driver) + cube_fw (for HAL/CMSIS include paths)
 target_link_libraries(instances
    PRIVATE led_gpio
            cube_fw
    PUBLIC  driver_uart_stm32   # propagates to dependents
 )
