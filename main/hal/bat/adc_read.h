/**
 * @file adc_read.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-10-12
 * https://github.com/espressif/esp-idf/blob/v4.4.6/examples/peripherals/adc/single_read/single_read/main/single_read.c
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


void adc_read_init();
uint32_t adc_read_get_value();


#ifdef __cplusplus
}
#endif
