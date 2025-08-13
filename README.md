# STM32 FreeRTOS Template with OOP-like Driver Structure

This repository provides a modern STM32 FreeRTOS project template that follows a modular and OOP-like design, inspired by Renesas and layered architecture best practices.

## ✨ Features

- **FreeRTOS** integration for STM32
- **OOP-like driver structure** using `interfaces`, `instances`, and `drivers`
- **Board abstraction** through `board_config.h` and dedicated board directories
- **HAL (CubeMX) + FreeRTOS middleware** integrated as a CMake library
- **Unit testing** support with GoogleTest (GTest) and optional coverage reports
- **Cross-platform build** with `CMake` + `Ninja` + `arm-none-eabi-gcc`
- **Debug ready** with OpenOCD and ST-Link (VS Code `cortex-debug`)

## 📂 Project Structure

.
├── boards/ # Board-specific code and CubeMX generated sources
│ └── nucleo_g070rb/
│ ├── cube/ # HAL + FreeRTOS from CubeMX
│ ├── st_mcu_g0.cfg # OpenOCD configuration
│ └── board_config.h # Pin definitions and board setup
│
├── drivers/ # Low-level drivers
│ └── led/
│ ├── led_gpio.c
│ └── led_gpio.h
│
├── interfaces/ # Abstract interfaces (header-only contracts)
│ └── led_interface.h
│
├── instances/ # Concrete instances binding drivers to hardware
│ └── led_instances.c
│
├── src/ # Application source code
│ └── main_app.c
│
├── tests/ # Unit tests with GoogleTest
│
├── CMakeLists.txt # Root CMake configuration
├── rebuild.sh # Build helper script
├── flash.sh # Flash helper script
└── README.md


## 🚀 Building the Project

Make sure you have the following installed:

- `arm-none-eabi-gcc` toolchain
- `CMake` (>= 3.20)
- `Ninja` build system
- `OpenOCD` or `st-flash`

### Build
```bash
./rebuild.sh

Flash

./flash.sh

Debug in VS Code

    Install the Cortex-Debug extension.

    Open the project folder in VS Code.

    Press F5 to start debugging.

🧪 Running Unit Tests

Build and run tests on your host machine:

cmake -S tests -B build-tests
cmake --build build-tests
cd build-tests && ctest

Enable coverage (optional):

cmake -S tests -B build-tests -DENABLE_COVERAGE=ON
cmake --build build-tests
cd build-tests && make coverage

🛠 OOP-like Design

The project follows an interface-instance-driver pattern:

    Interfaces define generic APIs (led_interface.h).

    Drivers implement hardware-specific functionality (led_gpio.c).

    Instances connect drivers to board-level configuration (led_instances.c).

This allows:

    Code reusability across different boards

    Easier unit testing (mocking drivers)

    Clear separation between hardware abstraction and application logic

📜 License

MIT License – feel free to use and modify.

