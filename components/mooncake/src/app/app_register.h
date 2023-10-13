/**
 * @file app_register.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.2
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "app.h"
#include <iostream>
#include <vector>


namespace MOONCAKE
{
    /* App register */
    /* This class contains a list of "APP_PACKER" */
    /* Which can tells what apps do you have */
    /* "install" will add a packer into the list, "uninstall" will remove it */
    class APP_Register
    {
        private:
            std::vector<APP_PACKER_BASE*> _app_packer_list;

        public:
            /* Free all the app packers' memory */
            ~APP_Register();

            /**
             * @brief Get the reference to the installed app packer list
             * 
             * @return std::vector<APP_PACKER_BASE*>& 
             */
            inline std::vector<APP_PACKER_BASE*>& getInstalledAppList() { return _app_packer_list; }

            /**
             * @brief Get the total number of the installed apps 
             * 
             * @return std::size_t 
             */
            inline std::size_t getInstalledAppNum() { return _app_packer_list.size(); }

            /**
             * @brief Install an app (Register an app packer)
             * 
             * @param appPacker 
             * @param database 
             * @param userData 
             * @return true 
             * @return false 
             */
            bool install(APP_PACKER_BASE* appPacker, void* userData = nullptr);

            /**
             * @brief Uninstall an app (Remove it from the register)
             * 
             * @param appPacker 
             * @param freeMemory help you to free the memory or not 
             * @return true 
             * @return false 
             */
            bool uninstall(APP_PACKER_BASE* appPacker, bool freeMemory = true);

            /**
             * @brief Uninstall all apps 
             * 
             * @param freeMemory help you to free the memory or not 
             */
            void uninstallAllApps(bool freeMemory = true);
            
            /**
             * @brief Check if app is installed
             * 
             * @param appPacker 
             * @return true 
             * @return false 
             */
            bool isAppInstalled(APP_PACKER_BASE* appPacker);
    };
}
