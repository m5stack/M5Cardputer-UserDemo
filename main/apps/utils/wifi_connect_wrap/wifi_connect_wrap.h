/**
 * @file wifi_connect_wrap.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-10-18
 * https://github.com/espressif/esp-idf/blob/v4.4.6/examples/protocols/sntp/main/sntp_example_main.c
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void wifi_connect_wrap_config(const char* ssid, const char* password);
void wifi_connect_get_config();
void wifi_connect_wrap_connect();
void wifi_connect_wrap_disconnect();
uint8_t wifi_connect_wrap_is_wifi_connect_success();


#ifdef __cplusplus
}
#endif