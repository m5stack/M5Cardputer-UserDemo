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
#include "../utils/ble_keyboard_wrap/ble_keyboard_wrap.h"


using namespace MOONCAKE::APPS;


void AppKeyboard::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


void AppKeyboard::onResume()
{
    ANIM_APP_OPEN();


    _data.hal->canvas()->fillScreen(THEME_COLOR_BG);
    _data.hal->canvas()->setFont(FONT_REPL);
    _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    _data.hal->canvas()->setTextSize(1);
    _data.hal->canvas_update();


    // Start ble keyboard 
    ble_keyboard_wrap_init(_data.hal->keyboard());


    // _data.hal->canvas()->deleteSprite();
    // while (1)
    // {
    //     delay(1000);
    // }
}


void AppKeyboard::onRunning()
{
    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit app {}", getAppName());
        destroyApp();
    }
}
