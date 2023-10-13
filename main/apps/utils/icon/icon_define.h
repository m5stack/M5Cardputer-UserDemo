/**
 * @file icon_define.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <stdint.h>

// App icon interface 
struct AppIcon_t 
{
    public:
        AppIcon_t(const uint16_t* iconBig, const uint16_t* iconSmall)
        {
            this->iconBig = iconBig;
            this->iconSmall = iconSmall;
        }

        const uint16_t* iconBig;
        const uint16_t* iconSmall;
};
