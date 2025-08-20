# drivers/uart/CMakeLists.txt
add_library(driver_uart_stm32 STATIC
    uart_stm32.c
)

target_include_directories(driver_uart_stm32 PUBLIC
    ${CMAKE_SOURCE_DIR}/interfaces
    ${CMAKE_SOURCE_DIR}/drivers         # allows #include "uart/uart_stm32.h"
    ${CMAKE_SOURCE_DIR}/instances       # uart_api.h includes instances/err.h
)

target_link_libraries(driver_uart_stm32 PUBLIC
    cube_fw
)
