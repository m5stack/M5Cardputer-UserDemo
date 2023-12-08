/**
 * @file ble_keyboard_wrap.h
 * @author Forairaaaaa
 * @brief Ref: https://github.com/Gitshaoxiang/M5Cardputer-BLE-HID-Keyboard
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "../../../hal/keyboard/keyboard.h"


enum BleKbWrapState_t
{
    ble_kb_wrap_state_wait_connect = 0,
    ble_kb_wrap_state_connected,
};

// Stack overflow crush can not fix, menuconfig not working  
// https://github.com/espressif/esp-idf/issues/9720
void ble_keyboard_wrap_init(KEYBOARD::Keyboard* keyboard);
void ble_keyboard_wrap_deinit();
void ble_keyboard_wrap_update_input(uint8_t* input);
BleKbWrapState_t ble_keyboard_wrap_get_current_state();
const char* ble_keyboard_wrap_get_device_name();
