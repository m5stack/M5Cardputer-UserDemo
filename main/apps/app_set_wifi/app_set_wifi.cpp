/**
 * @file app_set_wifi.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_set_wifi.h"
#include "spdlog/spdlog.h"


using namespace MOONCAKE::APPS;


void AppSetWiFi::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


void AppSetWiFi::onRunning()
{
    destroyApp();
}
