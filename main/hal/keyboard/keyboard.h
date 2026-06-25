/**
 * @file keyboard.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <iostream>
#include <vector>
#include "keymap.h"


namespace KEYBOARD 
{
    struct Chart_t 
    {
        uint8_t value;
        uint8_t x_1;
        uint8_t x_2;
    };

    struct Point2D_t 
    {
        int x;
        int y;
    };

    const std::vector<int> output_list = {8, 9, 11};
    const std::vector<int> input_list = {13, 15, 3, 4, 5, 6, 7};
    // const std::vector<int> input_list = {1, 2, 3, 4, 5, 6, 7};

    const Chart_t X_map_chart[7] = 
    {
        {1, 0, 1},
        {2, 2, 3},
        {4, 4, 5},
        {8, 6, 7},
        {16, 8, 9},
        {32, 10, 11},
        {64, 12, 13}
    };

    struct KeyValue_t 
    {
        const char* value_first;
        const int value_num_first;
        const char* value_second;
        const int value_num_second;
        const int value_num_third;
    };

    const KeyValue_t _key_value_map[4][14] = {
    {{"`", KEY_GRAVE, "~", KEY_GRAVE, KEY_ESC},
     {"1", KEY_1, "!", KEY_1, KEY_F1},
     {"2", KEY_2, "@", KEY_2, KEY_F2},
     {"3", KEY_3, "#", KEY_3, KEY_F3},
     {"4", KEY_4, "$", KEY_4, KEY_F4},
     {"5", KEY_5, "%", KEY_5, KEY_F5},
     {"6", KEY_6, "^", KEY_6, KEY_F6},
     {"7", KEY_7, "&", KEY_7, KEY_F7},
     {"8", KEY_8, "*", KEY_8, KEY_F8},
     {"9", KEY_9, "(", KEY_9, KEY_F9},
     {"0", KEY_0, ")", KEY_0, KEY_F10},
     {"-", KEY_MINUS, "_", KEY_MINUS, KEY_F11},
     {"=", KEY_EQUAL, "+", KEY_EQUAL, KEY_F12},
     {"del", KEY_BACKSPACE, "del", KEY_BACKSPACE, KEY_DELETE}},
    {{"tab", KEY_TAB, "tab", KEY_TAB, 0},
     {"q", KEY_Q, "Q", KEY_Q, 0},
     {"w", KEY_W, "W", KEY_W, 0},
     {"e", KEY_E, "E", KEY_E, 0},
     {"r", KEY_R, "R", KEY_R, 0},
     {"t", KEY_T, "T", KEY_T, 0},
     {"y", KEY_Y, "Y", KEY_Y, 0},
     {"u", KEY_U, "U", KEY_U, 0},
     {"i", KEY_I, "I", KEY_I, 0},
     {"o", KEY_O, "O", KEY_O, 0},
     {"p", KEY_P, "P", KEY_P, 0},
     {"[", KEY_LEFTBRACE, "{", KEY_LEFTBRACE, KEY_HOME},
     {"]", KEY_RIGHTBRACE, "}", KEY_RIGHTBRACE, KEY_END},
     {"\\", KEY_BACKSLASH, "|", KEY_BACKSLASH, KEY_INSERT}},
    {{"fn", 0, "fn", 0, 0},
     {"shift", 0, "shift", 0, 0},
     {"a", KEY_A, "A", KEY_A, 0},
     {"s", KEY_S, "S", KEY_S, 0},
     {"d", KEY_D, "D", KEY_D, 0},
     {"f", KEY_F, "F", KEY_F, 0},
     {"g", KEY_G, "G", KEY_G, 0},
     {"h", KEY_H, "H", KEY_H, 0},
     {"j", KEY_J, "J", KEY_J, 0},
     {"k", KEY_K, "K", KEY_K, 0},
     {"l", KEY_L, "L", KEY_L, 0},
     {";", KEY_SEMICOLON, ":", KEY_SEMICOLON, KEY_UP},
     {"'", KEY_APOSTROPHE, "\"", KEY_APOSTROPHE, 0},
     {"enter", KEY_ENTER, "enter", KEY_ENTER, KEY_PAGEUP}},
    {{"ctrl", KEY_LEFTCTRL, "ctrl", KEY_LEFTCTRL, 0},
     {"opt", 0, "opt", 0, 0},
     {"alt", KEY_LEFTALT, "alt", KEY_LEFTALT, 0},
     {"z", KEY_Z, "Z", KEY_Z, 0},
     {"x", KEY_X, "X", KEY_X, 0},
     {"c", KEY_C, "C", KEY_C, 0},
     {"v", KEY_V, "V", KEY_V, 0},
     {"b", KEY_B, "B", KEY_B, KEY_PAUSE},
     {"n", KEY_N, "N", KEY_N, 0},
     {"m", KEY_M, "M", KEY_M, 0},
     {",", KEY_COMMA, "<", KEY_COMMA, KEY_LEFT},
     {".", KEY_DOT, ">", KEY_DOT, KEY_DOWN},
     {"/", KEY_SLASH, "?", KEY_SLASH, KEY_RIGHT},
     {"space", KEY_SPACE, "space", KEY_SPACE, KEY_PAGEDOWN}}};

    class Keyboard
    {
        public:
            
            struct KeysState
            {
                bool tab = false;
                bool fn = false;
                bool shift = false;
                bool ctrl = false;
                bool opt = false;
                bool alt = false;
                bool del = false;
                bool enter = false;
                bool space  = false;
                
                std::vector<char> values;
                std::vector<int> hidKey;

                void reset()
                {
                    tab = false;
                    fn = false;
                    shift = false;
                    ctrl = false;
                    opt = false;
                    alt = false;
                    del = false;
                    enter = false;
                    space = false;

                    values.clear();
                    hidKey.clear();
                }
            };


        private:
            std::vector<Point2D_t> _key_list_buffer;

            std::vector<Point2D_t> _key_values_without_special_keys;
            KeysState _keys_state_buffer;

            bool _is_caps_locked;
            uint8_t _last_key_size;


            void _set_output(const std::vector<int>& pinList, uint8_t output);
            uint8_t _get_input(const std::vector<int>& pinList);


        public:
            Keyboard() : 
                _is_caps_locked(false),
                _last_key_size(0)
                {}

            void init();

            Point2D_t getKey();

            uint8_t getKeyNum(Point2D_t keyCoor);

            void updateKeyList();
            inline std::vector<KEYBOARD::Point2D_t>& keyList() { return _key_list_buffer; }

            inline KeyValue_t getKeyValue(const Point2D_t& keyCoor) { return _key_value_map[keyCoor.y][keyCoor.x]; }

            bool isKeyPressing(int keyNum);

            void updateKeysState();
            inline KeysState& keysState() { return _keys_state_buffer; }

            inline bool capslocked(void) { return _is_caps_locked; }
            inline void setCapsLocked(bool isLocked) { _is_caps_locked = isLocked; }

            bool isChanged();
            inline uint8_t isPressed() { return _key_list_buffer.size(); }
    };
}
