/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_keyboard.h"
#include "assets/keyboard_big.h"
#include "assets/keyboard_small.h"
#include <apps/utils/audio/audio.h>
#include <apps/utils/common.h>
#include <apps/utils/theme.h>
#include <mooncake_log.h>
#include <assets.h>

using namespace mooncake;
using namespace smooth_ui_toolkit;

AppKeyboard::AppKeyboard()
{
    setAppInfo().name     = "Keyboard";
    setAppInfo().userData = new AppIcon_t(image_data_keyboard_big, image_data_keyboard_small);
}

AppKeyboard::~AppKeyboard()
{
    delete static_cast<AppIcon_t*>(getAppInfo().userData);
}

void AppKeyboard::onOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");

    _info_update_time   = 0;
    _is_selecting       = true;
    _is_keyboard_active = false;

    // Create and initialize selector menu
    _selector_menu = new KeyboardSelectorMenu();
    _selector_menu->init();
    select_keyboard_type();
}

void AppKeyboard::onRunning()
{
    if (_is_selecting && _selector_menu) {
        // Update selector menu
        _selector_menu->update();

        // Check if selection is made
        if (_selector_menu->isSelected()) {
            auto keyboard_type = _selector_menu->getSelectedType();
            _is_selecting      = false;

            // Destroy selector menu
            delete _selector_menu;
            _selector_menu = nullptr;

            // Initialize selected keyboard type
            if (keyboard_type == KeyboardSelectorMenu::KEYBOARD_TYPE_BLE) {
                init_ble_keyboard();
            } else if (keyboard_type == KeyboardSelectorMenu::KEYBOARD_TYPE_USB) {
                init_usb_keyboard();
            }

            render_keyboard_interface();
            _is_keyboard_active = true;
        }
    } else if (_is_keyboard_active) {
        // Update connection info periodically
        update_connection_info();
    }

    // Close app when home button clicked
    if (GetHAL().homeButton.wasClicked()) {
        close();
    }
}

void AppKeyboard::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    // Clean up selector menu if still exists
    if (_selector_menu) {
        delete _selector_menu;
        _selector_menu = nullptr;
    }

    if (_is_keyboard_active) {
        // The best way to release everything
        // Unfortunately, a side-effect is that in-memory counters and
        // settings all get lost
        esp_restart();
        __builtin_unreachable();
    }

    close();
}

void AppKeyboard::select_keyboard_type()
{
    GetHAL().canvas.fillScreen(THEME_COLOR_BG);
    GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    GetHAL().canvas.setCursor(0, 0);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.println("[Select Keyboard Type]");
    GetHAL().canvas.setTextColor(TFT_WHITE, THEME_COLOR_BG);
    GetHAL().canvas.println("Use arrow keys to select");
    GetHAL().canvas.println("Press Enter to confirm");
    GetHAL().pushCanvas();
}

void AppKeyboard::init_ble_keyboard()
{
    mclog::tagInfo(getAppInfo().name, "initializing BLE keyboard");
    GetHAL().bleKeyboardInit();
}

void AppKeyboard::init_usb_keyboard()
{
    mclog::tagInfo(getAppInfo().name, "initializing USB keyboard");
    GetHAL().usbKeyboardInit();
}

void AppKeyboard::update_connection_info()
{
    // Update connection status every 2 seconds
    if (GetHAL().millis() - _info_update_time > 1000) {
        render_connection_status();
        _info_update_time = GetHAL().millis();
    }
}

void AppKeyboard::render_keyboard_interface()
{
    GetHAL().canvas.fillScreen(THEME_COLOR_BG);
    GetHAL().canvas.setTextColor(TFT_GREEN, THEME_COLOR_BG);
    GetHAL().canvas.setCursor(0, 0);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.println("Keyboard Mode Active");

    GetHAL().canvas.setTextColor(TFT_CYAN, THEME_COLOR_BG);
    GetHAL().canvas.println("Type to send keys...");
    GetHAL().pushCanvas();
}

void AppKeyboard::render_connection_status()
{
    // Clear status area
    GetHAL().canvas.fillRect(0, 32, GetHAL().canvas.width(), 20, THEME_COLOR_BG);
    GetHAL().canvas.setCursor(0, 32);

    // Check both BLE and USB connections
    bool ble_connected = GetHAL().bleKeyboardIsConnected();
    bool usb_connected = GetHAL().usbKeyboardIsConnected();

    if (ble_connected) {
        GetHAL().canvas.setTextColor(TFT_GREEN, THEME_COLOR_BG);
        GetHAL().canvas.printf("BLE: Connected");
    } else if (usb_connected) {
        GetHAL().canvas.setTextColor(TFT_GREEN, THEME_COLOR_BG);
        GetHAL().canvas.printf("USB: Connected");
    } else {
        GetHAL().canvas.setTextColor(TFT_RED, THEME_COLOR_BG);
        GetHAL().canvas.printf("Disconnected");
    }

    GetHAL().pushCanvas();
}
