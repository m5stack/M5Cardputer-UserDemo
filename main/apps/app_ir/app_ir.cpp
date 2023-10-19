/**
 * @file app_ir.cpp
 * @author Forairaaaaa
 * @brief http://gitlab.m5stack.com/Forairaaaaa/stamps3_keypad_factory_test/blob/master/code/src/factory_test/ir/ir_test.cpp
 * @version 0.6
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_ir.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"
#include "../utils/ir_wrap/ir_wrap.h"


using namespace MOONCAKE::APPS;


#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)


void AppIR::_update_input()
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
                // New line 
                _canvas->print(" \n");

                // // Reset buffer
                // _data.repl_input_buffer = "";

                _data.current_state = state_fire;
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


void AppIR::_update_cursor()
{
    if ((millis() - _data.cursor_update_time_count) > _data.cursor_update_period)
    {
        // Get cursor 
        int cursor_x = _canvas->getCursorX();
        int cursor_y = _canvas->getCursorY();

        // spdlog::info("cursor {} {}", cursor_x, cursor_y);

        _canvas->print(_data.cursor_state ? '_' : ' ');
        _canvas->setCursor(cursor_x, cursor_y);
        _canvas_update();

        _data.cursor_state = !_data.cursor_state;
        _data.cursor_update_time_count = millis();
    }
}


void AppIR::_update_state()
{
    if (_data.current_state == state_init)
    {
        _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
        _canvas->printf("Input NEC msg in:\naddr(dec),data(dec)\ne.g. 16,1\nHit Enter to send\n");
        _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
        _canvas->printf(">>> ");
        _canvas_update();
        
        _data.current_state = state_wait_input;
    }

    else if (_data.current_state == state_fire)
    {
        // ir_wrap_send(16, 1);
        spdlog::info("raw input: {}", _data.repl_input_buffer);

        //  Try parse input shit 
        bool is_parse_ok = false;
        int ir_addr = 0;
        int ir_data = 0;

        _customSplit(_data.repl_input_buffer, ',', _data.parse_result);

        spdlog::info("parse get {}:", _data.parse_result.size());
        for (const auto& i : _data.parse_result)
            spdlog::info("{}", i);
        
        if (_data.parse_result.size() >= 2)
        {
            // https://docs.espressif.com/projects/esp-idf/zh_CN/v4.4.6/esp32/api-guides/error-handling.html
            try
            {
                ir_addr = std::stoi(_data.parse_result[0]);
                ir_data = std::stoi(_data.parse_result[1]);
                spdlog::info("get addr: {} data: {}", ir_addr, ir_data);

                is_parse_ok = true;
            }
            catch(const std::exception& e)
            {
                // std::cerr << e.what() << '\n';
                spdlog::info("parse failed: {}", e.what());

                is_parse_ok = false;
            }
        }



        // Send the shit out 
        if (is_parse_ok)
        {
            ir_wrap_send((uint8_t)ir_addr, (uint8_t)ir_data);

            _canvas->setTextColor(TFT_GREEN, THEME_COLOR_BG);
            _canvas->printf("Msg Send:\naddr: 0x%02X data: 0x%02X\n", (uint8_t)ir_addr, (uint8_t)ir_data);
            _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
        }
        else 
        {
            _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
            _canvas->printf("Parse %s failed\n", _data.repl_input_buffer.c_str());
            _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
        }



        // // New line 
        // _canvas->print(">>> ");
        // _canvas_update();
        // // Reset buffer
        // _data.repl_input_buffer = "";


        // Keep that shit still 
        _canvas->printf(">>> %s", _data.repl_input_buffer.c_str());
        _canvas_update();

        _data.current_state = state_wait_input;
    }
}


// Cpp sucks 
// https://favtutor.com/blogs/split-string-cpp
void AppIR::_customSplit(const std::string& str, char separator, std::vector<std::string>& result)
{
    result.clear();

    int startIndex = 0, endIndex = 0;
    std::string temp;
    for (int i = 0; i <= str.size(); i++) 
    {
        // If we reached the end of the word or the end of the input.
        if (str[i] == separator || i == str.size()) 
        {
            endIndex = i;
            temp.clear();
            temp.append(str, startIndex, endIndex - startIndex);
            result.push_back(temp);
            startIndex = endIndex + 1;
        }
    }
}


void AppIR::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal*>();

    ir_wrap_init();
}


void AppIR::onResume()
{ 
    ANIM_APP_OPEN(); 

    _canvas_clear();
    _canvas->setTextScroll(true);
    _canvas->setBaseColor(THEME_COLOR_BG);
    _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
    _canvas->setFont(FONT_REPL);
    _canvas->setTextSize(FONT_SIZE_REPL);
    _canvas->setCursor(0, 0);
    
    _data.current_state = state_init;
}


void AppIR::onRunning()
{
    _update_state();
    _update_input();
    _update_cursor();

    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit ir");
        destroyApp();
    }
}


void AppIR::onDestroy()
{
    _canvas->setTextScroll(false);
}
