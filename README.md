# STM32 FreeRTOS Template with OOP-like Driver Structure

This repository provides a modern **STM32 FreeRTOS project template** that follows a modular and OOP-like design, inspired by Renesas and layered architecture best practices.

---

## ✨ Features

- **FreeRTOS** integration for STM32
- **OOP-like driver structure** using `interfaces`, `instances`, and `drivers`
- **Board abstraction** via `board_config.h` and board-specific directories
- **HAL (CubeMX) + FreeRTOS middleware** integrated as a CMake library
- **Unit testing** with GoogleTest (GTest) and optional coverage reports
- **Cross-platform build**: `CMake` + `Ninja` + `arm-none-eabi-gcc`
- **Debug ready**: OpenOCD + ST-Link (`cortex-debug` in VS Code)

---

## 📂 Project Structure

```
.
├── boards/                    # Board-specific code and CubeMX sources
│   └── nucleo_g070rb/
│       ├── cube/               # HAL + FreeRTOS from CubeMX
│       ├── st_mcu_g0.cfg       # OpenOCD configuration
│       └── board_config.h      # Pin definitions and board setup
│
├── drivers/                    # Low-level hardware drivers
│   └── led/
│       ├── led_gpio.c
│       └── led_gpio.h
│
├── interfaces/                 # Abstract APIs (header-only contracts)
│   └── led_interface.h
│
├── instances/                  # Bind drivers to board configuration
│   └── led_instances.c
│
├── src/                        # Application code
│   └── main_app.c
│
├── tests/                      # Unit tests with GoogleTest
│
├── CMakeLists.txt               # Root CMake configuration
├── rebuild.sh                   # Build helper script
├── flash.sh                     # Flash helper script
└── README.md
```

---

## 🚀 Building the Project

**Requirements:**

- `arm-none-eabi-gcc` toolchain
- `CMake` ≥ 3.20
- `Ninja` build system
- `OpenOCD` or `st-flash`

### Build
```bash
./rebuild.sh
```

### Flash
```bash
./flash.sh
```

### Debug in VS Code

1. Install the **Cortex-Debug** extension  
2. Open the project folder in VS Code  
3. Press **F5** to start debugging  

---

## 🧪 Running Unit Tests

**Build & run tests on host:**
```bash
cmake -S tests -B build-tests
cmake --build build-tests
cd build-tests && ctest
```

**Enable coverage (optional):**
```bash
cmake -S tests -B build-tests -DENABLE_COVERAGE=ON
cmake --build build-tests
cd build-tests && make coverage
```

---

## 🛠 OOP-like Design

The project follows an **Interface → Driver → Instance → Application** pattern:

```
[ Interface ]  -->  Generic API definition
[ Driver    ]  -->  Hardware-specific implementation
[ Instance  ]  -->  Binds driver to board config
[ App Logic ]  -->  Uses interfaces only
```

**Benefits:**
- Code reusability across boards
- Easier unit testing (mock drivers)
- Clear separation between hardware and logic

Example for LED:

1. **Interface**: `led_interface.h` – defines `LED_On()`, `LED_Off()`  
2. **Driver**: `led_gpio.c` – implements functions using HAL GPIO  
3. **Instance**: `led_instances.c` – connects driver to `LED1` pin from `board_config.h`  

---

## 📜 License

MIT License – feel free to use, modify, and distribute.