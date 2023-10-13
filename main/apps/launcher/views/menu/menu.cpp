/**
 * @file menu.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-09-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../../launcher.h"
#include "mc_conf_internal.h"
#include "spdlog/spdlog.h"
#include "menu_render_callback.hpp"
#include <string>
#include <vector>
#include "../../../utils/common_define.h"


using namespace MOONCAKE::APPS;


void Launcher::_start_menu()
{
    spdlog::info("start menu");

    /* Create a menu to handle selector */
    _data.menu = new SMOOTH_MENU::Simple_Menu;
    _data.menu_render_cb = new LauncherRender_CB_t;
    ((LauncherRender_CB_t*)_data.menu_render_cb)->setHal(_data.hal);

    _data.menu->init(_data.hal->canvas()->width(), _data.hal->canvas()->height());
    _data.menu->setRenderCallback(_data.menu_render_cb);

    /* Set selector anim */
    auto cfg_selector = _data.menu->getSelector()->config();
    cfg_selector.animPath_x = LVGL::ease_out;
    cfg_selector.animPath_y = LVGL::ease_out;
    cfg_selector.animTime_x = 400;
    cfg_selector.animTime_y = 400;
    _data.menu->getSelector()->config(cfg_selector);

    /* Set menu looply */
    _data.menu->setMenuLoopMode(true);


    // // Fake apps 
    // std::vector<std::string> app_list = {
    //     "TIMER",
    //     "RECORD",
    //     "REMOTE",
    //     "CHAT",
    //     "REPL",
    //     "SCAN",
    // };
    // for (int i = 0; i < app_list.size(); i++)
    // {
    //     _data.menu->getMenu()->addItem(
    //         app_list[i],
    //         ICON_GAP + i * (ICON_WIDTH + ICON_GAP),
    //         ICON_MARGIN_TOP,
    //         ICON_WIDTH,
    //         ICON_WIDTH
    //     );
    // }

    // Get install app list
    spdlog::info("installed apps num: {}", mcAppGetFramework()->getAppRegister().getInstalledAppNum());
    int i = 0;
    for (const auto& app : mcAppGetFramework()->getAppRegister().getInstalledAppList())
    {
        spdlog::info("app: {} icon: {}", app->getAppName(), app->getAppIcon());

        // Pass launcher 
        if (app->getAddr() == getAppPacker())
            continue;
        
        // Push items 
        _data.menu->getMenu()->addItem(
            app->getAppName(),
            ICON_GAP + i * (ICON_WIDTH + ICON_GAP),
            ICON_MARGIN_TOP,
            ICON_WIDTH,
            ICON_WIDTH,
            app->getAppIcon()
        );
        i++;
    }
}

void Launcher::_update_menu()
{
    if ((millis() - _data.menu_update_count) > _data.menu_update_preiod)
    {
        // Navigate 
        if (_port_check_last_pressed())
            _data.menu->goLast();
        else if (_port_check_next_pressed())
            _data.menu->goNext();

        // If pressed enter 
        if (_port_check_enter_pressed())
        {
            auto selected_item = _data.menu->getSelector()->getTargetItem();
            selected_item++;

            spdlog::info("select: {} try create", selected_item);

            // Create app
            _data._opened_app = mcAppGetFramework()->createApp(mcAppGetFramework()->getInstalledAppList()[selected_item]);
            spdlog::info("addr: {}", (void*)_data._opened_app);

            // Start app
            mcAppGetFramework()->startApp(_data._opened_app);

            // Stack launcher into background 
            closeApp();
        }

        // Update menu
        _data.menu->update(millis());
        _data.hal->canvas_update();

        // Reset flag
        _data.menu_update_count = millis();
    }
}
