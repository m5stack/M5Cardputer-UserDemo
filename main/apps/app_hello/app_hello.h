/**
 * @file app_hello.h
 * @author Logimancer
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 */

#include <mooncake.h>
#include "../../hal/hal.h"
#include "../utils/theme/theme_define.h"
#include "../utils/anim/anim_define.h"
#include "../utils/icon/icon_define.h"

#include "assets/hello_big.h"
#include "assets/hello_small.h"

namespace MOONCAKE
{
    namespace APPS
    {
        class AppHello : public APP_BASE
        {
            private:
                struct Data_t
                {
                    HAL::Hal* hal = nullptr;
                };
                Data_t _data;
                void _display_helloworld();

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
        };

        class AppHello_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "HELLO"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_hello_big, image_data_hello_small)); }
            void* newApp() override { return new AppHello; }
            void deleteApp(void *app) override { delete (AppHello*)app; }
        };
    }
}