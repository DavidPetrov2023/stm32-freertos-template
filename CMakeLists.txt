cmake_minimum_required(VERSION 3.20)

project(stm32-freertos-template C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Společné flagy
set(MCU_FLAGS -mcpu=cortex-m0plus -mthumb)
set(COMMON_C_FLAGS
  -ffunction-sections -fdata-sections -fno-builtin
  -Wall -Wextra
  $<$<CONFIG:Debug>:-Og -g3 -DDEBUG>
)
set(LD_FLAGS -Wl,--gc-sections)
set(CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG} -g3")

# Cílová deska
set(BOARD nucleo_g070rb CACHE STRING "Target board")

# Nástroje
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE    arm-none-eabi-size    REQUIRED)

# Sdílení flagů pro podprojekty
set_property(GLOBAL PROPERTY MCU_FLAGS       "${MCU_FLAGS}")
set_property(GLOBAL PROPERTY COMMON_C_FLAGS "${COMMON_C_FLAGS}")
set_property(GLOBAL PROPERTY LD_FLAGS       "${LD_FLAGS}")

# Board knihovna a aplikace
add_subdirectory(boards/${BOARD})
add_subdirectory(app)
