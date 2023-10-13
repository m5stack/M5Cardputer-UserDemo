/**
 * @file boot_anim_ascii.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "../../../src/mooncake.h"
#include <cstdio>
#include <iostream>
#include <string>


namespace MOONCAKE
{
    namespace APPS
    {
        class BootAnim_ASCII : public APP_BASE
        {
            private:
                static const std::string _mooncake_ascii_logo;

            public:
                static const std::string& getMooncakeASCIILogo() { return _mooncake_ascii_logo; }
                static void printInfos()
                {
                    /* Print logo */
                    std::cout << BootAnim_ASCII::getMooncakeASCIILogo();
                    std::cout << "\n- @author Forairaaaaa\n";
                    std::cout << "- @version " << MC_VERSION << "\n";
                    std::cout << "- @build at " << __TIME__ << " " << __DATE__ << "\n\n";
                }

                void onCreate() override
                {
                    /* Print Info */
                    BootAnim_ASCII::printInfos();

                    /* Go destroy */
                    destroyApp();
                }
        };

        class BootAnim_ASCII_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "BootAnim_ASCII"; }
            void* newApp() override { return new BootAnim_ASCII; }
            void deleteApp(void *app) override { delete (BootAnim_ASCII*)app; }
        };
    }
}
