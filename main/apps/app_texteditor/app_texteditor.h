/**
 * @file app_texteditor.h
 * @author Anderson Antunes
 * @brief 
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 * 
 */
#include <mooncake.h>
#include "../../hal/hal.h"
#include "../utils/theme/theme_define.h"
#include "../utils/anim/anim_define.h"
#include "../utils/icon/icon_define.h"

#include "assets/texteditor_big.h"
#include "assets/texteditor_small.h"

namespace MOONCAKE {
    namespace APPS {
        enum DialogAction_t {
            none = 0,
            delete_file,
            format,
        };
        class AppTextEditor : public APP_BASE {
            private:
                struct Data_t {
                    HAL::Hal* hal = nullptr;
                    int last_key_num = 0;
                    int lines = 1;
                    std::string text_buffer;
                    uint32_t cursor_update_time_count = 0;
                    uint32_t cursor_update_period = 500;
                    bool cursor_state = false;
                    bool waiting_user_input = false;
                    bool can_skip = false;
                    DialogAction_t dialog_action = DialogAction_t::none;
                };
                Data_t _data;

                void _update_input();
                void _update_cursor();
                void _dialog(const std::string& message, bool can_skip);

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
        };

        class AppTextEditor_Packer : public APP_PACKER_BASE {
            std::string getAppName() override { return "NOTEPAD"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_texteditor_big, image_data_texteditor_small)); }
            void* newApp() override { return new AppTextEditor; }
            void deleteApp(void *app) override { delete (AppTextEditor*)app; }
        };
    }
}