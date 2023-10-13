/**
 * @file app_wifi_scan.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-19
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

#include "assets/scan_big.h"
#include "assets/scan_small.h"


namespace MOONCAKE
{
    namespace APPS
    {
        class AppWifiScan : public APP_BASE
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

        class AppWifiScan_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "SCAN"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_scan_big, image_data_scan_small)); }
            void* newApp() override { return new AppWifiScan; }
            void deleteApp(void *app) override { delete (AppWifiScan*)app; }
        };
    }
}
