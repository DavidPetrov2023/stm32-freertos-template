# Application (user code)
add_executable(app.elf
    src/main_app.c
)

# Public headers from this app + board (for board_config.h, gpio.h, …)
target_include_directories(app.elf PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_SOURCE_DIR}/boards/${BOARD}
)

# C standard for the app
set_property(TARGET app.elf PROPERTY C_STANDARD 11)
set_property(TARGET app.elf PROPERTY C_STANDARD_REQUIRED ON)

# Compile options – reuse shared flags + better debug experience
target_compile_options(app.elf PRIVATE
    ${MCU_FLAGS} ${COMMON_C_FLAGS}
    $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
)

# Link against the board FW library (Cube + HAL + FreeRTOS)
target_link_libraries(app.elf PRIVATE cube_fw)

# Linker flags exported by the board (also produce a .map and print memory usage)
get_property(_CUBE_LINK_FLAGS GLOBAL PROPERTY CUBE_LINK_FLAGS)
target_link_options(app.elf PRIVATE
    ${_CUBE_LINK_FLAGS}
    -Wl,-Map,${CMAKE_BINARY_DIR}/app.map
    -Wl,--print-memory-usage
)

# If the board publishes its .ld path, force relink whenever the linker script changes
get_property(_CUBE_LINKER_SCRIPT GLOBAL PROPERTY CUBE_LINKER_SCRIPT)
if(_CUBE_LINKER_SCRIPT)
  set_property(TARGET app.elf APPEND PROPERTY LINK_DEPENDS ${_CUBE_LINKER_SCRIPT})
endif()

# Post-build artifacts (size/hex/bin)
add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.bin
    COMMENT "Generating HEX and BIN files"
    VERBATIM
)
