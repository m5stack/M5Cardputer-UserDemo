/**
 * @file anim_define.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "../common_define.h"


#define ANIM_APP_OPEN() for (int i = 10; i < 123; i+=8) \
    { _data.hal->canvas()->fillSmoothCircle(_data.hal->canvas()->width() / 2, _data.hal->canvas()->height() / 2 - 10, i, THEME_COLOR_BG); \
    _data.hal->canvas_update(); } \


#define ANIM_APP_CLOSE() for (int i = 123; i > 10; i-=8) \
    { _data.menu->update(millis()); \
    _data.hal->canvas()->fillSmoothCircle(_data.hal->canvas()->width() / 2, _data.hal->canvas()->height() / 2 - 10, i, THEME_COLOR_BG); \
    _data.hal->canvas_update(); } 


