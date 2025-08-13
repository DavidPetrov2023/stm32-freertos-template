# STM32 FreeRTOS Template

Minimal CMake + FreeRTOS template project for STM32 development with VS Code, Ninja, and OpenOCD.

## 📂 Project structure

STM32-FREERTOS-TEMPLATE/
├── app/ # Application code (user logic)
│ ├── include/ # Application headers
│ └── src/ # Application sources
├── boards/ # Board-specific configs (CubeMX, HAL, FreeRTOS)
│ └── nucleo_g070rb/ # Example board
│ ├── cube/ # CubeMX-generated sources
│ ├── board_config.h
│ ├── CMakeLists.txt
│ ├── st_mcu_g0.cfg
│ └── STM32G070.svd
├── build/ # Build output (ignored in Git)
├── CMakeLists.txt # Root CMake configuration
├── flash.sh # Script to flash firmware via OpenOCD
├── rebuild.sh # Clean & rebuild script
├── .vscode/ # VS Code debug/build configs
│ ├── launch.json
│ ├── settings.json
│ └── tasks.json
└── README.md

## 🛠️ Prerequisites

Install required tools:

```bash
sudo apt install cmake ninja-build openocd gcc-arm-none-eabi gdb-multiarch

🚀 Build

./rebuild.sh

Or manually:

mkdir -p build
cd build
cmake -G Ninja ..
ninja

🔥 Flash to board

./flash.sh

🐞 Debug in VS Code

    Connect ST-Link.

    Open the project in VS Code.

    Press F5 or choose Debug (OpenOCD + ST-Link) from the debug menu.

The debugger will stop at main() by default.
📜 License

MIT License
