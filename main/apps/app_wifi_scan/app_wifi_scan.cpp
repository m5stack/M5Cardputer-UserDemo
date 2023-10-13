/**
 * @file app_record.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_wifi_scan.h"
#include "spdlog/spdlog.h"
#include "../utils/wifi_common_test/wifi_common_test.h"
#include "../utils/theme/theme_define.h"


using namespace MOONCAKE::APPS;
using namespace WIFI_COMMON_TEST;


void AppWifiScan::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}


static std::vector<SCAN::ScanResult_t> _scan_result;

void AppWifiScan::onResume()
{
    ANIM_APP_OPEN();


    _data.hal->canvas()->fillScreen(THEME_COLOR_BG);
    _data.hal->canvas()->setFont(FONT_REPL);
    _data.hal->canvas()->setTextColor(TFT_ORANGE, THEME_COLOR_BG);
    _data.hal->canvas()->setTextSize(1);
    _data.hal->canvas()->setCursor(10, 5);
    _data.hal->canvas()->print("Scanning...");
    _data.hal->canvas_update();


    SCAN::scan(_scan_result);


    _data.hal->canvas()->fillScreen(THEME_COLOR_BG);

    _data.hal->canvas()->setTextSize(1);
    // _data.hal->canvas()->setCursor(0, 5);
    // _data.hal->canvas()->printf("  Get %d :\n", _scan_result.size());

    _data.hal->canvas()->setCursor(0, 0);

    _data.hal->canvas()->setTextSize(1);
    int max_count = 0;
    for (auto& i : _scan_result)
    {
        max_count++;
        if (max_count > 6)
            break;

        spdlog::info("{} {}", i.ssid, i.rssi);

        _data.hal->canvas()->setTextColor((i.rssi > -65) ? TFT_GREEN : TFT_YELLOW, THEME_COLOR_BG);
    
        _data.hal->canvas()->printf("%d", i.rssi);
        _data.hal->canvas()->printf(" %s\n", i.ssid.c_str());
    
        _data.hal->canvas_update();
    }
}


void AppWifiScan::onRunning()
{
    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit wifi scan");
        destroyApp();
    }
}
