/**
 * @file esp_now_wrap.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

void espnow_wrap_init();

int espnow_wrap_send(uint8_t* message, size_t size);

int espnow_wrap_available();

uint8_t* espnow_wrap_get_received();

void espnow_wrap_clear();

uint8_t espnow_wrap_is_inited();


#ifdef __cplusplus
}
#endif
