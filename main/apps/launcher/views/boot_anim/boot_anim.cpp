/**
 * @file boot_anim.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-09-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../../launcher.h"
#include "lgfx/v1/misc/enum.hpp"
#include "logo.h"
#include "spdlog/spdlog.h"

using namespace MOONCAKE::APPS;

void Launcher::_boot_anim()
{
    spdlog::info("start boot anim");

    // If software restart 
    if (esp_reset_reason() != ESP_RST_POWERON)
        return;

    // Show logo
    _data.hal->display()->pushImage(0, 0, 240, 135, image_data_logo);
    _data.hal->display()->setFont(&fonts::Font0);
    _data.hal->display()->setTextColor((uint32_t)0x999999, (uint32_t)0xE6E6E6);
    _data.hal->display()->drawString("Q=quiet", 6, 122);
    _port_wait_enter();
}
