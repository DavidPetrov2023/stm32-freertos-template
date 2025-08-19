# Application (user code)
add_executable(app.elf
    src/main_app.c
)

# App veřejné headery + API/instances (BEZ boards/${BOARD})
target_include_directories(app.elf PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_SOURCE_DIR}/interfaces
    ${CMAKE_SOURCE_DIR}/instances
)

# C standard
set_property(TARGET app.elf PROPERTY C_STANDARD 11)
set_property(TARGET app.elf PROPERTY C_STANDARD_REQUIRED ON)

# Compile options – sdílené flagy + debug/relwithdebinfo
target_compile_options(app.elf PRIVATE
    ${MCU_FLAGS} ${COMMON_C_FLAGS}
    $<$<CONFIG:Debug>:-Og -g3 -fno-omit-frame-pointer>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g3>
)

# Link: BSP (board_init), instances a CubeFW (startup+HAL+RTOS)
target_link_libraries(app.elf PRIVATE
    board_bsp      # z boards/${BOARD}/CMakeLists.txt
    instances      # 'interfaces' je PUBLIC přes instances → není třeba přidávat
    cube_fw        # alias na cube_fw_${BOARD}
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
