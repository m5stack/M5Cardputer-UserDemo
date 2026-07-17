# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

This is an **ESP-IDF v4.4.6** project (not Arduino/PlatformIO), targeting the ESP32-S3 (M5Stack Cardputer). All commands require the ESP-IDF environment to be sourced first.

```bash
# Source the toolchain (adjust path to your local esp-idf install)
. $HOME/esp/esp-idf-v4.4.6/export.sh

# Build
idf.py build

# Flash + open serial monitor (repo helper script; assumes /dev/ttyACM0, WSL usbipd passthrough setup)
./flash.sh
# equivalent manually:
idf.py -p /dev/ttyACM0 flash -b 1500000 monitor

# Open project configuration menu (Kconfig)
idf.py menuconfig

# Clean build artifacts
idf.py fullclean
```

There is no test suite, linter, or CI configured in this repo. `.clang-format` (LLVM-based, 4-space indent, Allman braces) defines the C++ style — run `clang-format` on touched files if available.

`hotplugSetup.sh` is a one-time WSL/udev setup script for USB device passthrough, unrelated to the build itself.

## Architecture

### Layers
- **`main/cardputer.cpp`** — entry point (`app_main()`, ESP-IDF's equivalent of `main()`). Instantiates the global `HalCardputer hal` and `Mooncake mooncake`, calls `hal.init()`, registers `hal` into mooncake's DI container via `setDatabaseSetupCallback`, calls `mooncake.installApp(...)` once per demo app, installs the launcher, then runs `while (1) mooncake.update();` forever. `#ifdef ON_HAL_TEST` / `ON_APP_TEST` / `ON_APP_TEST_WITH_LAUNCHER` blocks at the top of `app_main()` are dev-only shortcuts for testing a single app or the raw HAL in isolation — leave them commented out for normal builds.
- **`main/hal/`** — hardware abstraction. `hal.h` declares the abstract `HAL::Hal` base (display/canvas pointers, keyboard, mic, speaker, home button, sdcard, battery). `hal_cardputer.{h,cpp}` is the concrete `HalCardputer : public Hal` implementation with per-peripheral `_display_init/_keyboard_init/_mic_init/_speaker_init/_button_init/_bat_init/_sdcard_init`. Apps never touch peripherals directly — they get the shared `Hal*` through mooncake's DI database (see below).
- **`components/mooncake/`** (vendored) — a small Android-activity-style app framework. `app/app.h` defines `APP_BASE` (runtime instance, lifecycle hooks `onCreate/onResume/onRunning/onRunningBG/onPause/onDestroy`) and `APP_PACKER_BASE` (static metadata + factory: `getAppName()`, `getAppIcon()`, `newApp()`, `deleteApp()`). `app/app_manager.cpp` drives the FSM each tick. A `SimpleKV` key/value store is used purely for dependency injection — `cardputer.cpp` registers the HAL under the key `"HAL"`, and every app fetches it in `onCreate()` via `mcAppGetDatabase()->Get("HAL")->value<HAL::Hal*>()`.
- **`components/M5GFX/`** (vendored, LovyanGFX-derived) — the graphics library. `HalCardputer` owns the real `M5GFX` display plus three `LGFX_Sprite` double-buffer canvases (main content, keyboard bar, system bar). Apps draw into `hal->canvas()` and push with `hal->canvas_update()` / `canvas_system_bar_update()` / `canvas_keyboard_bar_update()` — never touch the display object directly.
- **`main/apps/`** — one folder per demo app (`app_hello`, `app_timer`, `app_chat`, `app_ir`, `app_keyboard`, `app_record`, `app_repl`, `app_set_wifi`, `app_texteditor`, `app_wifi_scan`), plus the special `launcher/` app and shared code in `apps/utils/` (theme colors/fonts, icon struct, open/close animation macros, smooth_menu UI toolkit, wifi/BLE/USB/IR wrappers).

### The launcher menu is automatic
`launcher/views/menu/menu.cpp` builds its menu by iterating `mcAppGetFramework()->getAppRegister().getInstalledAppList()` at runtime and reading each app's `getAppName()`/`getAppIcon()`. **Installing an app in `cardputer.cpp` is the only registration step** — there is no separate menu-wiring file to edit.

### Adding a new app (repeated pattern across every existing app)
1. New folder `main/apps/app_<name>/` with `app_<name>.h`, `app_<name>.cpp`, and `assets/<name>_big.h` + `assets/<name>_small.h` (icon bitmap arrays, see any existing app for the format).
2. In the header, under `namespace MOONCAKE { namespace APPS { ... } }`:
   - `class App<Name> : public APP_BASE` — private `Data_t` struct holding at minimum `HAL::Hal* hal`; override the lifecycle hooks you need.
   - `class App<Name>_Packer : public APP_PACKER_BASE` — override `getAppName()` (string shown in launcher), `getAppIcon()`, `newApp()`/`deleteApp()`.
3. In `onCreate()`, fetch the HAL via `mcAppGetDatabase()->Get("HAL")->value<HAL::Hal*>()`. Use `apps/utils/theme/theme_define.h` for colors/fonts and `apps/utils/icon/icon_define.h`'s `AppIcon_t` for icons.
4. Add `#include "app_<name>/app_<name>.h"` to `main/apps/apps.h`.
5. Add `mooncake.installApp(new APPS::App<Name>_Packer);` next to the other `installApp` calls in `main/cardputer.cpp`'s `app_main()`. That's the entire integration — the launcher picks it up automatically.

`main/CMakeLists.txt` glob-includes every `.c`/`.cpp` under `apps/` and `hal/`, so no CMake changes are needed when adding a new app folder.
