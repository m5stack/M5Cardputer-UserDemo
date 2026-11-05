/**
 * @file sntp_wrap.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-10-18
 * https://github.com/espressif/esp-idf/blob/v4.4.6/examples/protocols/sntp/main/sntp_example_main.c
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif


// Returns true only after a fresh SNTP response has updated the system clock.
bool sntp_warp_init(void);


#ifdef __cplusplus
}
#endif
