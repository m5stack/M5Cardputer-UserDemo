/**
 * @file launcher.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-09-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "../../hal/hal.h"
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <mooncake.h>
#include <stdint.h>
#include <string>
#include "../utils/smooth_menu/src/smooth_menu.h"
#include "app/app.h"

namespace MOONCAKE
{
    namespace APPS
    {
        /**
        * @brief Launcher
        *
        */
        class Launcher : public APP_BASE
        {
        private:
            struct KeyboardBarState_t
            {
                bool caps_lock = false;
                bool fn = false;
                bool ctrl = false;
                bool opt = false;
                bool alt = false;

                void reset()
                {
                    caps_lock = false;
                    fn = false;
                    ctrl = false;
                    opt = false;
                    alt = false;
                }
            };

            struct SystemState_t 
            {
                // 1 ~ 5
                uint8_t wifi_state = 5;
                // 1 ~ 4
                uint8_t bat_state = 1;
                std::string time = "12:34";
            };

            struct Data_t
            {
                HAL::Hal* hal = nullptr;

                // View shit
                // Menu
                uint32_t menu_update_preiod = 10;
                uint32_t menu_update_count = 0;
                SMOOTH_MENU::Simple_Menu* menu = nullptr;
                SMOOTH_MENU::SimpleMenuCallback_t* menu_render_cb = nullptr;
                APP_BASE* _opened_app = nullptr;
                
                // System bar
                uint32_t system_bar_update_preiod = 1000;
                uint32_t system_bar_update_count = 0;
                SystemState_t system_state;

                // Keyboard bar
                uint32_t keyboard_bar_update_preiod = 50;
                uint32_t keyboard_bar_update_count = 0;
                KeyboardBarState_t keybaord_state;

                char string_buffer[100];
            };
            Data_t _data;

            void _boot_anim();

            void _start_menu();
            void _update_menu();

            void _start_system_bar();
            void _update_system_bar();
            void _start_keyboard_bar();
            void _update_keyboard_bar();
            
            // Platform port 
            void _port_wait_enter();
            bool _port_check_next_pressed();
            bool _port_check_last_pressed();
            bool _port_check_enter_pressed();
            void _port_update_keyboard_state();
            void _port_update_system_state();
        

        public:
            void onCreate() override;
            void onRunning() override;
            void onRunningBG() override;
        };

        class Launcher_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "Launcher"; }
            void *newApp() override { return new Launcher; }
            void deleteApp(void *app) override { delete (Launcher *)app; }
        };
    }
}
