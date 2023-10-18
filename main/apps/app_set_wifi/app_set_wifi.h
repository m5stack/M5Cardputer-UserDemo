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
                enum State_t
                {
                    state_init = 0,
                    state_wait_ssid,
                    state_wait_password,
                    state_connect,
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
                    std::string wifi_ssid;
                    std::string wifi_password;
                };
                Data_t _data;
                void _update_input();
                void _update_cursor();
                void _update_state();

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
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
