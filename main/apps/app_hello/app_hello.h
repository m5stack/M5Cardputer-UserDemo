/**
 * @file app_hello.h
 * @author Logimancer
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
//#pragma once
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

//                    int last_key_num = 0;
//                    char string_buffer[100];
//                    std::string repl_input_buffer;
//                    bool is_caps = false;
//                    char* value_buffer = nullptr;
//
//                    uint32_t cursor_update_time_count = 0;
//                    uint32_t cursor_update_period = 500;
//                    bool cursor_state = false;
                };
                Data_t _data;
//                void _update_input();
//                void _update_cursor();
//                void _update_input_panel();
//                void _update_receive();

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
