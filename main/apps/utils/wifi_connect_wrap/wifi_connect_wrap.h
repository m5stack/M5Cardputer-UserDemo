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


void wifi_connect_wrap_config(const char* ssid, const char* password);
void wifi_connect_wrap_connect();
void wifi_connect_wrap_disconnect();


#ifdef __cplusplus
}
#endif