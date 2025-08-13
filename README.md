# STM32 FreeRTOS Template (Nucleo-G070RB)

Minimal CMake + FreeRTOS šablona pro **STM32 Nucleo-G070RB**:
- STM32Cube HAL + FreeRTOS (kernel + port pro CM0+)
- CMake + Ninja, `arm-none-eabi-gcc`
- OpenOCD + VS Code (Cortex-Debug)
- Jednoduché skripty `rebuild.sh` a `flash.sh`

## Požadavky

- `arm-none-eabi-gcc`, `arm-none-eabi-gdb`, `arm-none-eabi-objcopy`, `arm-none-eabi-size`
- `cmake`, `ninja`
- `openocd` (pro debug), nebo `st-flash` (pro rychlé nahrání)
- VS Code + rozšíření **Cortex-Debug**

Ubuntu:
```bash
sudo apt install cmake ninja-build openocd stlink-tools
sudo apt install gcc-arm-none-eabi gdb-multiarch   # nebo balík s arm-none-eabi-gdb
