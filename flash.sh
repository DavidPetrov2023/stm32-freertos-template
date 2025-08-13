#!/usr/bin/env bash
set -euo pipefail

ELF="build/app.elf"
BIN="build/app.bin"
OCDCFG="boards/nucleo_g070rb/st_mcu_g0.cfg"

# 1) Build, pokud není ELF
if [[ ! -f "$ELF" ]]; then
  echo "[flash] ELF nenalezen → spouštím ./rebuild.sh"
  ./rebuild.sh
fi

# 2) Zkusit st-flash, jinak OpenOCD
if command -v st-flash >/dev/null 2>&1; then
  # zajisti .bin
  if [[ ! -f "$BIN" ]]; then
    echo "[flash] BIN nenalezen → vytvářím z ELF"
    arm-none-eabi-objcopy -O binary "$ELF" "$BIN"
  fi
  echo "[flash] st-flash"
  st-flash --reset write "$BIN" 0x08000000
else
  echo "[flash] openocd"
  openocd -f "$OCDCFG" -c "program $ELF verify reset exit"
fi

echo "[done] hotovo ✨"
