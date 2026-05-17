/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../app_launcher.h"
#include "assets/boot_sfx.h"
#include "assets/logo_adv.h"
#include "assets/logo.h"
#include <apps/utils/common.h>
#include <apps/utils/audio/audio.h>
#include <smooth_ui_toolkit.h>
#include <mooncake_log.h>
#include <hal.h>

// #define NO_BOOT_PLAY

using namespace smooth_ui_toolkit;

void start_arkanoid();

static void fancy_logo_fade_in()
{
    GetHAL().display.pushImage(63, 32, 114, 62, image_data_logo_adv);

    AnimateValue brightness;
    brightness.easingOptions().duration = 600;

    GetHAL().display.setBrightness(0);
    brightness      = 255;
    auto time_count = GetHAL().millis();
    while (GetHAL().millis() - time_count < 1200) {
        GetHAL().feedTheDog();
        GetHAL().update();
        GetHAL().delay(10);

        GetHAL().display.setBrightness(brightness);
    }

    GetHAL().display.setBrightness(255);
}

void Launcher::boot_anim()
{
    mclog::tagInfo(getAppInfo().name, "start boot anim");

    // GetHAL().delay(300);  // Codec init takes some time

    // Restore quiet mode preference before playing anything
    audio::set_quiet_mode(GetHAL().getSettings().GetBool("quiet_mode", false));

    // If software restart
    if (esp_reset_reason() != ESP_RST_POWERON) {
        mclog::tagInfo(getAppInfo().name, "not power on reset, skip boot anim");
        GetHAL().display.setBrightness(255);
        return;
    }

    fancy_logo_fade_in();

    // Show boot image
    GetHAL().display.pushImage(0, 0, 240, 135, image_data_logo);
    GetHAL().display.setFont(&fonts::efontCN_16);
    GetHAL().display.setTextColor((uint32_t)0x999999);
    int textWidth = GetHAL().display.textWidth(FW_VERSION);
    GetHAL().display.drawString(FW_VERSION, 235 - textWidth, 115);

    // Play sfx
#ifndef NO_BOOT_PLAY
    if (!audio::is_quiet_mode()) {
        GetHAL().speaker.setVolume(audio::DEFAULT_VOLUME);
        GetHAL().speaker.playWav(boot_sfx, sizeof(boot_sfx));
    }
#endif

    // Wait enter
    int egg_count = 0;
    while (1) {
        GetHAL().feedTheDog();
        GetHAL().delay(50);
        GetHAL().update();

        if (GetHAL().homeButton.wasPressed()) {
            GetHAL().speaker.setVolume(audio::DEFAULT_VOLUME);
            audio::play_random_tone();
            break;
        }
        auto key_event = GetHAL().keyboard.getLatestKeyEvent();

        if (!key_event.state && key_event.keyCode == KEY_G) {
            for (int i = 0; i < 3; i++) {
                GetHAL().delay(40);
                audio::play_random_tone();
            }
            egg_count++;
            if (egg_count > 2) {
                while (1) {
                    start_arkanoid();
                }
            }

        } else if (!key_event.state && key_event.keyCode == KEY_Q) {
            // During the boot screen, Q can only enable quiet mode
            if (!audio::is_quiet_mode()) {
                audio::set_quiet_mode(true);
                GetHAL().getSettings().SetBool("quiet_mode", true);
            }
            if (egg_count) {
                // Propagate quiet mode to elsewhere
                egg_count--;
            } else {
                break;
            }

        } else if (!key_event.state && key_event.keyCode != KEY_NONE) {
            break;
        }
    }

    GetHAL().keyboard.clearKeyEvent();
}
