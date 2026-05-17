/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_record.h"
#include "assets/record_big.h"
#include "assets/record_small.h"
#include <apps/utils/audio/audio.h>
#include <apps/utils/common.h>
#include <apps/utils/theme.h>
#include <mooncake_log.h>
#include <assets.h>

using namespace mooncake;

AppRecord::AppRecord()
{
    setAppInfo().name     = "Record";
    setAppInfo().userData = new AppIcon_t(image_data_record_big, image_data_record_small);
}

AppRecord::~AppRecord()
{
    delete static_cast<AppIcon_t*>(getAppInfo().userData);
    if (_rec_data) {
        delete[] _rec_data;
        _rec_data = nullptr;
    }
}

void AppRecord::onOpen()
{
    mclog::tagInfo(getAppInfo().name, "on open");

    audio::set_keyboard_sfx_enable(false);

    _rec_data = new int16_t[RECORD_SIZE]();

    start_recording();
    render_page_recording();
}

void AppRecord::onRunning()
{
    if (_is_recording && GetHAL().mic.isEnabled()) {
        render_waveform();
    }

    // Handle keyboard input
    auto key_event = GetHAL().keyboard.getLatestKeyEvent();
    if (key_event.state == true) {
        if (key_event.keyCode == KEY_ENTER) {
            handle_enter_key();
        }
    }

    // Close app when home button clicked
    if (GetHAL().homeButton.wasClicked()) {
        // GetHAL().speaker.setVolume(90);
        // audio::play_random_tone();
        close();
    }
}

void AppRecord::onClose()
{
    mclog::tagInfo(getAppInfo().name, "on close");

    // Stop recording if active
    while (GetHAL().mic.isRecording()) {
        GetHAL().delay(1);
    }

    // Cleanup audio devices
    GetHAL().mic.end();
    GetHAL().speaker.begin();
    GetHAL().speaker.setVolume(audio::DEFAULT_VOLUME);

    // Free memory
    if (_rec_data) {
        delete[] _rec_data;
        _rec_data = nullptr;
    }

    audio::set_keyboard_sfx_enable(true);
}

void AppRecord::start_recording()
{
    // Since microphone and speaker cannot be used at the same time, turn off speaker
    GetHAL().speaker.end();
    GetHAL().speaker.setVolume(255);

    auto cfg = GetHAL().mic.config();
    // cfg.over_sampling = 1;
    cfg.magnification      = 128;
    cfg.noise_filter_level = 2;
    GetHAL().mic.config(cfg);
    GetHAL().mic.begin();

    _is_recording = true;
}

void AppRecord::start_playback()
{
    if (!GetHAL().speaker.isEnabled()) {
        return;
    }

    // Stop recording and start playback
    while (GetHAL().mic.isRecording()) {
        GetHAL().delay(1);
    }

    GetHAL().mic.end();
    GetHAL().speaker.begin();
    GetHAL().speaker.setVolume(255);

    render_page_playing();

    // Play recorded data
    int start_pos = _rec_record_idx * RECORD_LENGTH;
    if (start_pos < RECORD_SIZE) {
        GetHAL().speaker.playRaw(&_rec_data[start_pos], RECORD_SIZE - start_pos, PLAYBACK_SAMPLERATE, false);
    }
    if (start_pos > 0) {
        GetHAL().speaker.playRaw(_rec_data, start_pos, PLAYBACK_SAMPLERATE, false);
    }

    // Wait for playback to finish
    do {
        GetHAL().delay(1);
    } while (GetHAL().speaker.isPlaying());

    // Resume recording
    start_recording();
    render_page_recording();
}

void AppRecord::render_page_recording()
{
    GetHAL().canvas.fillScreen(THEME_COLOR_BG);
    GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    GetHAL().canvas.setCursor(10, 0);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.print("Press enter to play");
    GetHAL().pushCanvas();
}

void AppRecord::render_page_playing()
{
    GetHAL().canvas.fillScreen(THEME_COLOR_BG);
    GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    GetHAL().canvas.setCursor(10, 0);
    GetHAL().canvas.setTextSize(1);
    GetHAL().canvas.print("playing");
    GetHAL().pushCanvas();
}

void AppRecord::render_waveform()
{
    if (!_rec_data) {
        return;
    }

    auto data = &_rec_data[_rec_record_idx * RECORD_LENGTH];

    if (GetHAL().mic.record(data, RECORD_LENGTH, RECORD_SAMPLERATE)) {
        data = &_rec_data[_draw_record_idx * RECORD_LENGTH];

        // 清除波形区域（避免重叠绘制）
        int32_t waveform_top    = 15;  // 文字下方
        int32_t waveform_height = GetHAL().canvas.height() - waveform_top;
        GetHAL().canvas.fillRect(10, waveform_top, RECORD_LENGTH, waveform_height, THEME_COLOR_BG);

        int32_t w = GetHAL().canvas.width();
        if (w > RECORD_LENGTH) {
            w = RECORD_LENGTH;
        }

        // 直接用录音数据画点 - 无需额外缓冲区
        int32_t center_y           = waveform_top + waveform_height / 2;
        static constexpr int shift = 8;  // 调整振幅显示

        for (int32_t x = 0; x < w; ++x) {
            // 计算波形点的y坐标
            int32_t sample_value = data[x] >> shift;
            int32_t y            = center_y + sample_value;

            // 限制在波形区域内
            if (y < waveform_top) y = waveform_top;
            if (y >= waveform_top + waveform_height) y = waveform_top + waveform_height - 1;

            // 画点 - 使用2x2像素块让点更明显
            GetHAL().canvas.drawPixel(x + 10, y, TFT_WHITE);
            GetHAL().canvas.drawPixel(x + 11, y, TFT_WHITE);
            GetHAL().canvas.drawPixel(x + 10, y + 1, TFT_WHITE);
            GetHAL().canvas.drawPixel(x + 11, y + 1, TFT_WHITE);
        }

        // 重绘UI文字
        GetHAL().canvas.setCursor(10, 0);
        GetHAL().canvas.setTextColor(TFT_ORANGE, THEME_COLOR_BG);
        GetHAL().canvas.setTextSize(1);
        GetHAL().canvas.print("Press enter to play");

        GetHAL().pushCanvas();

        // 更新循环缓冲区索引
        if (++_draw_record_idx >= RECORD_NUMBER) {
            _draw_record_idx = 0;
        }
        if (++_rec_record_idx >= RECORD_NUMBER) {
            _rec_record_idx = 0;
        }
    }
}

void AppRecord::handle_enter_key()
{
    start_playback();
}
