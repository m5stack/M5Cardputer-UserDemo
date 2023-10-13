/**
 * @file wifi_common_test.cpp
 * @author Forairaaaaa
 * @brief https://github.com/espressif/esp-idf/blob/v5.1/examples/wifi/scan/main/scan.c
 * @version 0.1
 * @date 2023-08-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "wifi_common_test.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"


namespace WIFI_COMMON_TEST
{
    /**
     * @brief Init nvs, only once
     * 
     */
    void nvs_init()
    {
        if (!is_nvs_inited)
        {
            ESP_LOGI("nvs", "init");
            is_nvs_inited = true;

            // Initialize NVS
            esp_err_t ret = nvs_flash_init();
            if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                ESP_ERROR_CHECK(nvs_flash_erase());
                ret = nvs_flash_init();
            }
            ESP_ERROR_CHECK( ret );
        }
    }


    /**
     * @brief Wifi scan 
     * 
     */
    namespace SCAN
    {
        #define DEFAULT_SCAN_LIST_SIZE 10

        static const char *TAG = "scan";

        static void print_auth_mode(int authmode)
        {
            switch (authmode) {
            case WIFI_AUTH_OPEN:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OPEN");
                break;
            // case WIFI_AUTH_OWE:
            //     ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_OWE");
            //     break;
            case WIFI_AUTH_WEP:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_PSK");
                break;
            case WIFI_AUTH_WPA2_PSK:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_PSK");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
                break;
            case WIFI_AUTH_WPA3_PSK:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA3_PSK");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
                break;
            default:
                ESP_LOGI(TAG, "Authmode \tWIFI_AUTH_UNKNOWN");
                break;
            }
        }

        static void print_cipher_type(int pairwise_cipher, int group_cipher)
        {
            switch (pairwise_cipher) {
            case WIFI_CIPHER_TYPE_NONE:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
                break;
            case WIFI_CIPHER_TYPE_WEP40:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
                break;
            case WIFI_CIPHER_TYPE_WEP104:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
                break;
            case WIFI_CIPHER_TYPE_TKIP:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
                break;
            case WIFI_CIPHER_TYPE_CCMP:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
                break;
            case WIFI_CIPHER_TYPE_TKIP_CCMP:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
                break;
            case WIFI_CIPHER_TYPE_AES_CMAC128:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_AES_CMAC128");
                break;
            case WIFI_CIPHER_TYPE_SMS4:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_SMS4");
                break;
            case WIFI_CIPHER_TYPE_GCMP:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP");
                break;
            case WIFI_CIPHER_TYPE_GCMP256:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_GCMP256");
                break;
            default:
                ESP_LOGI(TAG, "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
                break;
            }

            switch (group_cipher) {
            case WIFI_CIPHER_TYPE_NONE:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
                break;
            case WIFI_CIPHER_TYPE_WEP40:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
                break;
            case WIFI_CIPHER_TYPE_WEP104:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
                break;
            case WIFI_CIPHER_TYPE_TKIP:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
                break;
            case WIFI_CIPHER_TYPE_CCMP:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
                break;
            case WIFI_CIPHER_TYPE_TKIP_CCMP:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
                break;
            case WIFI_CIPHER_TYPE_SMS4:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_SMS4");
                break;
            case WIFI_CIPHER_TYPE_GCMP:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP");
                break;
            case WIFI_CIPHER_TYPE_GCMP256:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_GCMP256");
                break;
            default:
                ESP_LOGI(TAG, "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
                break;
            }
        }


        uint8_t scan(std::string& result)
        {
            // Initialize NVS
            // nvs_init();

            ESP_LOGI("nvs", "init");
            is_nvs_inited = true;

            // Initialize NVS
            esp_err_t ret = nvs_flash_init();
            if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
            {
                ESP_ERROR_CHECK(nvs_flash_erase());
                ret = nvs_flash_init();
            }
            ESP_ERROR_CHECK( ret );



            

            
            ESP_ERROR_CHECK(esp_netif_init());
            ESP_ERROR_CHECK(esp_event_loop_create_default());
            esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
            assert(sta_netif);

            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));

            uint16_t number = DEFAULT_SCAN_LIST_SIZE;
            wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
            uint16_t ap_count = 0;
            memset(ap_info, 0, sizeof(ap_info));

            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());
            esp_wifi_scan_start(NULL, true);
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
            ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);


            uint8_t wifi_num = ap_count;
            result = "";

            for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
            {
                ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
                ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
                print_auth_mode(ap_info[i].authmode);
                if (ap_info[i].authmode != WIFI_AUTH_WEP)
                {
                    print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
                }
                ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);



                result += std::to_string(i);
                result += ". (";
                result += std::to_string(ap_info[i].rssi);
                result += ") ";
                result += (char*)ap_info[i].ssid;
                result += "\n";

            }


            // esp_wifi_scan_stop();
            // esp_wifi_disconnect();



            esp_wifi_clear_ap_list();
            esp_wifi_clear_default_wifi_driver_and_handlers(sta_netif);



            esp_wifi_stop();
            // vTaskDelay(pdMS_TO_TICKS(3000));
            esp_wifi_deinit();

            


            // esp_netif_create_default_wifi_sta
            esp_netif_destroy_default_wifi(sta_netif);
            esp_netif_deinit();
            esp_event_loop_delete_default();






            ESP_ERROR_CHECK(nvs_flash_deinit());





            return wifi_num;
        }


        void scan(std::vector<ScanResult_t>& scanResult)
        {
            // Initialize NVS
            nvs_init();

            // ESP_LOGI("nvs", "init");
            // is_nvs_inited = true;

            // // Initialize NVS
            // esp_err_t ret = nvs_flash_init();
            // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
            // {
            //     ESP_ERROR_CHECK(nvs_flash_erase());
            //     ret = nvs_flash_init();
            // }
            // ESP_ERROR_CHECK( ret );


            
            ESP_ERROR_CHECK(esp_netif_init());

            // ESP_ERROR_CHECK(esp_event_loop_create_default());
            esp_event_loop_create_default();

            esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
            assert(sta_netif);

            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));

            uint16_t number = DEFAULT_SCAN_LIST_SIZE;
            wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
            uint16_t ap_count = 0;
            memset(ap_info, 0, sizeof(ap_info));

            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());
            esp_wifi_scan_start(NULL, true);
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
            ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);


            /* Clear result list */
            std::vector<ScanResult_t>().swap(scanResult);


            for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
            {
                ESP_LOGI(TAG, "SSID \t\t%s", ap_info[i].ssid);
                ESP_LOGI(TAG, "RSSI \t\t%d", ap_info[i].rssi);
                print_auth_mode(ap_info[i].authmode);
                if (ap_info[i].authmode != WIFI_AUTH_WEP)
                {
                    print_cipher_type(ap_info[i].pairwise_cipher, ap_info[i].group_cipher);
                }
                ESP_LOGI(TAG, "Channel \t\t%d\n", ap_info[i].primary);


                ScanResult_t new_result;
                new_result.ssid = (char*)ap_info[i].ssid;
                new_result.rssi = ap_info[i].rssi;
                
                /* Push data into list */
                scanResult.push_back(new_result);
            }


            // esp_wifi_scan_stop();
            // esp_wifi_disconnect();


            esp_wifi_clear_ap_list();
            esp_wifi_clear_default_wifi_driver_and_handlers(sta_netif);


            


            esp_wifi_stop();
            vTaskDelay(pdMS_TO_TICKS(3000));


            esp_wifi_deinit();


            // esp_netif_create_default_wifi_sta
            esp_netif_destroy_default_wifi(sta_netif);
            esp_netif_deinit();
            esp_event_loop_delete_default();







            // ESP_ERROR_CHECK(nvs_flash_deinit());
        }


    }





}



