# STM32 FreeRTOS Template (API → Driver → Instance → BSP → App)

Modern STM32 project template with clear separation of application logic from HAL.  
The goal is to have **clean layers** and easy portability to another board or driver.

---

## ✨ Main Features

- **FreeRTOS** on STM32 (CubeMX HAL + middleware integrated via CMake)
- **Strict layer separation**: app never uses HAL directly
- **BSP** (`board_bsp`) only exposes `board_init()`
- **Driver/instance pattern** – easy driver swapping and testing
- Built with **CMake + Ninja + arm-none-eabi-gcc**
- Debug ready for **VS Code + Cortex-Debug (OpenOCD/ST-Link)**

---

## 📂 Project Structure

```
.
├─ app/
│  ├─ include/
│  └─ src/
│      └─ main_app.c          # calls board_init(), creates tasks
│
├─ boards/
│  └─ nucleo_g070rb/
│     ├─ cube/                # CubeMX: HAL + FreeRTOS, startup, LD
│     ├─ board.c              # board_init(): HAL_Init + clock + MX_GPIO_Init
│     ├─ board_config.h       # public BSP header (no HAL types)
│     ├─ board_pins_stm32g0.h # private pins (HAL macros)
│     ├─ st_mcu_g0.cfg        # OpenOCD config
│     └─ STM32G070.svd        # SVD for debug
│
├─ drivers/
│  └─ led/
│     ├─ led_gpio.c           # driver over HAL GPIO
│     └─ led_gpio.h
│
├─ interfaces/
│  └─ led_api.h               # pure API (vtable style)
│
├─ instances/
│  ├─ led_instances.c         # binds driver with board (g_led0)
│  └─ led_instances.h
│
├─ CMakeLists.txt             # root; adds subdirs in proper order
├─ rebuild.sh                 # build (Debug)
└─ README.md
```

Rule: `app/` includes only `interfaces/` and instances (`led_instances.h`).  
**Never** directly HAL or board_pins.

---

## 🧱 CMake Targets

- `cube_fw` – CubeMX export (HAL, FreeRTOS, startup, LD)
- `board_bsp` – `board_init()`, privately linked to `cube_fw`
- `led_gpio` – LED driver, private link to `cube_fw`
- `instances` – connect drivers with board
- `app.elf` – main app; links `board_bsp`, `instances`, `cube_fw`

Root `CMakeLists.txt` adds subdirs in order:

```
boards/${BOARD} → drivers/led → instances → app
```

---

## 🚀 Build

Requirements: `arm-none-eabi-gcc`, `cmake >= 3.20`, `ninja`, `openocd`.

```bash
./rebuild.sh
```

Artifacts (Debug):
- `build/app/app.elf`
- `build/app.hex`
- `build/app.bin`
- `build/app.map`

---

## 🔌 Flash

With OpenOCD:

```bash
openocd -f boards/nucleo_g070rb/st_mcu_g0.cfg -c "program build/app/app.elf verify reset exit"
```

Or use `st-flash` / `STM32CubeProgrammer`.

---

## 🐞 Debug (VS Code)

Use **Cortex-Debug** extension. Example `launch.json`:

```json
"executable": "${workspaceFolder}/build/app/app.elf",
"svdFile": "${workspaceFolder}/boards/nucleo_g070rb/STM32G070.svd",
"configFiles": ["${workspaceFolder}/boards/nucleo_g070rb/st_mcu_g0.cfg"],
"runToEntryPoint": "main"
```

Tip:  
```json
"executable": "${command:cmake.launchTargetPath}"
```
and select `app.elf` as CMake launch target.

---

## 💡 LED Example

- **API**: `interfaces/led_api.h`
- **Driver**: `drivers/led/led_gpio.[ch]` (HAL GPIO)
- **Instance**: `instances/led_instances.[ch]` (`g_led0`)
- **App**: `main_app.c` → FreeRTOS task blinking LED

App never sees HAL, only API.

---

## 🧩 Porting to Another Board

1. Create `boards/<new_board>/`:
   - export CubeMX (`cube/`)
   - `board.c` (`board_init()`)
   - `board_config.h` (public, no HAL)
   - `board_pins_*.h` (private)
   - CMakeLists for `cube_fw_<board>` and `board_bsp`
2. Set `BOARD` in root CMakeLists.txt.
3. Adjust `instances`.

---

## 🧼 Code Style

- No single-letter variables (except i/j in loops)
- Descriptive names (`ctrl`, `config`, `handle`)
- Comments only where names are not self-explanatory
- HAL dependencies only in BSP/driver/instances, never in app

---

## 📄 License

MIT