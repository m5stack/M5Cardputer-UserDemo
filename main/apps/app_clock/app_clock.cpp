/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_clock.h"
#include "assets/timer_big.h"
#include "assets/timer_small.h"
#include <apps/utils/audio/audio.h>
#include <apps/utils/common.h>
#include <apps/utils/theme.h>
#include <mooncake_log.h>
#include <assets.h>

using namespace mooncake;

AppClock::AppClock()
{
    setAppInfo().name     = "Clock";
    setAppInfo().userData = new AppIcon_t(image_data_timer_big, image_data_timer_small);
}

AppClock::~AppClock()
{
    delete static_cast<AppIcon_t*>(getAppInfo().userData);
}

void AppClock::onOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");
    render_interface();
}

void AppClock::onRunning()
{
    // Update time display every second
    if (GetHAL().millis() - _time_count > UPDATE_INTERVAL) {
        update_time_display();
        _time_count = GetHAL().millis();
    }

    // Close app when home button clicked
    if (GetHAL().homeButton.wasClicked()) {
        audio::play_random_tone();
        close();
    }
}

void AppClock::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");
}

void AppClock::render_interface()
{
    GetHAL().canvas.fillScreen(THEME_COLOR_BG);
    GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    GetHAL().canvas.setTextSize(2);
    GetHAL().pushCanvas();
}

void AppClock::update_time_display()
{
    // We have proper time if we had a WiFi connection
    if (GetHAL().isTimeSynced()) {
        show_network_time();
    } else {
        show_system_time();
    }
}

void AppClock::show_network_time()
{
    // Get current time from system (assuming SNTP has synchronized it when WiFi was connected)
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    GetHAL().canvas.fillScreen(THEME_COLOR_BG);

    // Show time source indicator
    GetHAL().canvas.setCursor(10, 5);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.setTextColor(TFT_GREEN, THEME_COLOR_BG);
    GetHAL().canvas.print("Network Time");

    // Format and display time
    auto time_str = fmt::format("{:02d}:{:02d}:{:02d}", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    GetHAL().canvas.setTextSize(2);
    GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    GetHAL().canvas.drawCenterString(time_str.c_str(), GetHAL().canvas.width() / 2, GetHAL().canvas.height() / 2 - 20);

    // Show date
    auto date_str = fmt::format("{:04d}-{:02d}-{:02d}", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);

    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.setTextColor(TFT_WHITE, THEME_COLOR_BG);
    GetHAL().canvas.drawCenterString(date_str.c_str(), GetHAL().canvas.width() / 2, GetHAL().canvas.height() / 2 + 20);

    GetHAL().pushCanvas();
}

void AppClock::show_system_time()
{
    // Calculate system uptime
    uint32_t total_seconds = GetHAL().millis() / 1000;
    uint32_t hours         = total_seconds / 3600;
    uint32_t minutes       = (total_seconds % 3600) / 60;
    uint32_t seconds       = total_seconds % 60;

    GetHAL().canvas.fillScreen(THEME_COLOR_BG);

    // Show time source indicator
    GetHAL().canvas.setCursor(10, 5);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.setTextColor(TFT_YELLOW, THEME_COLOR_BG);
    GetHAL().canvas.print("System Uptime");

    // Format and display uptime
    auto uptime_str = fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, seconds);

    GetHAL().canvas.setTextSize(2);
    GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    GetHAL().canvas.drawCenterString(uptime_str.c_str(), GetHAL().canvas.width() / 2,
                                     GetHAL().canvas.height() / 2 - 20);

    // Show WiFi status hint
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.setTextColor(TFT_DARKGREY, THEME_COLOR_BG);
    GetHAL().canvas.drawCenterString("Set WiFi for real time", GetHAL().canvas.width() / 2,
                                     GetHAL().canvas.height() / 2 + 30);

    GetHAL().pushCanvas();
}
