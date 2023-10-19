/**
 * @file cardputer.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.6
 * @date 2023-10-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <mooncake.h>
#include "hal/hal_cardputer.h"
#include "apps/apps.h"


using namespace HAL;
using namespace MOONCAKE;


HalCardputer hal;
Mooncake mooncake;


void _data_base_setup_callback(SIMPLEKV::SimpleKV& db) 
{
    // DI
    db.Add<HAL::Hal*>("HAL", &hal);
    spdlog::info("hal injection: {}", db.Get("HAL")->value<HAL::Hal*>()->type());
}


// #define ON_HAL_TEST
// #define ON_APP_TEST 1
#define ON_APP_TEST_WITH_LAUNCHER 1
extern "C" void app_main(void)
{
    // Init hal 
    hal.init();


    // ------------------------------------------------------------------ //
    #ifdef ON_HAL_TEST
        spdlog::info("on hal test");

        // HalCardputer::MicTest(&hal);
        // HalCardputer::SpeakerTest(&hal);

        HalCardputer::LcdBgLightTest(&hal);
    // ------------------------------------------------------------------ //


    // ------------------------------------------------------------------ //
    #elif ON_APP_TEST
        mooncake.setDatabaseSetupCallback(_data_base_setup_callback);
        mooncake.init();


        // auto test_app = new APPS::AppRecord_Packer;
        // auto test_app = new APPS::AppWifiScan_Packer;
        // auto test_app = new APPS::AppTimer_Packer;
        // auto test_app = new APPS::AppREPL_Packer;
        // auto test_app = new APPS::AppChat_Packer;
        auto test_app = new APPS::AppIR_Packer;


        mooncake.installApp(test_app);
        mooncake.startApp(mooncake.createApp(test_app));
        while (1)
            mooncake.update();
    // ------------------------------------------------------------------ //


    // ------------------------------------------------------------------ //
    #elif ON_APP_TEST_WITH_LAUNCHER
        // Init framework 
        mooncake.setDatabaseSetupCallback(_data_base_setup_callback);
        mooncake.init();

        // Install launcher
        auto launcher = new APPS::Launcher_Packer;
        mooncake.installApp(launcher);

        // mooncake.installApp(new APPS::AppREPL_Packer);
        // mooncake.installApp(new APPS::AppChat_Packer);
        // mooncake.installApp(new APPS::AppRecord_Packer);
        // mooncake.installApp(new APPS::AppWifiScan_Packer);
        mooncake.installApp(new APPS::AppSetWiFi_Packer);
        mooncake.installApp(new APPS::AppIR_Packer);

        // Create launcher 
        mooncake.createApp(launcher);
        while (1)
            mooncake.update();
    #else
    // ------------------------------------------------------------------ //



    // ------------------------------------------------------------------ //
    // Init framework 
    mooncake.setDatabaseSetupCallback(_data_base_setup_callback);
    mooncake.init();

    // Install launcher
    auto launcher = new APPS::Launcher_Packer;
    mooncake.installApp(launcher);

    // Install apps 
    mooncake.installApp(new APPS::AppWifiScan_Packer);
    mooncake.installApp(new APPS::AppRecord_Packer);
    mooncake.installApp(new APPS::AppChat_Packer);
    mooncake.installApp(new APPS::AppIR_Packer);
    mooncake.installApp(new APPS::AppREPL_Packer);
    mooncake.installApp(new APPS::AppSetWiFi_Packer);
    mooncake.installApp(new APPS::AppTimer_Packer);

    // Create launcher 
    mooncake.createApp(launcher);

    // Update framework 
    while (1)
        mooncake.update();
    // ------------------------------------------------------------------ //

    #endif
}
