add_library(led_gpio STATIC
    led_gpio.c
)

# Export driver headers (led_gpio.h) + common API (led_api.h)
target_include_directories(led_gpio PUBLIC
    ${CMAKE_SOURCE_DIR}/drivers/led   # accessed by app/instances
    ${CMAKE_SOURCE_DIR}/interfaces    # led_api.h
)

# Driver depends on HAL (from cube_fw)
target_link_libraries(led_gpio PRIVATE cube_fw)
