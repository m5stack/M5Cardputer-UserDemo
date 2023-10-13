/**
 * @file app_repl.h
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

#include "assets/repl_big.h"
#include "assets/repl_small.h"

extern "C"
{
    #include <pikaScript.h>
}


namespace MOONCAKE
{
    namespace APPS
    {
        class AppREPL : public APP_BASE
        {
            private:
                struct Data_t
                {
                    HAL::Hal* hal = nullptr;

                    int last_key_num = 0;
                    std::string repl_input_buffer;
                    bool is_caps = false;
                    char* value_buffer = nullptr;

                    uint32_t cursor_update_time_count = 0;
                    uint32_t cursor_update_period = 500;
                    bool cursor_state = false;
                };
                Data_t _data;

                void _update_input();
                void _update_cursor();

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
        };

        class AppREPL_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "REPL"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_repl_big, image_data_repl_small)); }
            void* newApp() override { return new AppREPL; }
            void deleteApp(void *app) override { delete (AppREPL*)app; }
        };
    }
}
