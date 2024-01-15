/**
 * @file app_texteditor.cpp
 * @author Anderson Antunes
 * @brief 
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 * 
 */
#include "app_texteditor.h"
#include "spdlog/spdlog.h"
#include "../utils/theme/theme_define.h"

using namespace MOONCAKE::APPS;

#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)
#define _sdcard _data.hal->sdcard()

void AppTextEditor::onCreate() {
    spdlog::info("{} onCreate", getAppName());
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}

void AppTextEditor::onResume() {
    spdlog::info("{} onResume", getAppName());
    ANIM_APP_OPEN();
    _data.text_buffer = "";
    _canvas_clear();
    _canvas->setTextScroll(true);
    _canvas->setBaseColor(THEME_COLOR_BG);
    _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
    _canvas->setFont(FONT_REPL);
    _canvas->setTextSize(FONT_SIZE_REPL);
    _canvas->setCursor(0, 0);

    if (!_sdcard->mount(false)) {
        _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
        _canvas->print("SD card mount failed\nPress enter to try again\nctrl + q to quit\n");
        _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
    } else {
        spdlog::info("SD card mounted");
        char * filename = _sdcard->get_filepath("note.txt");
        spdlog::info("filename: {}", filename);
        
        if (!_sdcard->file_exists(filename)) {
            free(filename);
            spdlog::info("file not exists");
            _canvas->print(" 1 ");
            _canvas_update();
            return;
        }

        FILE *f = fopen(filename, "r");
        if (f != NULL) {
            int line_num = 1;
            char text[80];
            while (fgets(text, 80, f)) {
                if (line_num < 10) {
                    _canvas->print(" ");
                }
                _canvas->print(line_num);
                _canvas->print(" ");
                _canvas->print(text);
                line_num++;
            }
            if (_canvas->getCursorX() == 0) {
                if (line_num < 10) {
                    _canvas->print(" ");
                }
                _canvas->print(line_num);
                _canvas->print(" ");
            }
            _data.lines = line_num - 1;
        }
        free(filename);
    }
    _canvas_update();
}

void AppTextEditor::onRunning() {
    _update_input();
    _update_cursor();

    if (_data.hal->homeButton()->pressed()) {
        _data.hal->playNextSound();
        spdlog::info("quit text editor");

        if (_data.text_buffer.size()) {
            char * filename = _sdcard->get_filepath("note.txt");
            FILE *f = fopen(filename, "a");
            if (f == NULL) {
                _canvas_clear();
                _canvas->setCursor(0, 0);
                spdlog::error("Failed to open file for writing");
                _canvas->setBaseColor(THEME_COLOR_BG);
                _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
                _canvas->print("Failed to open file for writing\nctrl + q to quit\n");
                _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
                _data.waiting_user_input = true;
                _canvas_update();
                free(filename);
                return;
            }
            free(filename);
            fprintf(f, "%s\n", _data.text_buffer.c_str());
            fclose(f);
            spdlog::info("file written");
        }
        
        destroyApp();
    }
}

void AppTextEditor::_update_input() {
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
                if (!_sdcard->is_mounted()) {
                    onResume();
                }

                _canvas->print(" \n");
                if (_data.lines < 10) {
                    _canvas->print(" ");
                }
                _canvas->print(_data.lines);
                _canvas->print(" ");
                _data.lines++;

                if (_data.waiting_user_input) {
                    _data.waiting_user_input = false;
                    _data.last_key_num = _keyboard->keyList().size();
                    if (_data.text_buffer == "y" || _data.text_buffer == "Y") {
                        
                        char * filename = _sdcard->get_filepath("note.txt");
                        if (_sdcard->file_exists(filename)) {
                            if (remove(filename) != 0) {
                                _canvas_clear();
                                _canvas->setCursor(0, 0);
                                spdlog::error("Failed to delete file");
                                _canvas->setBaseColor(THEME_COLOR_BG);
                                _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
                                _canvas->print("Failed to delete file\n");
                                _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
                                _canvas_update();
                                free(filename);
                                return;
                            }
                            spdlog::info("file deleted");
                        }
                        free(filename);
                    } 
                    
                    onResume();
                    return;
                }

                char * filename = _sdcard->get_filepath("note.txt");
                FILE *f = fopen(filename, "a");
                if (f == NULL) {
                    _canvas_clear();
                    _canvas->setCursor(0, 0);
                    spdlog::error("Failed to open file for writing");
                    _canvas->setBaseColor(THEME_COLOR_BG);
                    _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
                    _canvas->print("Failed to open file for writing\n");
                    _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
                    _canvas_update();
                    free(filename);
                    return;
                }
                free(filename);
                fprintf(f, "%s\n", _data.text_buffer.c_str());
                fclose(f);

                // Reset buffer
                _data.text_buffer = "";
            }

            // If space 
            else if (_keyboard->keysState().space)
            {
                _canvas->print(' ');
                _data.text_buffer += ' ';
            }

            // If delete 
            else if (_keyboard->keysState().del)
            {
                if (_keyboard->keysState().ctrl)
                {
                    _data.waiting_user_input = false;
                    _data.text_buffer = "";
                    _data.last_key_num = _keyboard->keyList().size();
                    _canvas_clear();
                    _canvas->setCursor(0, 0);
                    _data.text_buffer = "";
                    _canvas->setBaseColor(THEME_COLOR_BG);
                    _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
                    _canvas->print("Delete file? [y/N]: ");
                    _data.waiting_user_input = true;
                    _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
                    _canvas_update();
                } 
                
                else if (_data.text_buffer.size())
                {
                    // Pop input buffer 
                    _data.text_buffer.pop_back();

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

            else if (_keyboard->keysState().ctrl)
            {
                _data.last_key_num = _keyboard->keyList().size();   
                if (_keyboard->isKeyPressing(16)) { // Q
                    spdlog::info("quit text editor");
                    destroyApp();
                } else if (_keyboard->isKeyPressing(34)) { // F
                    spdlog::info("format sd card");
                    /*
                    _canvas_clear();
                    _canvas->setCursor(0, 0);
                    _data.text_buffer = "";
                    _canvas->setBaseColor(THEME_COLOR_BG);
                    _canvas->setTextColor(TFT_RED, THEME_COLOR_BG);
                    _canvas->print("Fordmat SD card? [y/N]: ");
                    _data.waiting_user_input = true;
                    _canvas->setTextColor(TFT_WHITE, THEME_COLOR_BG);
                    _canvas_update();
                    */
                }
                return;
            }

            // Normal chars 
            else 
            {
                for (auto& i : _keyboard->keysState().values)
                {
                    // spdlog::info("{}", i);

                    _canvas->print(i);
                    _data.text_buffer += i;
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

void AppTextEditor::_update_cursor() {
    if ((millis() - _data.cursor_update_time_count) > _data.cursor_update_period)
    {
        // Fix cursor position
        if (_canvas->getCursorX() == 0 || _canvas->width() - _canvas->getCursorX() < FONT_REPL_WIDTH) {
            _canvas->print(" ");
            _canvas->setCursor(0, _canvas->getCursorY());
        }

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

void AppTextEditor::onDestroy() {
    _canvas->setTextScroll(false);
    _sdcard->eject();
}