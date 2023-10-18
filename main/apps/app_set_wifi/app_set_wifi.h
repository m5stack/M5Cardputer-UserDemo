/**
 * @file app_set_wifi.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-20
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

#include "assets/set_wifi_big.h"
#include "assets/set_wifi_small.h"


namespace MOONCAKE
{
    namespace APPS
    {
        class AppSetWiFi : public APP_BASE
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

        class AppSetWiFi_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "SetWiFi"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_set_wifi_big, image_data_set_wifi_small)); }
            void* newApp() override { return new AppSetWiFi; }
            void deleteApp(void *app) override { delete (AppSetWiFi*)app; }
        };
    }
}
