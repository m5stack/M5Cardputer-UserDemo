/**
 * @file select_menu.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "../app_keyboard.h"
#include "spdlog/spdlog.h"
#include "../../utils/theme/theme_define.h"


using namespace MOONCAKE::APPS;


struct SelectItem_t
{
    std::string tag;
    int x = 0;
    int y = 0;
    SelectItem_t(std::string tag, int x, int y)
    {
        this->tag = tag;
        this->x = x;
        this->y = y;
    }
};



void AppKeyboard::_select_kb_type()
{
    int8_t selected = 0;
    bool is_select_change = true;
    std::vector<SelectItem_t> item_list;
    item_list.push_back(SelectItem_t("BLE Keyboard", 0, 32));
    item_list.push_back(SelectItem_t("USB Keyboard", 0, 56));

    while (1)
    {
        // Update select
        _data.hal->keyboard()->updateKeyList();
        _data.hal->keyboard()->updateKeysState();

        if (_data.hal->keyboard()->isKeyPressing(55) || _data.hal->keyboard()->isKeyPressing(54))
        {
            _data.hal->playKeyboardSound();

            // Hold till release 
            while (_data.hal->keyboard()->isKeyPressing(55) || _data.hal->keyboard()->isKeyPressing(54))
                _data.hal->keyboard()->updateKeyList();

            selected++;
            if (selected >= item_list.size())
                selected = 0;
            is_select_change = true;

            _data.hal->playNextSound();
        }
        else if (_data.hal->keyboard()->isKeyPressing(53) || _data.hal->keyboard()->isKeyPressing(40))
        {
            _data.hal->playKeyboardSound();

            // Hold till release 
            while (_data.hal->keyboard()->isKeyPressing(53) || _data.hal->keyboard()->isKeyPressing(40))
                _data.hal->keyboard()->updateKeyList();

            selected--;
            if (selected < 0)
                selected = item_list.size() - 1;
            is_select_change = true;

            _data.hal->playNextSound();
        }
        else if (_data.hal->keyboard()->isKeyPressing(42))
        {
            _data.hal->playKeyboardSound();

            // Hold till release 
            while (_data.hal->keyboard()->isKeyPressing(42))
                _data.hal->keyboard()->updateKeyList();

            _data.hal->playNextSound();

            // Quit 
            break;
        }


        // Render 
        if (is_select_change)
        {
            is_select_change = false;

            _data.hal->canvas()->fillScreen(THEME_COLOR_BG);
            _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
            _data.hal->canvas()->setCursor(0, 0);
            _data.hal->canvas()->print("[Select Type]");

            for (int i = 0; i < item_list.size(); i++)
            {
                if (i == selected)
                {
                    _data.hal->canvas()->fillSmoothCircle(item_list[i].x + 12, item_list[i].y + 8, 6, TFT_GREENYELLOW);
                    _data.hal->canvas()->setTextColor(TFT_GREENYELLOW, THEME_COLOR_BG);
                }
                else 
                    _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
                
                _data.hal->canvas()->setCursor(item_list[i].x + 26, item_list[i].y);
                _data.hal->canvas()->print(item_list[i].tag.c_str());
            }

            _data.hal->canvas_update();
        }
    }

    spdlog::info("selected {}", item_list[selected].tag);
    _data.kb_type = static_cast<KeyboardType_t>(selected);
}
