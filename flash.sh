#!/usr/bin/env bash
set -euo pipefail

ELF="build/app.elf"
BIN="build/app.bin"
OCDCFG="boards/nucleo_g070rb/st_mcu_g0.cfg"

# 1) Build if ELF does not exist
if [[ ! -f "$ELF" ]]; then
  echo "[flash] ELF not found → running ./rebuild.sh"
  ./rebuild.sh
fi

# 2) Try st-flash first, otherwise fall back to OpenOCD
if command -v st-flash >/dev/null 2>&1; then
  # Ensure the .bin file exists
  if [[ ! -f "$BIN" ]]; then
    echo "[flash] BIN not found → creating from ELF"
    arm-none-eabi-objcopy -O binary "$ELF" "$BIN"
  fi
  echo "[flash] Using st-flash"
  st-flash --reset write "$BIN" 0x08000000
else
  echo "[flash] Using OpenOCD"
  openocd -f "$OCDCFG" -c "program $ELF verify reset exit"
fi

echo "[done] Flashing complete ✨"
