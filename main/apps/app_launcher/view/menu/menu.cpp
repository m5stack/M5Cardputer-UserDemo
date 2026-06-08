/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "menu.h"
#include "../../app_launcher.h"
#include <apps/utils/audio/audio.h>
#include <apps/utils/theme.h>
#include <apps/utils/common.h>
#include <mooncake_log.h>
#include <mooncake.h>
#include <hal/hal.h>
#include <string>

using namespace mooncake;
using namespace smooth_ui_toolkit;

static const std::string _tag = "LauncherMenu";

void LauncherMenu::init(int launcherAppId)
{
    mclog::tagInfo(_tag, "init");

    // Add apps as option
    int i               = 0;
    auto installed_apps = GetMooncake().getAppAbilityManager()->getAllAbilityInstance();
    for (auto& app_raw : installed_apps) {
        auto app = static_cast<AppAbility*>(app_raw);

        // Skip launcher
        if (app->getId() == launcherAppId) {
            continue;
        }

        mclog::tagInfo(_tag, "add app: name: {} id: {}", app->getAppInfo().name, app->getId());
        _option_infos.push_back({app->getId(), app->getAppInfo().name});
        addOption({Vector4i{ICON_GAP + i * (ICON_WIDTH + ICON_GAP), ICON_MARGIN_TOP, ICON_WIDTH, ICON_WIDTH},
                   app->getAppInfo().userData});

        i++;
    }

    // Setup animation
    getSelectorPostion().x.springOptions().visualDuration = 0.4;
    getSelectorShape().x.springOptions().visualDuration   = 0.4;
    getSelectorShape().y.springOptions().visualDuration   = 0.4;

    // Default app number2
    moveTo(1);

    // Readinput in every update
    setConfig().readInputInterval = 0;
}

void LauncherMenu::onReadInput()
{
    auto event = GetHAL().keyboard.getLatestKeyEventRaw();
    // mclog::info("({},{}) {}", event.row, event.col, event.state);

    // Key pressed
    if (event.state == true) {
        // Left
        if (event.row == 3 && event.col == 10) {
            goLast();
        }
        // Right
        else if (event.row == 3 && event.col == 12) {
            goNext();
        }
        // Up
        else if (event.row == 2 && event.col == 11) {
            goLast();
        }
        // Down
        else if (event.row == 3 && event.col == 11) {
            goNext();
        }
        // Enter
        else if (event.row == 2 && event.col == 13) {
            press(getSelectedKeyframe());
        }
        // Q -- toggle quiet mode (KEY_Q)
        else if (event.row == 1 && event.col == 1) {
            bool new_quiet = !audio::is_quiet_mode();
            audio::set_quiet_mode(new_quiet);
            GetHAL().getSettings().SetBool("quiet_mode", new_quiet);
        }
    }
    // Key released
    else {
        // Enter
        if (event.row == 2 && event.col == 13) {
            release();
        }
    }
}

void LauncherMenu::onClick()
{
    // mclog::tagInfo(_tag, "on click {}", getSelectedOptionIndex());
    if (onAppOpen) {
        onAppOpen(getSelectedOptionIndex(), _option_infos[getSelectedOptionIndex()].appId);
    }
}

void LauncherMenu::onRender()
{
    auto selector_kf = getSelectorCurrentFrame();

    // Clear
    GetHAL().canvas.fillScreen(THEME_COLOR_BG);

    // X offset (keep selector ar the center)
    int x_offset = -(selector_kf.x) + GetHAL().canvas.width() / 2 - ICON_WIDTH / 2;

    // Font
    GetHAL().canvas.setFont(FONT_BASIC);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.setTextColor(THEME_COLOR_ICON, THEME_COLOR_BG);

    // Redner options
    int index = 0;
    for (auto& i : getOptionList()) {
        if (index == getSelectedOptionIndex()) {
            // Icon bg
            GetHAL().canvas.fillSmoothRoundRect(i.keyframe.x - (ICON_SELECTED_WIDTH - i.keyframe.width) / 2 + x_offset,
                                                i.keyframe.y - (ICON_SELECTED_WIDTH - i.keyframe.height) / 2,
                                                ICON_SELECTED_WIDTH, ICON_SELECTED_WIDTH, 8, THEME_COLOR_ICON);

            // Icon
            if (i.userData != nullptr) {
                GetHAL().canvas.pushImage(i.keyframe.x - (ICON_SELECTED_WIDTH - i.keyframe.width) / 2 + x_offset + 4,
                                          i.keyframe.y - (ICON_SELECTED_WIDTH - i.keyframe.height) / 2 + 4, 56, 56,
                                          ((AppIcon_t*)(i.userData))->iconBig);
            }

            // Draw name
            GetHAL().canvas.drawCenterString(
                _option_infos[index].name.c_str(), i.keyframe.x + i.keyframe.width / 2 + x_offset,
                i.keyframe.y + i.keyframe.height + ICON_TAG_MARGIN_TOP + (ICON_SELECTED_WIDTH - i.keyframe.width) / 2);
        } else {
            // Icon bg
            GetHAL().canvas.fillSmoothRoundRect(i.keyframe.x + x_offset, i.keyframe.y, i.keyframe.width,
                                                i.keyframe.height, 8, THEME_COLOR_ICON);

            // Icon
            if (i.userData != nullptr) {
                GetHAL().canvas.pushImage(i.keyframe.x + x_offset + 4, i.keyframe.y + 4, 40, 40,
                                          ((AppIcon_t*)(i.userData))->iconSmall);
            }

            // Draw tag
            GetHAL().canvas.drawCenterString(_option_infos[index].name.c_str(),
                                             i.keyframe.x + i.keyframe.width / 2 + x_offset,
                                             i.keyframe.y + i.keyframe.height + ICON_TAG_MARGIN_TOP);
        }

        index++;
    }

    if (pushCanvas) {
        GetHAL().pushCanvas();
    }
}

static LauncherMenu* _launcher_menu = nullptr;

void Launcher::start_menu()
{
    _launcher_menu = new LauncherMenu();
    _launcher_menu->init(getId());
    _launcher_menu->onAppOpen = [this](int index, int appId) { handle_app_open(index, appId); };
}

void Launcher::update_menu(bool pushCanvas)
{
    _launcher_menu->pushCanvas = pushCanvas;
    _launcher_menu->update();
}

void Launcher::handle_app_open(int index, int appId)
{
    // mclog::tagInfo(_tag, "handle app open: index: {} appId: {} running app id: {}", index, appId,
    // _data.running_app_id);

    GetMooncake().openApp(appId);
    _data.running_app_id = appId;

    ANIM_APP_OPEN();
}
