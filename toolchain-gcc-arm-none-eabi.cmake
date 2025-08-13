# toolchain-gcc-arm-none-eabi.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# DŮLEŽITÉ: zabránit try-compile spouštět host toolchain
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Kompilátory
set(CMAKE_C_COMPILER   arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# Volby pro ARM Cortex-M0+
set(MCU_FLAGS "-mcpu=cortex-m0plus -mthumb")
set(CMAKE_C_FLAGS_INIT "${MCU_FLAGS} -ffunction-sections -fdata-sections -fno-builtin -Wall -Wextra")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")

# Linker
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--gc-sections")
