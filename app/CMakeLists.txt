# Application (user code)
add_executable(app.elf
    src/main_app.c
    src/led_app.c
    src/error_handler.c   
    ${CMAKE_SOURCE_DIR}/boards/nucleo_g070rb/cube/Core/Src/stm32g0xx_it.c
)

target_include_directories(app.elf PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_SOURCE_DIR}/interfaces
    ${CMAKE_SOURCE_DIR}/instances
    ${CMAKE_SOURCE_DIR}/drivers/uart
)

set_property(TARGET app.elf PROPERTY C_STANDARD 11)
set_property(TARGET app.elf PROPERTY C_STANDARD_REQUIRED ON)

target_compile_options(app.elf PRIVATE
    ${MCU_FLAGS} ${COMMON_C_FLAGS}
    $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
)

target_link_libraries(app.elf PRIVATE
    instances
    led_gpio
    cube_fw
    board_bsp
    driver_uart_stm32
)

# Linker flags exportované z Cube
get_property(_CUBE_LINK_FLAGS GLOBAL PROPERTY CUBE_LINK_FLAGS)
target_link_options(app.elf PRIVATE
    ${_CUBE_LINK_FLAGS}
    -Wl,-Map,${CMAKE_BINARY_DIR}/app.map
    -Wl,--print-memory-usage
)
# Rebuild při změně linker skriptu (pokud publikován)
get_property(_CUBE_LINKER_SCRIPT GLOBAL PROPERTY CUBE_LINKER_SCRIPT)
if(_CUBE_LINKER_SCRIPT)
  set_property(TARGET app.elf APPEND PROPERTY LINK_DEPENDS ${_CUBE_LINKER_SCRIPT})
endif()

# Post-build artefakty
find_program(CMAKE_SIZE size)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy)
add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.bin
    COMMENT "Generating HEX and BIN files"
    VERBATIM
)
