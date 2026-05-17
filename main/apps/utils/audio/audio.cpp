/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <hal/hal.h>
#include <mooncake_log.h>
#include <random>
#include "audio.h"

namespace audio {

static bool quiet_mode_is_enabled = false;

static std::vector<int> c_major_scale = {60, 62, 64, 65, 67, 69, 71};  // C大调音阶（C D E F G A B）

void play_tone(int frequency, double durationSec)
{
    if (GetHAL().speaker.getVolume() <= 0) {
        return;
    }

    const int sample_rate = GetHAL().speaker.config().sample_rate;
    const int samples     = static_cast<int>(sample_rate * durationSec);
    std::vector<int16_t> buffer(samples * 2);  // 双声道

    const int fade_len    = 200;  // 淡出长度（采样点）
    const float amplitude = 32767.0f / 5;

    for (int i = 0; i < samples; ++i) {
        float amp = amplitude;

        // 应用结尾淡出（fade-out）
        if (i >= samples - fade_len) {
            float fade_factor = static_cast<float>(samples - i) / fade_len;
            amp *= fade_factor;
        }

        int16_t value     = static_cast<int16_t>(amp * sin(2.0 * M_PI * frequency * i / sample_rate));
        buffer[i * 2]     = value;  // 左声道
        buffer[i * 2 + 1] = value;  // 右声道
    }

    GetHAL().speaker.playRaw(buffer.data(), buffer.size());
}

void play_melody(const std::vector<int>& midiList, double durationSec)
{
    if (GetHAL().speaker.getVolume() <= 0) {
        return;
    }

    const int sample_rate      = GetHAL().speaker.config().sample_rate;
    const int samples_per_note = static_cast<int>(sample_rate * durationSec);
    const int fade_len         = 200;  // 每个音符结尾的淡出长度
    const float amplitude      = 32767.0f / 5;

    std::vector<int16_t> buffer;                             // 大 buffer 存放整首旋律
    buffer.reserve(midiList.size() * samples_per_note * 2);  // 双声道预留空间

    for (int midiNote : midiList) {
        for (int i = 0; i < samples_per_note; ++i) {
            float amp = amplitude;

            // 应用淡出（仅每个音符的结尾）
            if (i >= samples_per_note - fade_len) {
                float fade_factor = static_cast<float>(samples_per_note - i) / fade_len;
                amp *= fade_factor;
            }

            int16_t sample = 0;
            if (midiNote >= 0) {
                double freq = 440.0 * pow(2.0, (midiNote - 69) / 12.0);
                sample      = static_cast<int16_t>(amp * sin(2.0 * M_PI * freq * i / sample_rate));
            }

            buffer.push_back(sample);  // 左声道
            buffer.push_back(sample);  // 右声道
        }
    }

    GetHAL().speaker.playRaw(buffer.data(), buffer.size());
}

void play_tone_from_midi(int midi, double durationSec)
{
    if (GetHAL().speaker.getVolume() <= 0) {
        return;
    }

    double freq = 440.0 * std::pow(2.0, (midi - 69) / 12.0);
    play_tone(static_cast<int>(freq), durationSec);
}

void play_random_tone(int semitoneShift, double durationSec)
{
    // No random sounds during quiet mode
    if (GetHAL().speaker.getVolume() <= 0 || is_quiet_mode()) {
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(c_major_scale.size()) - 1);

    int index = dist(gen);
    int midi  = c_major_scale[index] + semitoneShift;

    play_tone_from_midi(midi, durationSec);
}

/* -------------------------------------------------------------------------- */
/*                                  Keyboard                                  */
/* -------------------------------------------------------------------------- */
static void _keyboard_sfx_on_key_event(const Keyboard::KeyEvent_t& event)
{
    // No keypress sounds during quiet mode
    if (!event.state || is_quiet_mode()) {
        return;
    }

    GetHAL().speaker.setVolume(DEFAULT_VOLUME);

    int semitoneShift = 48;
    switch (event.keyCode) {
        case KEY_1:
            play_tone_from_midi(c_major_scale[0] + semitoneShift, 0.02);
            return;
        case KEY_2:
            play_tone_from_midi(c_major_scale[1] + semitoneShift, 0.02);
            return;
        case KEY_3:
            play_tone_from_midi(c_major_scale[2] + semitoneShift, 0.02);
            return;
        case KEY_4:
            play_tone_from_midi(c_major_scale[3] + semitoneShift, 0.02);
            return;
        case KEY_5:
            play_tone_from_midi(c_major_scale[4] + semitoneShift, 0.02);
            return;
        case KEY_6:
            play_tone_from_midi(c_major_scale[5] + semitoneShift, 0.02);
            return;
        case KEY_7:
            play_tone_from_midi(c_major_scale[6] + semitoneShift, 0.02);
            return;
        default:
            play_random_tone(semitoneShift, 0.02);
    }
}

void set_keyboard_sfx_enable(bool enable)
{
    static size_t slot_id  = 0;
    static bool is_enabled = false;

    mclog::tagInfo("audio", "set keyboard sfx enable: {}", enable);
    if (enable) {
        if (is_enabled) {
            return;
        }
        is_enabled = true;
        slot_id    = GetHAL().keyboard.onKeyEvent.connect(_keyboard_sfx_on_key_event);
    } else {
        if (!is_enabled) {
            return;
        }
        is_enabled = false;
        GetHAL().keyboard.onKeyEvent.disconnect(slot_id);
    }
}

bool is_quiet_mode()
{
    return quiet_mode_is_enabled;
}

void set_quiet_mode(bool quiet)
{
    quiet_mode_is_enabled = quiet;
}

}  // namespace audio
