/**
 * @file hal.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-09-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "../../components/LovyanGFX/src/LovyanGFX.hpp"
#include "keyboard/keyboard.h"
#include "mic/Mic_Class.hpp"
#include "speaker/Speaker_Class.hpp"
#include "button/Button.h"
#include <iostream>
#include <string>

namespace HAL
{
    /**
    * @brief Hal base for DI
    *
    */
    class Hal 
    {
        protected:
            LGFX_Device* _display;
            LGFX_Sprite* _canvas;
            LGFX_Sprite* _canvas_system_bar;
            LGFX_Sprite* _canvas_keyboard_bar;

            KEYBOARD::Keyboard* _keyboard;
            m5::Mic_Class* _mic;
            m5::Speaker_Class* _speaker;
            Button* _homeButton;

            bool _sntp_adjusted;

        public:
            Hal() : 
            _display(nullptr), 
            _canvas(nullptr), 
            _canvas_system_bar(nullptr),
            _canvas_keyboard_bar(nullptr),
            _keyboard(nullptr),
            _mic(nullptr),
            _speaker(nullptr),
            _homeButton(nullptr),
            _sntp_adjusted(false)
            {}

            // Getter 
            inline LGFX_Device* display() { return _display; }
            inline LGFX_Sprite* canvas() { return _canvas; }
            inline LGFX_Sprite* canvas_system_bar() { return _canvas_system_bar; }
            inline LGFX_Sprite* canvas_keyboard_bar() { return _canvas_keyboard_bar; }
            inline KEYBOARD::Keyboard* keyboard() { return _keyboard; }
            inline m5::Mic_Class* mic() { return _mic; }
            inline m5::Speaker_Class* Speaker() { return _speaker; }
            inline Button* homeButton() { return _homeButton; }

            inline void setSntpAdjusted(bool isAdjusted) { _sntp_adjusted = isAdjusted; }
            inline bool isSntpAdjusted(void) { return _sntp_adjusted; }


            
            // Canvas 
            inline void canvas_system_bar_update() { _canvas_system_bar->pushSprite(_canvas_keyboard_bar->width(), 0); }
            inline void canvas_keyboard_bar_update() { _canvas_keyboard_bar->pushSprite(0, 0); }
            inline void canvas_update() { _canvas->pushSprite(_canvas_keyboard_bar->width(), _canvas_system_bar->height()); }

            // Override 
            virtual std::string type() { return "null"; }
            virtual void init() {}

            virtual void playLastSound() {}
            virtual void playNextSound() {}
            virtual void playKeyboardSound() {}

            virtual uint8_t getBatLevel() { return 100; }
    };
}
