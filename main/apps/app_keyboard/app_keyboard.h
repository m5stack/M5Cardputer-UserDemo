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
                struct Data_t
                {
                    HAL::Hal* hal = nullptr;
                };
                Data_t _data;

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
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
