/**
 * @file launcher_render_callback.hpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "../../../../hal/hal.h"
#include "../../../utils/smooth_menu/src/smooth_menu.h"
#include "../../../utils/icon/icon_define.h"
#include "lgfx/v1/lgfx_fonts.hpp"
#include "lgfx/v1/misc/enum.hpp"
#include <cstdint>
#include <cstdio>


#include "../../../utils/theme/theme_define.h"


struct LauncherRender_CB_t : public SMOOTH_MENU::SimpleMenuCallback_t
{
private:
    HAL::Hal* _hal;

public:
    inline void setHal(HAL::Hal* hal) { _hal = hal; }

    /* Override render callback */
    void renderCallback(
        const std::vector<SMOOTH_MENU::Item_t*>& menuItemList,
        const SMOOTH_MENU::RenderAttribute_t& selector,
        const SMOOTH_MENU::RenderAttribute_t& camera
    ) override
    {
        // Clear 
        _hal->canvas()->fillScreen(THEME_COLOR_BG);

        // X offset (keep selector ar the center)
        int x_offset = -(selector.x) + _hal->canvas()->width() / 2 - ICON_WIDTH / 2;

        // Font
        _hal->canvas()->setFont(FONT_BASIC);
        _hal->canvas()->setTextSize(1);
        _hal->canvas()->setTextColor(THEME_COLOR_ICON, THEME_COLOR_BG);

        // Render items
        for (const auto& item : menuItemList)
        {
            // Draw icon
            if (item->id == selector.targetItem)
            {
                // Icon bg
                _hal->canvas()->fillSmoothRoundRect(
                    item->x - (ICON_SELECTED_WIDTH - item->width) / 2 + x_offset, 
                    item->y - (ICON_SELECTED_WIDTH - item->height) / 2, 
                    ICON_SELECTED_WIDTH, 
                    ICON_SELECTED_WIDTH, 
                    8, 
                    THEME_COLOR_ICON
                );

                // Icon 
                if (item->userData != nullptr)
                {
                    _hal->canvas()->pushImage(
                        item->x - (ICON_SELECTED_WIDTH - item->width) / 2 + x_offset + 4, 
                        item->y - (ICON_SELECTED_WIDTH - item->height) / 2 + 4, 
                        56, 
                        56, 
                        ((AppIcon_t*)(item->userData))->iconBig
                    );
                }

                // Draw tag
                _hal->canvas()->drawCenterString(
                    item->tag.c_str(), 
                    item->x + item->width / 2 + x_offset, 
                    item->y + item->height + ICON_TAG_MARGIN_TOP + (ICON_SELECTED_WIDTH - item->width) / 2
                );
            }
            else
            {
                // Icon bg 
                _hal->canvas()->fillSmoothRoundRect(
                    item->x + x_offset, 
                    item->y, 
                    item->width, 
                    item->height, 
                    8, 
                    THEME_COLOR_ICON
                );

                // Icon 
                if (item->userData != nullptr)
                {
                    _hal->canvas()->pushImage(
                        item->x + x_offset + 4, 
                        item->y + 4, 
                        40, 
                        40, 
                        ((AppIcon_t*)(item->userData))->iconSmall
                    );
                }

                // Draw tag 
                _hal->canvas()->drawCenterString(
                    item->tag.c_str(), 
                    item->x + item->width / 2 + x_offset, 
                    item->y + item->height + ICON_TAG_MARGIN_TOP
                );
            }
        }

        // // Push canvas
        // _hal->canvas_update();
    }
};