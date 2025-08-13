#!/usr/bin/env bash
set -euo pipefail

# ---------- config ----------
BOARD="${BOARD:-nucleo_g070rb}"
BUILD_DIR="${BUILD_DIR:-build}"
TC_FILE="${TC_FILE:-toolchain-gcc-arm-none-eabi.cmake}"
# prefer Ninja if available
if command -v ninja >/dev/null 2>&1; then
  GENERATOR="${GENERATOR:-Ninja}"
else
  GENERATOR="${GENERATOR:-Unix Makefiles}"
fi

# ---------- helpers ----------
configure() {
  cmake -S . -B "$BUILD_DIR" -G "$GENERATOR" \
        -DCMAKE_TOOLCHAIN_FILE="$TC_FILE" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DBOARD="$BOARD" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
}

needs_wipe=false
if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
  cur_gen=$(grep -E '^CMAKE_GENERATOR(:INTERNAL|:STRING)=' "${BUILD_DIR}/CMakeCache.txt" | head -n1 | cut -d= -f2- || true)
  [[ "${cur_gen:-}" != "$GENERATOR" ]] && needs_wipe=true

  # ensure correct compiler (ARM GCC)
  if ! grep -q "CMAKE_C_COMPILER:FILEPATH=.*arm-none-eabi-gcc" "${BUILD_DIR}/CMakeCache.txt"; then
    needs_wipe=true
  fi
else
  needs_wipe=true
fi

if $needs_wipe; then
  echo "[reconfigure] (re)creating ${BUILD_DIR} with ${GENERATOR}"
  rm -rf "${BUILD_DIR}"
fi

# ---------- configure & build ----------
configure
echo "[build] ${BUILD_DIR} (${GENERATOR}, Debug)"
cmake --build "$BUILD_DIR" -j

# ---------- quality of life ----------
# create/update compile_commands.json symlink for IDEs
if [[ -f "${BUILD_DIR}/compile_commands.json" ]]; then
  ln -sf "${BUILD_DIR}/compile_commands.json" compile_commands.json
fi

# print size if we have an ELF
if [[ -f "${BUILD_DIR}/app.elf" ]]; then
  echo
  echo "[size]"
  arm-none-eabi-size "${BUILD_DIR}/app.elf" || true
fi

echo
echo "[done] Build finished."
