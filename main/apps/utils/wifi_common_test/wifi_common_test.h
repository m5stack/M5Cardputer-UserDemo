/**
 * @file wifi_common_test.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <iostream>
#include <string>
#include <vector>


namespace WIFI_COMMON_TEST
{
    static bool is_nvs_inited = false;
    void nvs_init();

    namespace SCAN
    {
        struct ScanResult_t
        {
            std::string ssid;
            int8_t rssi = 0;
        };
        uint8_t scan(std::string& result);
        void scan(std::vector<ScanResult_t>& scanResult);
    }
    
}
