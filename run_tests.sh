#!/usr/bin/env bash
set -euo pipefail

# Vyber generátor (Ninja preferováno)
if command -v ninja >/dev/null 2>&1; then
  GEN="Ninja"
else
  GEN="Unix Makefiles"
fi

BUILD_DIR="build-tests"

# Když už existuje cache s jiným generátorem, smaž ji
if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
  CUR_GEN=$(grep -E '^CMAKE_GENERATOR(:INTERNAL|:STRING)=' "${BUILD_DIR}/CMakeCache.txt" | head -n1 | cut -d= -f2- || true)
  if [[ "${CUR_GEN:-}" != "$GEN" ]]; then
    echo "[reconfigure] Generator changed (${CUR_GEN:-unknown} -> ${GEN}), wiping ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
  fi
fi

# Configure & build & run
cmake -S tests -B "${BUILD_DIR}" -G "$GEN" -DENABLE_COVERAGE=OFF
cmake --build "${BUILD_DIR}" -j
ctest --test-dir "${BUILD_DIR}" --output-on-failure
