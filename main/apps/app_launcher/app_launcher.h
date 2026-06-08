/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <hal.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <mooncake.h>
#include <stdint.h>
#include <string>

/**
 * @brief
 *
 */
class Launcher : public mooncake::AppAbility {
private:
    struct KeyboardBarState_t {
        bool shift = false;
        bool fn    = false;
        bool ctrl  = false;
        bool opt   = false;
        bool alt   = false;

        void reset()
        {
            shift = false;
            fn    = false;
            ctrl  = false;
            opt   = false;
            alt   = false;
        }
    };

    struct SystemState_t {
        // 1 ~ 5
        uint8_t wifi_state = 5;
        // 1 ~ 4
        uint8_t bat_state     = 1;
        std::string bat_level = "100%";
        std::string time      = "12:34";
    };

    struct Data_t {
        // System bar
        uint32_t system_bar_update_period = 1000;
        uint32_t system_bar_update_count  = 0;
        uint32_t bat_update_time_count    = 0;
        SystemState_t system_state;

        // Keyboard bar
        KeyboardBarState_t keyboard_state;

        char string_buffer[100];

        int running_app_id = -1;
    };
    Data_t _data;

    void boot_anim();

    void start_menu();
    void update_menu(bool pushCanvas = true);

    void start_system_bar();
    void update_system_bar();
    void start_keyboard_bar();
    void render_keyboard_bar();
    void render_system_bar();
    void handle_app_open(int index, int appId);

public:
    void onCreate() override;
    void onRunning() override;
};
