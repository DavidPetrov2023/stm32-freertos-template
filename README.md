# STM32 FreeRTOS Template (API → Instances → Driver → BSP → App)

Modern STM32 project template with a **clean separation of concerns** and a focus on portability and testability.
The app never touches HAL directly — it talks to small, stable APIs that are wired to drivers in the *instances* layer.

---

## ✨ Main Features
- FreeRTOS on STM32 (CubeMX HAL integrated via CMake)
- **Strict layer separation:** app never uses HAL directly
- BSP exposes only `board_init()`
- **Driver / Instances / API pattern** — easy driver swapping and testing
- UART and LED examples following the same architecture
- Built with CMake + Ninja + arm-none-eabi-gcc
- Debug-ready for VS Code + Cortex-Debug (OpenOCD/ST-Link)

---

## 🧱 Architecture

```
App (tasks, business logic) ──▶ uses ▶  API (pure C, no HAL/RTOS)

API ──(implemented by)──▶ Instances (binds driver to board pins, owns RTOS buffers)

Instances ──▶ Driver (HAL-only I/O, ISR callbacks, rings)

Driver ──▶ BSP (board_init, clocks, MX_*Init) ──▶ HAL (CubeMX)
```

**Rules**
- `app/` includes **only** headers from `interfaces/` and `instances/`.
- HAL APIs are used **only** in drivers, BSP, or inside instances (where wiring happens).
- RTOS objects (StreamBuffer/MessageBuffer/Tasks) live **above** drivers (instances or app).

---

## 📁 Project Structure

```
.
├─ app/
│  ├─ include/
│  └─ src/
│     └─ main_app.c        # calls board_init(), starts app tasks
│
├─ boards/
│  └─ nucleo_g070rb/
│     ├─ cube/             # CubeMX export: HAL, startup, LD, FreeRTOS config
│     ├─ board.c           # board_init(): HAL_Init + clocks + MX_*Init
│     ├─ board_config.h    # public BSP header (no HAL types)
│     ├─ board_pins_stm32g0.h  # private pins/macros
│     ├─ st_mcu_g0.cfg     # OpenOCD config
│     └─ STM32G070.svd     # SVD for debugging
│
├─ drivers/
│  ├─ led/
│  │  ├─ led_gpio.c        # LED driver over HAL GPIO
│  │  └─ led_gpio.h
│  └─ uart/
│     ├─ uart_dma_stm32.c  # HAL-only UART (DMA/IT, rings, ISR callbacks)
│     └─ uart_dma_stm32.h
│
├─ interfaces/
│  ├─ led_api.h            # pure LED API (vtable-like)
│  └─ uart_api.h           # pure UART API (no HAL/RTOS)
│
├─ instances/
│  ├─ led_instances.c      # binds LED driver to board; exposes g_led0
│  ├─ led_instances.h
│  ├─ uart_instances.c     # implements uart_api over driver + StreamBuffer
│  └─ uart_instances.h
│
├─ tests/                  # unit/integration tests
├─ CMakeLists.txt          # root; adds subdirs in order
├─ rebuild.sh              # build (Debug)
├─ run_tests.sh            # run unit tests
└─ README.md
```

> Order in root `CMakeLists.txt` is important (BSP → drivers → instances → app).

---

## 💡 LED Example (reference pattern)
- API: `interfaces/led_api.h`
- Driver: `drivers/led/led_gpio.[ch]` (HAL GPIO only)
- Instance: `instances/led_instances.[ch]` (creates `g_led0` by binding to board pin)
- App: `main_app.c` creates a FreeRTOS task that blinks via `g_led0`

The app never includes HAL headers, only `led_api.h` + `led_instances.h`.

---

## 🔌 UART Example (mirrors LED pattern)

### Files
- **API:** `interfaces/uart_api.h` — pure, no HAL/RTOS
- **Instances:** `instances/uart_instances.[ch]`
  - Owns the RX `StreamBuffer` (RTOS)
  - Registers a driver RX callback and forwards bytes into the buffer
  - Implements `g_uart2` API: `.init()`, `.write()`, `.read()`, `.getc()`
- **Driver:** `drivers/uart/uart_dma_stm32.[ch]` — HAL-only
  - TX ring & non-blocking `uart_dma_write()`
  - `HAL_UARTEx_RxEventCallback()` just calls the user callback (no RTOS)
  - `HAL_UART_TxCpltCallback()` advances the TX ring and kicks the next chunk

### Using the UART API in the app
```c
#include "uart_api.h"

void app_init(void) {
    g_uart2.init();                             // creates buffers, binds driver
    (void)g_uart2.write((uint8_t*)"hello\r\n", 7);

    uint8_t ch;
    if (g_uart2.getc(&ch, 20)) {                // timeout in ms
        (void)g_uart2.write(&ch, 1);
    }
}
```

### Why this split?
- **Driver** is reusable & testable (no RTOS).  
- **Instances** keep RTOS details local (StreamBuffer, timeouts→ticks).  
- **App** talks only to a stable, HAL-free API — consistent with the LED pattern.

---

## 🛠️ Build

Requirements
- `arm-none-eabi-gcc`, `cmake >= 3.20`, `ninja`, `openocd`

Build (Debug)
```bash
./rebuild.sh
```
Artifacts (Debug)
```
build/app/app.elf
build/app.hex
build/app.bin
build/app.map
```

---

## ⚡ Flash

OpenOCD
```bash
openocd -f boards/nucleo_g070rb/st_mcu_g0.cfg -c "program build/app/app.elf verify reset exit"
```
Alternatively: `st-flash` or STM32CubeProgrammer.

---

## 🐞 Debug (VS Code)

Use **Cortex-Debug** extension. Example `launch.json`:
```json
{
  "executable": "${workspaceFolder}/build/app/app.elf",
  "svdFile": "${workspaceFolder}/boards/nucleo_g070rb/STM32G070.svd",
  "configFiles": ["${workspaceFolder}/boards/nucleo_g070rb/st_mcu_g0.cfg"],
  "runToEntryPoint": "main"
}
```
Tip for CMake Tools users:
```
"executable": "${command:cmake.launchTargetPath}"
```
and select `app.elf` as the launch target.

---

## 🧭 Porting to Another Board
1. Create `boards/<new_board>/`  
   - Export CubeMX (`cube/`), add `board.c` (`board_init()`), `board_config.h`, private `board_pins_*.h`
2. Point `BOARD` in root `CMakeLists.txt` to the new board folder
3. Adjust `instances/*` to use the new pins/handles

---

## 🧪 Code Style
- No single-letter identifiers (except `i/j` in tiny loops)
- Descriptive names (`ctrl`, `config`, `handle`)
- Comments where names are not self-explanatory
- HAL used only in BSP/drivers/instances — **never in app**

---

## 📄 License
MIT