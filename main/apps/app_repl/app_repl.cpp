/**
 * @file app_repl.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.6
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_repl.h"
#include "spdlog/spdlog.h"
#include "../utils/theme/theme_define.h"


using namespace MOONCAKE::APPS;


#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)


/// -----------------------------
/// Pika python output redirect 
static HAL::Hal* _hal = nullptr;
static PikaObj* _pika_main = nullptr;

int pika_platform_putchar(char ch)
{
    _hal->canvas()->print(ch);
    return putchar(ch);
}

void pika_platform_printf(char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    // pika_vprintf(fmt, args);
    _hal->canvas()->vprintf(fmt, args);
    va_end(args);
}

void pika_platform_clear(void)
{
    _hal->canvas()->fillScreen(THEME_COLOR_BG);
    _hal->canvas()->setCursor(0, 0);
    _hal->canvas_update();
}
/// -----------------------------


void AppREPL::_update_input()
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
                if (_data.repl_input_buffer == "exit()")
                {
                    destroyApp();
                    return;
                }

                // New line (little space at front is to cover the cursor)
                _canvas->print(" \n");

                // Run cmd 
                spdlog::info("run cmd: \"{}\"", _data.repl_input_buffer);
                obj_run(_pika_main, (char*)_data.repl_input_buffer.c_str());

                _canvas->print(">>> ");

                // Reset buffer
                _data.repl_input_buffer = "";
            }

            // If space 
            else if (_keyboard->keysState().space)
            {
                _canvas->print(' ');
                _data.repl_input_buffer += ' ';
            }

            // If delete 
            else if (_keyboard->keysState().del)
            {
                if (_data.repl_input_buffer.size())
                {
                    // Pop input buffer 
                    _data.repl_input_buffer.pop_back();

                    // Pop canvas display 
                    int cursor_x = _canvas->getCursorX();
                    int cursor_y = _canvas->getCursorY();

                    if (cursor_x - FONT_REPL_WIDTH < 0)
                    {
                        // Last line 
                        cursor_y -= FONT_REPL_HEIGHT;
                        cursor_x = _canvas->width() - FONT_REPL_WIDTH;
                    }
                    else 
                    {
                        cursor_x -= FONT_REPL_WIDTH;
                    }

                    spdlog::info("new cursor {} {}", cursor_x, cursor_y);

                    _canvas->setCursor(cursor_x, cursor_y);
                    _canvas->print("  ");
                    _canvas->setCursor(cursor_x, cursor_y);
                }
            }

            // Normal chars 
            else 
            {
                for (auto& i : _keyboard->keysState().values)
                {
                    // spdlog::info("{}", i);

                    _canvas->print(i);
                    _data.repl_input_buffer += i;
                }
            }

            _canvas_update();

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


void AppREPL::_update_cursor()
{
    if ((millis() - _data.cursor_update_time_count) > _data.cursor_update_period)
    {
        // Get cursor 
        int cursor_x = _canvas->getCursorX();
        int cursor_y = _canvas->getCursorY();

        _canvas->print(_data.cursor_state ? '_' : ' ');
        _canvas->setCursor(cursor_x, cursor_y);
        _canvas_update();

        _data.cursor_state = !_data.cursor_state;
        _data.cursor_update_time_count = millis();
    }
}


void AppREPL::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


void AppREPL::onResume()
{
    spdlog::info("{} onResume", getAppName());

    ANIM_APP_OPEN();

    _canvas_clear();
    _canvas->setTextScroll(true);
    _canvas->setBaseColor(THEME_COLOR_BG);
    _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
    _canvas->setFont(FONT_REPL);
    _canvas->setTextSize(FONT_SIZE_REPL);
    

    // Create pika python 
    if (_pika_main == nullptr)
    {
        spdlog::info("create pica python");

        // Expose pointer for pica's output redirect
        _hal = _data.hal;

        // Create pika 
        _pika_main = pikaPythonInit();
    }
    else 
    {
        spdlog::info("pica python already created");
    }
        

    _canvas_clear();
    _canvas->setCursor(0, 0);
    _canvas->printf(">>> ");
    _canvas_update();
}


void AppREPL::onRunning()
{
    _update_input();
    _update_cursor();

    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit repl");
        destroyApp();
    }
}


void AppREPL::onDestroy()
{
    _canvas->setTextScroll(false);
}

