/**
 * @file app_chat.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_chat.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"
#include "../utils/esp_now_wrap/esp_now_wrap.h"


using namespace MOONCAKE::APPS;


#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)


void AppChat::_update_input()
{
    // spdlog::info("{} {}", _keyboard->keyList().size(), _data.last_key_num);

    // If changed 
    if (_keyboard->keyList().size() != _data.last_key_num)
    {
        // If key pressed 
        if (_keyboard->keyList().size() != 0)
        {
            // Update states and values 
            _keyboard->updateKeysState();

            // If enter 
            if (_keyboard->keysState().enter)
            {
                // Send message 
                spdlog::info("send message: \"{}\"", _data.repl_input_buffer);
                espnow_wrap_send((uint8_t*)_data.repl_input_buffer.c_str(), _data.repl_input_buffer.size());

                // Draw on canvas 
                _data.repl_input_buffer += " << ";
                for (int i = 0; i < 114514; i++)
                {
                    spdlog::info("font width {}", FONT_REPL_WIDTH);

                    // If longger than a line 
                    if (_data.repl_input_buffer.length() * FONT_REPL_WIDTH > _canvas->width())
                    {
                        _canvas->printf("%s\n", _data.repl_input_buffer.substr(0, _canvas->width() / FONT_REPL_WIDTH).c_str());
                        _data.repl_input_buffer = _data.repl_input_buffer.substr(_canvas->width() / FONT_REPL_WIDTH);
                    }
                    else
                    {
                        _canvas->setCursor(_canvas->width() - _data.repl_input_buffer.length() * FONT_REPL_WIDTH, _canvas->getCursorY());
                        _canvas->printf("%s\n", _data.repl_input_buffer.c_str());
                        break;
                    }
                }

                // Reset buffer
                _data.repl_input_buffer = "";
            }

            // If space 
            else if (_keyboard->keysState().space)
            {
                // _canvas->print(' ');
                _data.repl_input_buffer += ' ';
            }

            // If delete 
            else if (_keyboard->keysState().del)
            {
                if (_data.repl_input_buffer.size())
                {
                    // Pop input buffer 
                    _data.repl_input_buffer.pop_back();
                }
            }

            // Normal chars 
            else 
            {
                for (auto& i : _keyboard->keysState().values)
                {
                    // spdlog::info("{}", i);

                    // _canvas->print(i);
                    _data.repl_input_buffer += i;
                }
            }

           _update_input_panel();

            // Update last key num 
            _data.last_key_num = _keyboard->keyList().size();
        }
        else 
        {
            // Reset last key num 
            _data.last_key_num = 0;
        }
    }
}


void AppChat::_update_cursor()
{
    if ((millis() - _data.cursor_update_time_count) > _data.cursor_update_period)
    {
        _data.cursor_state = !_data.cursor_state;
        _data.cursor_update_time_count = millis();
    }
}


void AppChat::_update_input_panel()
{
    _update_cursor();

    _canvas->fillRect(0, 0, _canvas->width(), _canvas->fontHeight() + 5, THEME_COLOR_BG);

    // _canvas->printf(">>> %s", _data.repl_input_buffer.c_str());
    snprintf(_data.string_buffer, sizeof(_data.string_buffer), "  >>> %s%c", _data.repl_input_buffer.c_str(), _data.cursor_state ? '_' : ' ');

    _canvas->drawString(_data.string_buffer, 0, 0);

    _canvas_update();
}


void AppChat::_update_receive()
{
    if (espnow_wrap_available())
    {
        _canvas->printf(" >> %s\n", (char*)espnow_wrap_get_received());
        _update_input_panel();
        espnow_wrap_clear();
    }
}


void AppChat::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


void AppChat::onResume()
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
    _canvas->print("\n\n");

    _update_input_panel();

    espnow_wrap_init();
}


void AppChat::onRunning()
{
    _update_input_panel();
    _update_input();
    _update_receive();

    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit chat");
        destroyApp();
    }
}

