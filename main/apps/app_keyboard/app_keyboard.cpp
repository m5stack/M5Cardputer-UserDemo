/**
 * @file app_keyboard.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_keyboard.h"
#include "spdlog/spdlog.h"
#include "../utils/wifi_common_test/wifi_common_test.h"
#include "../utils/theme/theme_define.h"


using namespace MOONCAKE::APPS;


// #define TEST_USE_BLE
// #define TEST_USE_USB


void AppKeyboard::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


void AppKeyboard::onResume()
{
    ANIM_APP_OPEN();

    {
        _data.hal->canvas()->fillScreen(THEME_COLOR_BG);
        _data.hal->canvas()->setFont(FONT_REPL);
        _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
        _data.hal->canvas()->setTextSize(1);
        _data.hal->canvas_update();

        // Start ble keyboard 
        spdlog::info("remain before: {}", uxTaskGetStackHighWaterMark(NULL));
    }

    _select_kb_type();

    // #ifdef TEST_USE_BLE
    // _ble_kb_init();
    // #endif
    // #ifdef TEST_USE_USB
    // _usb_kb_init();
    // #endif

    if (_data.kb_type == kb_type_ble)
        _ble_kb_init();
    else if (_data.kb_type == kb_type_usb)
        _usb_kb_init();
}


void AppKeyboard::onRunning()
{
    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit app {}", getAppName());
        destroyApp();
    }

    // #ifdef TEST_USE_BLE
    // _ble_kb_update_infos();
    // _ble_kb_update_kb_input();
    // #endif
    // #ifdef TEST_USE_USB
    // _usb_kb_update_infos();
    // _usb_kb_update_kb_input();
    // #endif

    if (_data.kb_type == kb_type_ble)
    {
        _ble_kb_update_infos();
        _ble_kb_update_kb_input();
    }
    else if (_data.kb_type == kb_type_usb)
    {
        _usb_kb_update_infos();
        _usb_kb_update_kb_input();
    }
}


void AppKeyboard::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    // ble_keyboard_wrap_deinit();
    
    delay(200);
    esp_restart();
    delay(10000);
}
