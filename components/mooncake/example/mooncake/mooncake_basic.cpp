/**
 * @file mooncake_basic.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "spdlog/spdlog.h"
#include <cstdint>
#include <iostream>
#include <mooncake.h>


using namespace MOONCAKE;



void _data_base_setup_callback(SIMPLEKV::SimpleKV& db)
{
    spdlog::warn("check database: {} {} {} {} {}",
        db.Get(MC_DB_DISP_HOR)->value<MC_DB_DISP_HOR_T>(),
        db.Get(MC_DB_DISP_VER)->value<MC_DB_DISP_VER_T>(),
        db.Get(MC_DB_DISP_BRIGHTNESS)->value<MC_DB_DISP_BRIGHTNESS_T>(),
        db.Get(MC_DB_BATTERY_LEVEL)->value<MC_DB_BATTERY_LEVEL_T>(),
        db.Get(MC_DB_BATTERY_IS_CHARGING)->value<MC_DB_BATTERY_IS_CHARGING_T>()
    );
}


int main()
{
    std::cout << "[Mooncake basic example]\n";


    Mooncake mooncake;


    mooncake.setDatabaseSetupCallback(_data_base_setup_callback);


    mooncake.init();

    for (int i = 0; i < 66; i++)
        mooncake.update();


    return 0;
}
