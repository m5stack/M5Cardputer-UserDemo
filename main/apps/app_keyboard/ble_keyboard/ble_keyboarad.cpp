/**
 * @file ble_keyboarad.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "../app_keyboard.h"
#include "spdlog/spdlog.h"
#include "../../utils/theme/theme_define.h"
#include "../../utils/ble_keyboard_wrap/ble_keyboard_wrap.h"


using namespace MOONCAKE::APPS;


void AppKeyboard::_ble_kb_init()
{
    ble_keyboard_wrap_init(_data.hal->keyboard());
}


void AppKeyboard::_ble_kb_update_infos()
{
    if (millis() - _data.update_infos_time_count > 500)
    {
        // spdlog::info("remain: {}", uxTaskGetStackHighWaterMark(NULL));

        _data.hal->canvas()->fillScreen(THEME_COLOR_BG);

        _data.hal->canvas()->setCursor(0, 0);
        _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
        _data.hal->canvas()->printf("[BLE Keyboard]\n");
        _data.hal->canvas()->printf("> Name: %s\n", ble_keyboard_wrap_get_device_name());

        // State 
        _data.hal->canvas()->printf("> State: ");
        _data.hal->canvas()->setTextColor(
            ble_keyboard_wrap_get_current_state() == ble_kb_wrap_state_connected ? TFT_GREENYELLOW : TFT_ORANGE, 
            THEME_COLOR_BG
        );
        _data.hal->canvas()->printf(
            "%s\n", 
            ble_keyboard_wrap_get_current_state() == ble_kb_wrap_state_connected ? "Connected" : "Wait connect.."
        );

        _data.hal->canvas_update();
        _data.update_infos_time_count = millis();
    }
}


static uint8_t _input_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void AppKeyboard::_ble_kb_update_kb_input()
{
    // spdlog::info("remain: {}", uxTaskGetStackHighWaterMark(NULL));

    if (ble_keyboard_wrap_get_current_state() != ble_kb_wrap_state_connected)
        return;

    if (millis() - _data.update_kb_time_count > 10)
    {
        if (_data.hal->keyboard()->isChanged()) 
        {
            uint8_t modifier = 0;
            if (_data.hal->keyboard()->isPressed()) 
            {
                memset(_input_buffer, 0, 8);
                auto status = _data.hal->keyboard()->keysState();

                int count = 0;
                for (auto& i : status.hidKey) 
                {
                    if (count < 6) 
                    {
                        _input_buffer[2 + count] = i;
                        count++;
                    }
                }

                if (status.ctrl) 
                {
                    // ESP_LOGI(TAG, "CONTROL");
                    modifier |= 0x01;
                }

                if (status.shift || _data.hal->keyboard()->capslocked()) {
                    // ESP_LOGI(TAG, "SHIFT");
                    modifier |= 0x02;
                }

                if (status.alt) 
                {
                    // ESP_LOGI(TAG, "ALT");
                    modifier |= 0x03;
                }

                _input_buffer[0] = modifier;

                // ESP_LOG_BUFFER_HEX(TAG, buffer, 8);
                ble_keyboard_wrap_update_input(_input_buffer);
            } 
            else 
            {
                memset(_input_buffer, 0, 8);
                ble_keyboard_wrap_update_input(_input_buffer);
            }
        }

        _data.update_kb_time_count = millis();
    }
}
