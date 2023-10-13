/**
 * @file app_ir.h
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

#include "assets/ir_big.h"
#include "assets/ir_small.h"


namespace MOONCAKE
{
    namespace APPS
    {
        class AppIR : public APP_BASE
        {
            private:
                struct Data_t
                {
                    HAL::Hal* hal = nullptr;

                    uint8_t cmd = 0x00;
                    unsigned long count = millis();
                    unsigned long anim_count = millis();
                    uint8_t anim_pos = 0;
                };
                Data_t _data;

            public:
                void onCreate() override;
                void onResume() override { ANIM_APP_OPEN(); }
                void onRunning() override;
        };

        class AppIR_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "REMOTE"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_ir_big, image_data_ir_small)); }
            void* newApp() override { return new AppIR; }
            void deleteApp(void *app) override { delete (AppIR*)app; }
        };
    }
}
