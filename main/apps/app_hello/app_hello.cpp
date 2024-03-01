/**
 * @file app_hello.cpp
 * @author Logimancer
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 */

#include "app_hello.h"
#include "spdlog/spdlog.h"
#include "../utils/theme/theme_define.h"

using namespace MOONCAKE::APPS;

#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)

void AppHello::_display_helloworld()
{
    _canvas->print("Hello, Cardputer!");
    _canvas_update();
}

void AppHello::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}

void AppHello::onResume()
{
    spdlog::info("{} onResume", getAppName());

    ANIM_APP_OPEN();

    _canvas_clear();
    _canvas->setTextScroll(true);
    _canvas->setBaseColor(THEME_COLOR_BG);
    _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
    _canvas->setFont(FONT_REPL);
    _canvas->setTextSize(FONT_SIZE_REPL);

    // Avoid input panel 
    _canvas->setCursor(0, 0);
    
    //run function once
    _display_helloworld();
}

void AppHello::onRunning()
{
    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit hello");
        destroyApp();
    }
}

void AppHello::onDestroy() {
    _canvas->setTextScroll(false);
}