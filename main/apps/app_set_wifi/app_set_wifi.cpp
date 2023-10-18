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
#include "../utils/wifi_connect_wrap/wifi_connect_wrap.h"
#include "../utils/sntp_wrap/sntp_wrap.h"


using namespace MOONCAKE::APPS;


void AppSetWiFi::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


void AppSetWiFi::onResume() 
{ 
    ANIM_APP_OPEN(); 


    wifi_connect_wrap_config("M5-R&D", "echo\"password\">/dev/null");
    // wifi_connect_wrap_connect();

    sntp_warp_init();
    
    _data.hal->setSntpAdjusted(true);
}


void AppSetWiFi::onRunning()
{
    destroyApp();
}
