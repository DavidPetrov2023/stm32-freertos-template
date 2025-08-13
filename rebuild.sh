#!/usr/bin/env bash
set -euo pipefail

BOARD="${BOARD:-nucleo_g070rb}"
GENERATOR="${GENERATOR:-Ninja}"
BUILD_DIR="${BUILD_DIR:-build}"
TC_FILE="${TC_FILE:-toolchain-gcc-arm-none-eabi.cmake}"

# funkce: (re)configure s toolchainem
configure() {
  cmake -S . -B "$BUILD_DIR" -G "$GENERATOR" \
        -DCMAKE_TOOLCHAIN_FILE="$TC_FILE" \
        -DBOARD="$BOARD"
}

# pokud cache není, nebo není správný kompilátor, znovu vytvoř build/
if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  rm -rf "$BUILD_DIR"
  configure
else
  # zkontroluj, že se používá arm-none-eabi-gcc
  if ! grep -q "CMAKE_C_COMPILER:FILEPATH=.*arm-none-eabi-gcc" "$BUILD_DIR/CMakeCache.txt"; then
    echo "[reconfigure] switching to arm-none-eabi-gcc"
    rm -rf "$BUILD_DIR"
    configure
  else
    # i tak pošli toolchain (nevadí), CMake už ví, že ho má
    configure
  fi
fi

echo "[build]"
cmake --build "$BUILD_DIR" -j
