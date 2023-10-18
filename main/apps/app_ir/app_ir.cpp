/**
 * @file app_ir.cpp
 * @author Forairaaaaa
 * @brief http://gitlab.m5stack.com/Forairaaaaa/stamps3_keypad_factory_test/blob/master/code/src/factory_test/ir/ir_test.cpp
 * @version 0.6
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_ir.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"
#include "../utils/ir_wrap/ir_wrap.h"


using namespace MOONCAKE::APPS;


#define _canvas _data.hal->canvas()


void AppIR::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();


    _data.hal->canvas()->fillScreen(THEME_COLOR_BG);
    _data.hal->canvas()->setFont(FONT_REPL);
    _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    _data.hal->canvas()->setTextSize(1);
    _data.hal->canvas()->setTextScroll(false);
    ir_wrap_init();
}


void AppIR::onRunning()
{
    if ((millis() - _data.count) > 800) 
    {
        _data.cmd++;

        // sendNEC(IR_SEND_PIN, 0x10, _data.cmd, 0);
        ir_wrap_send(0x10, _data.cmd);

        printf("ir sended: addr: 0x%02X cmd:0x%02X\n", 0x10, _data.cmd);
        _data.count = millis();
    }




    if ((millis() - _data.anim_count) > 400)  {

        _data.anim_pos++;
        if (_data.anim_pos > 3) {
            _data.anim_pos = 0;
        }

        
        
        // _canvas->fillRect(0, 0, 240, 135, TFT_BLACK);
        // // _canvas->setSwapBytes(true);
        // // _canvas->pushImage(180, 16, 48, 48, (uint16_t *)icon_ir);
        // _canvas->setTextColor(_canvas->color565(180, 180, 180));
        // _canvas->setTextSize(3);
        // _canvas->setCursor(12, 20);
        // _canvas->printf("IR Send ");
        // _canvas->setTextSize(5);

        _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
        _data.hal->canvas()->setCursor(10, 5);
        _data.hal->canvas()->setTextSize(1);
        _data.hal->canvas()->print("IR Send");

        
        _canvas->setCursor(12, 40);
        _canvas->setTextSize(3);


        switch (_data.anim_pos) {
            case 0:
                _canvas->printf("0x%02X >    ", _data.cmd);
                break;
            case 1:
                _canvas->printf("0x%02X >>   ", _data.cmd);
                break;
            case 2:
                _canvas->printf("0x%02X  >>  ", _data.cmd);
                break;
            case 3:
                _canvas->printf("0x%02X   >  ", _data.cmd);
                break;
            default:
                break;
        }


        _data.hal->canvas_update();


        _data.anim_count = millis();
    }


    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit ir");
        destroyApp();
    }
}

