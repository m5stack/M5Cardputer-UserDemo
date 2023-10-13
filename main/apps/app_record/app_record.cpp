/**
 * @file app_record.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-19
 * http://gitlab.m5stack.com/Forairaaaaa/stamps3_keypad_factory_test/blob/master/code/src/factory_test/mic/mic_test.cpp
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_record.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"


using namespace MOONCAKE::APPS;


void AppRecord::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


static constexpr const size_t record_number = 256;
static constexpr const size_t record_length = 200;
static constexpr const size_t record_size = record_number * record_length;
static constexpr const size_t record_samplerate = 16000;
static int16_t prev_y[record_length];
static int16_t prev_h[record_length];
static size_t rec_record_idx = 2;
static size_t draw_record_idx = 0;
static int16_t *rec_data;

#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _speaker _data.hal->Speaker()
#define _mic _data.hal->mic()


void AppRecord::onResume()
{
    ANIM_APP_OPEN();




    // _canvas->setCursor(0, 0);
    // _canvas->print(" REC");
    // rec_data = (typeof(rec_data))heap_caps_malloc(record_size * sizeof(int16_t), MALLOC_CAP_8BIT);

    rec_data = (typeof(rec_data))malloc(record_size * sizeof(int16_t));

    memset(rec_data, 0 , record_size * sizeof(int16_t));
    _canvas_update();
    
    /// Since the microphone and speaker cannot be used at the same time, turn off the speaker here.
    _speaker->end();
    _speaker->setVolume(255);

    _mic->begin();

    _canvas->fillScreen(THEME_COLOR_BG);
    _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);


    _canvas->setCursor(10, 0);
    _canvas->setFont(FONT_REPL);
    _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    _canvas->setTextSize(1);
    _canvas->print("Press enter to play");
}


void AppRecord::onRunning()
{
    // while (1)
    // {



    
    if (_mic->isEnabled())
    {
        static constexpr int shift = 6;
        auto data = &rec_data[rec_record_idx * record_length];
        // if (_mic->record(data, record_length, record_samplerate))

        if (_mic->record(data, record_length, record_samplerate))
        {
            // _canvas->fillScreen(TFT_BLACK);

            data = &rec_data[draw_record_idx * record_length];

            int32_t w = _canvas->width();
            if (w > record_length - 1)
            {
                w = record_length - 1;
            }

            // Align center 
            static int y_offset = 0;
            static int last_y_offset = 0;
            static int average = 0;

            average = 0;
            for (int32_t i = 0; i < w; i++)
            {
                average += prev_y[i] / 3;
            }
            y_offset = _canvas->height() / 2 - average / w;
            // spdlog::info("{} {}", average / w, y_offset);


            for (int32_t x = 0; x < w; ++x)
            {
                // _canvas->writeFastVLine(x, prev_y[x], prev_h[x], TFT_BLACK);
                // printf("%d\n", prev_y[x]);

                // _canvas->writeFastVLine(x + 10, prev_y[x] / 3 + y_offset, prev_h[x], THEME_COLOR_BG);

                // Wipe last one 
                _canvas->writeFastVLine(x + 10, prev_y[x] / 3 + last_y_offset, prev_h[x], THEME_COLOR_BG);


                int32_t y1 = (data[x] >> shift);
                int32_t y2 = (data[x + 1] >> shift);
                if (y1 > y2)
                {
                    int32_t tmp = y1;
                    y1 = y2;
                    y2 = tmp;
                }
                int32_t y = (_canvas->height() >> 1) + y1;
                int32_t h = (_canvas->height() >> 1) + y2 + 1 - y;
                prev_y[x] = y;
                prev_h[x] = h;

                _canvas->writeFastVLine(x + 10, y / 3 + y_offset, h, TFT_WHITE);

            }

            last_y_offset = y_offset;


            // _canvas->fillRect(0, 0, _canvas->width(), 25, _canvas->color565(20, 20, 20));
            // _canvas->setTextSize(2);
            // _canvas->setTextColor(TFT_WHITE);
            // _canvas->drawCenterString("MIC Test", _canvas->width() / 2, 6);
            // _canvas->setTextColor(TFT_ORANGE);

            // _canvas->setCursor(0, 40);
            // _canvas->print(" REC\n");
            // _canvas->setTextSize(1);
            // _canvas->print("  Press enter to play");
            // _canvas->setTextSize(2);


            _canvas->setCursor(10, 0);
            _canvas->setFont(FONT_REPL);
            _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
            _canvas->setTextSize(1);
            _canvas->print("Press enter to play");


            _canvas_update();

            if (++draw_record_idx >= record_number)
            {
                draw_record_idx = 0;
            }
            if (++rec_record_idx >= record_number)
            {
                rec_record_idx = 0;
            }
        }
    }


    // spdlog::info("{} {}", _data.hal->keyboard()->getKey().x, _data.hal->keyboard()->getKey().y);
    auto pressing_key = _data.hal->keyboard()->getKey();
    if (pressing_key.x >= 0)
    // if (1)
    {
        // if (_keypad.getKeyValue(_keypad.getKey()).value_first == "opt")
        if (strcmp(_data.hal->keyboard()->getKeyValue(_data.hal->keyboard()->getKey()).value_first, "opt") == 0)
        {
            // printf("1111\n");

            auto cfg = _mic->config();
            cfg.noise_filter_level = (cfg.noise_filter_level + 8) & 255;
            _mic->config(cfg);
            _canvas->setCursor(0, 110);
            _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
            _canvas->printf(" nf:%03d", cfg.noise_filter_level);
            _canvas_update();
        }

        // else if (_keypad.getKeyValue(_keypad.getKey()).value_first == "enter")
        else if (strcmp(_data.hal->keyboard()->getKeyValue(_data.hal->keyboard()->getKey()).value_first, "enter") == 0)
        // if (_data.hal->homeButton()->pressed())
        {
            // printf("2222\n");


            if (_speaker->isEnabled())
            {
                // _canvas->clear();
                _canvas->fillScreen(THEME_COLOR_BG);
                while (_mic->isRecording())
                {
                    delay(1);
                }

                /// Since the microphone and speaker cannot be used at the same time, turn off the microphone here.
                _mic->end();
                _speaker->begin();

                // _canvas->setCursor(0, 40);
                // _canvas->print(" PLAY");


                _canvas->setCursor(10, 0);
                _canvas->setFont(FONT_REPL);
                _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
                _canvas->setTextSize(1);
                _canvas->print("playing");



                // _canvas->fillRect(0, 0, _canvas->width(), 25, _canvas->color565(20, 20, 20));
                // _canvas->setTextSize(2);
                // _canvas->setTextColor(TFT_WHITE);
                // _canvas->drawCenterString("MIC Test", _canvas->width() / 2, 6);
                // _canvas->setTextColor(TFT_ORANGE);

                _canvas_update();



                int start_pos = rec_record_idx * record_length;
                if (start_pos < record_size)
                {
                    // _speaker->playRaw(&rec_data[start_pos], record_size - start_pos, record_samplerate, false, 1, 0);
                    // _speaker->playRaw(&rec_data[start_pos], record_size - start_pos, record_samplerate, true, 1, 0);
                    _speaker->playRaw(&rec_data[start_pos], record_size - start_pos, 22050, false);
                }
                if (start_pos > 0)
                {
                    // _speaker->playRaw(rec_data, start_pos, record_samplerate, false, 1, 0);
                    // _speaker->playRaw(rec_data, start_pos, record_samplerate, true, 1, 0);
                    _speaker->playRaw(rec_data, start_pos, 22050, false);
                }
                do
                {
                    delay(1);
                } while (_speaker->isPlaying());

                /// Since the microphone and speaker cannot be used at the same time, turn off the speaker here.
                _speaker->end();
                _mic->begin();

                // _canvas->clear();
                _canvas->fillScreen(THEME_COLOR_BG);
                // _canvas->setCursor(0, 40);
                // _canvas->print(" REC\n");
                // _canvas->setTextSize(1);
                // _canvas->print("  Press enter to play");
                // _canvas->setTextSize(2);


                _canvas->setCursor(10, 0);
                _canvas->setFont(FONT_REPL);
                _canvas->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
                _canvas->setTextSize(1);
                _canvas->print("Press enter to play");


                // _canvas->fillRect(0, 0, _canvas->width(), 25, _canvas->color565(20, 20, 20));
                // _canvas->setTextSize(2);
                // _canvas->setTextColor(TFT_WHITE);
                // _canvas->drawCenterString("MIC Test", _canvas->width() / 2, 6);
                // _canvas->setTextColor(TFT_ORANGE);


                _canvas_update();
            }
        }
    }


    // }
    


    else if (_data.hal->homeButton()->pressed()) 
    {
        while (_mic->isRecording()) { delay(1); }

        /// Since the microphone and speaker cannot be used at the same time, turn off the microphone here.
        _mic->end();
        _speaker->begin();

        free(rec_data);
        // break;

        _data.hal->playNextSound();
        destroyApp();
    }
}

