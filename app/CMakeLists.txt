# Aplikace (uživatelský kód)
add_executable(app.elf
    src/main_app.c
)

# Hlavičky aplikace + board (kvůli board_config.h, gpio.h, …)
target_include_directories(app.elf PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_SOURCE_DIR}/boards/${BOARD}
)

# C standard pro app
set_property(TARGET app.elf PROPERTY C_STANDARD 11)
set_property(TARGET app.elf PROPERTY C_STANDARD_REQUIRED ON)

# Kompilační volby – sdílené + lepší debug
target_compile_options(app.elf PRIVATE
    ${MCU_FLAGS} ${COMMON_C_FLAGS}
    $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
)

# Link s board FW knihovnou (Cube + HAL + FreeRTOS)
target_link_libraries(app.elf PRIVATE cube_fw)

# Linker flags z boardu (+ map + přehled paměti)
get_property(_CUBE_LINK_FLAGS GLOBAL PROPERTY CUBE_LINK_FLAGS)
target_link_options(app.elf PRIVATE
    ${_CUBE_LINK_FLAGS}
    -Wl,-Map,${CMAKE_BINARY_DIR}/app.map
    -Wl,--print-memory-usage
)

# Pokud board publikuje cestu k .ld, přinuť relink při změně skriptu
get_property(_CUBE_LINKER_SCRIPT GLOBAL PROPERTY CUBE_LINKER_SCRIPT)
if(_CUBE_LINKER_SCRIPT)
  set_property(TARGET app.elf APPEND PROPERTY LINK_DEPENDS ${_CUBE_LINKER_SCRIPT})
endif()

# Post-build artefakty (size/hex/bin)
add_custom_command(TARGET app.elf POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:app.elf>
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:app.elf> ${CMAKE_BINARY_DIR}/app.bin
    COMMENT "Generating HEX and BIN files"
    VERBATIM
)
