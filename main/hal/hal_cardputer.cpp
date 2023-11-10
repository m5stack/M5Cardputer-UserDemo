/**
 * @file hal_cardputer.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "hal_cardputer.h"
#include "display/hal_display.hpp"
#include <mooncake.h>
#include "../apps/utils/common_define.h"
#include "bat/adc_read.h"


using namespace HAL;


void HalCardputer::_display_init()
{
    spdlog::info("init display");

    // Display 
    _display = new LGFX_Cardputer;
    _display->init();
    _display->setRotation(1);

    // Canvas
    _canvas = new LGFX_Sprite(_display);
    _canvas->createSprite(204, 109);

    _canvas_keyboard_bar = new LGFX_Sprite(_display);
    _canvas_keyboard_bar->createSprite(_display->width() - _canvas->width(), display()->height());

    _canvas_system_bar = new LGFX_Sprite(_display);
    _canvas_system_bar->createSprite(_canvas->width(), _display->height() - _canvas->height());
}


void HalCardputer::_keyboard_init()
{
    _keyboard = new KEYBOARD::Keyboard;
    _keyboard->init();
}


void HalCardputer::_mic_init()
{
    spdlog::info("init mic");

    _mic = new m5::Mic_Class;

    // Configs 
    auto cfg = _mic->config();
    cfg.pin_data_in = 46;
    cfg.pin_ws = 43;
    cfg.magnification = 4;

    cfg.task_priority = 15;

    // cfg.dma_buf_count = 8;
    // cfg.dma_buf_len = 512;
    // cfg.stereo = true;
    // cfg.sample_rate = 16000;

    cfg.i2s_port = i2s_port_t::I2S_NUM_0;
    _mic->config(cfg);
}


void HalCardputer::_speaker_init()
{
    spdlog::info("init speaker");

    _speaker = new m5::Speaker_Class;

    auto cfg = _speaker->config();
    cfg.pin_data_out = 42;
    cfg.pin_bck = 41;
    cfg.pin_ws = 43;
    cfg.i2s_port = i2s_port_t::I2S_NUM_1;
    // cfg.magnification = 1;
    _speaker->config(cfg);
}


void HalCardputer::_button_init()
{
    _homeButton = new Button(0);
    _homeButton->begin();
}


void HalCardputer::_bat_init()
{
    adc_read_init();
}


void HalCardputer::init()
{
    spdlog::info("hal init");

    _display_init();
    _keyboard_init();
    _speaker_init();
    _mic_init();
    _button_init();
    _bat_init();
}


double getBatVolBfb(double batVcc) //获取电压的百分比，经过换算并非线性关系
{
    double bfb = 0.0;

    //y = 497.50976 x4 - 7,442.07254 x3 + 41,515.70648 x2 - 102,249.34377 x + 93,770.99821
    bfb = 497.50976 * batVcc * batVcc * batVcc * batVcc
            - 7442.07254 * batVcc * batVcc * batVcc
            + 41515.70648 * batVcc * batVcc
            - 102249.34377 * batVcc
            + 93770.99821;
    if (bfb > 100) 
        bfb = 100.0;
    else if (bfb < 0) 
        bfb = 3.0;

    return bfb;
}


uint8_t HalCardputer::getBatLevel()
{
    // spdlog::info("get bat: {}", adc_read_get_value());
    // spdlog::info("bat level: {}", getBatVolBfb((double)(adc_read_get_value() * 2 / 1000)));

    // return 100;
    // return (uint8_t)getBatVolBfb(4.2);
    return (uint8_t)getBatVolBfb((double)((adc_read_get_value() * 2 + 100) / 1000));
}


void HalCardputer::MicTest(HalCardputer* hal)
{
    // hal->mic()->begin();

    int16_t mic_buffer[256];

    while (1) 
    {
        hal->mic()->record(mic_buffer, 256);
        while (hal->mic()->isRecording()) { vTaskDelay(5); }
        
        for (int i = 0; i < 256; i++)
            printf("m:%d\n", mic_buffer[i]);
    }
}


#include "../apps/utils/boot_sound/boot_sound_1.h"
#include "../apps/utils/boot_sound/boot_sound_2.h"

void HalCardputer::SpeakerTest(HalCardputer* hal)
{
    spdlog::info("speaker test");

    // hal->Speaker()->setVolume(32);
    hal->Speaker()->setVolume(128);

    while (1)
    {
        // hal->Speaker()->tone(4000, 200);
        // delay(500);
        // hal->Speaker()->tone(3000, 200);
        // delay(500);
        // hal->Speaker()->tone(6000, 200);
        // delay(500);
        // hal->Speaker()->tone(5000, 200);
        // delay(500);

        

        spdlog::info("boot 1");
        hal->Speaker()->playWav(boot_sound_1, sizeof(boot_sound_1));
        while (hal->Speaker()->isPlaying())
            delay(5);
        spdlog::info("boot 1");
        delay(1000);

        spdlog::info("boot 2");
        hal->Speaker()->playWav(boot_sound_2, sizeof(boot_sound_2));
        while (hal->Speaker()->isPlaying())
            delay(5);
        spdlog::info("boot 2");
        delay(1000);

    }

}


void HalCardputer::LcdBgLightTest(HalCardputer* hal)
{
    hal->display()->setTextSize(3);

    std::vector<int> color_list = {TFT_RED, TFT_GREEN, TFT_BLUE};
    for (auto i : color_list)
    {
        hal->display()->fillScreen(i);

        for (int i = 0; i < 256; i++)
        {
            hal->display()->setCursor(0, 0);
            hal->display()->printf("%d", i);
            hal->display()->setBrightness(i);
            delay(20);
        }
        delay(1000);

        for (int i = 255; i >= 0; i--)
        {
            hal->display()->setCursor(0, 0);
            hal->display()->printf("%d", i);
            hal->display()->setBrightness(i);
            delay(20);
        }
    }
}
