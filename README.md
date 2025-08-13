# STM32 FreeRTOS Template with Unit Tests and Coverage

This project is a **STM32 FreeRTOS template** designed for embedded development.  
It now includes:
- **Google Test** integration for unit testing
- **Code coverage** support with LCOV + HTML reports
- **CMake-based** build system for portability
- Separate build directories for firmware and tests

## Features
- **FreeRTOS** support for STM32
- **Separation of application and board layers**
- **Unit tests** in the `tests/` folder
- **Coverage reports** generated to `build-tests/coverage_html/index.html`
- Easy setup with `build.sh` and `run_tests.sh` scripts

## Folder Structure
```
stm32-freertos-template/
├── app/              # Application code
├── board/            # Board-specific code
├── freertos/         # FreeRTOS kernel
├── tests/            # Unit tests
├── build/            # Firmware build output
├── build-tests/      # Unit test build output
├── build.sh          # Build firmware script
├── run_tests.sh      # Build & run tests + coverage script
└── CMakeLists.txt    # Main CMake configuration
```

## Running Tests
```bash
./run_tests.sh
```
This will:
1. Configure the `build-tests/` directory with GoogleTest
2. Build the `unit_tests` binary
3. Run all tests
4. Generate a **code coverage HTML report**

Open the coverage report:
```bash
xdg-open build-tests/coverage_html/index.html
```

## Building Firmware
```bash
./build.sh
```

## Requirements
- **CMake >= 3.20**
- **gcc-arm-none-eabi** for firmware
- **g++ / gcc** for unit tests
- **lcov** and **genhtml** for coverage

## Notes
- Make sure to clean build directories when switching between Ninja and Unix Makefiles:
```bash
rm -rf build/ build-tests/
```