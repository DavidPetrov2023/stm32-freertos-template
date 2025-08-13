#!/usr/bin/env bash
set -euo pipefail

# Build & run GTest s nativním kompilátorem (žádný toolchain!)
BUILD_DIR="build-tests"

cmake -S tests -B "$BUILD_DIR" -G Ninja
cmake --build "$BUILD_DIR" --config Debug

# Spustí testy a ukáže výstup, když něco selže
ctest --test-dir "$BUILD_DIR" --output-on-failure
