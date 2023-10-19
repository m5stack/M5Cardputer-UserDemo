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
                enum State_t
                {
                    state_init = 0,
                    state_wait_input,
                    state_fire,
                };

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

                    State_t current_state = state_init;
                    std::vector<std::string> parse_result;
                };
                Data_t _data;

                void _update_input();
                void _update_cursor();
                void _update_state();
                void _customSplit(const std::string& str, char separator, std::vector<std::string>& result);

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
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
