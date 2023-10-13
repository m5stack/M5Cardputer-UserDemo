/**
 * @file keyboard_bar.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "../../launcher.h"
#include "lgfx/v1/lgfx_fonts.hpp"
#include "spdlog/spdlog.h"
#include "../../../utils/theme/theme_define.h"
#include "../../../utils/common_define.h"

#include "assets/Aa.h"
#include "assets/Aa0.h"
#include "assets/alt.h"
#include "assets/alt0.h"
#include "assets/crtl.h"
#include "assets/ctrl0.h"
#include "assets/fn.h"
#include "assets/fn0.h"
#include "assets/opt.h"
#include "assets/opt1.h"


using namespace MOONCAKE::APPS;


void Launcher::_start_keyboard_bar()
{
    // _data.hal->canvas_keyboard_bar()->fillScreen(TFT_RED);
}

void Launcher::_update_keyboard_bar()
{
    if ((millis() - _data.keyboard_bar_update_count) > _data.keyboard_bar_update_preiod)
    {
        // Updata state 
        _port_update_keyboard_state();

        // Backgound 
        int margin_x = 4;
        int margin_y = 6;

        _data.hal->canvas_keyboard_bar()->fillScreen(THEME_COLOR_BG);
        _data.hal->canvas_keyboard_bar()->fillSmoothRoundRect(
            margin_x,
            margin_y, 
            _data.hal->canvas_keyboard_bar()->width() - margin_x * 2, 
            _data.hal->canvas_keyboard_bar()->height() - margin_y * 2, 
            (_data.hal->canvas_keyboard_bar()->height() - margin_y * 2) / 2,
            THEME_COLOR_KB_BAR
        );

        // render state 
        int x = 7;
        int y = 20;
        int width = 22;
        int height = 17;
        int gap_y = 3;

        if (_data.keybaord_state.caps_lock)
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_Aa);
        else  
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_Aa0);

        y = y + height + gap_y;
        if (_data.keybaord_state.fn)
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_fn);
        else  
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_fn0);

        y = y + height + gap_y;
        if (_data.keybaord_state.ctrl)
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_crtl);
        else  
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_ctrl0);

        y = y + height + gap_y;
        if (_data.keybaord_state.opt)
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_opt);
        else  
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_opt1);

        y = y + height + gap_y;
        if (_data.keybaord_state.alt)
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_alt);
        else  
            _data.hal->canvas_keyboard_bar()->pushImage(x, y, width, height, image_data_alt0);


        // Push 
        _data.hal->canvas_keyboard_bar_update();
        
        // Reset flag
        _data.keyboard_bar_update_count = millis();
    }
}



