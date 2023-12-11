/**
 * @file app_keyboard.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <mooncake.h>
#include "../../hal/hal.h"
#include "../utils/theme/theme_define.h"
#include "../utils/anim/anim_define.h"
#include "../utils/icon/icon_define.h"

#include "assets/keyboard_big.h"
#include "assets/keyboard_small.h"


namespace MOONCAKE
{
    namespace APPS
    {
        class AppKeyboard : public APP_BASE
        {
            private:
                enum KeyboardType_t
                {
                    kb_type_ble = 0,
                    kb_type_usb,
                };

                struct Data_t
                {
                    HAL::Hal* hal = nullptr;
                    uint32_t update_infos_time_count = 0;
                    uint32_t update_kb_time_count = 0;
                    KeyboardType_t kb_type = kb_type_ble;
                };
                Data_t _data;
                void _select_kb_type();
                void _ble_kb_init();
                void _ble_kb_update_infos();
                void _ble_kb_update_kb_input();
                void _usb_kb_init();
                void _usb_kb_update_infos();
                void _usb_kb_update_kb_input();

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
        };

        class AppKeyboard_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "KEYBOARD"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_keyboard_big, image_data_keyboard_small)); }
            void* newApp() override { return new AppKeyboard; }
            void deleteApp(void *app) override { delete (AppKeyboard*)app; }
        };
    }
}
