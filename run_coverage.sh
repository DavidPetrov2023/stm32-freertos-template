#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build-tests"

# čistý rebuild (ať se jistě propíšou --coverage)
rm -rf "$BUILD_DIR"

cmake -S tests -B "$BUILD_DIR" -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD_DIR" -j

# spustí ctest a vygeneruje HTML report (cílový target z CMake)
cmake --build "$BUILD_DIR" --target coverage

echo
echo "[OK] Coverage hotovo."
echo "Otevři: $BUILD_DIR/coverage_html/index.html"
