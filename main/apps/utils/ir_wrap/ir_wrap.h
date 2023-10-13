/**
 * @file ir_wrap.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void ir_wrap_init(void);
void ir_wrap_deinit(void);
void ir_wrap_send(uint8_t addr, uint8_t cmd);


#ifdef __cplusplus
}
#endif
