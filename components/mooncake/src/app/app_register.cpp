/**
 * @file app_register.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.2
 * @date 2023-08-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_register.h"


using namespace MOONCAKE;


APP_Register::~APP_Register()
{
    /* Free all the app packers' memory */
    uninstallAllApps();
}


bool APP_Register::install(APP_PACKER_BASE* appPacker, void* userData)
{
    if (appPacker == nullptr)
        return false;
    
    if (isAppInstalled(appPacker))
        return false;

    /* Copy user data */
    appPacker->setUserData(userData);

    /* Push into list */
    _app_packer_list.push_back(appPacker);

    return true;
}


bool APP_Register::uninstall(APP_PACKER_BASE* appPacker, bool freeMemory)
{
    if (appPacker == nullptr)
        return false;

    /* Iterate the shit out */
    for (auto iter = _app_packer_list.begin(); iter != _app_packer_list.end(); iter++)
    {
        if (*iter == appPacker)
        {
            /* Free parker's memory */
            if (freeMemory)
                delete (*iter);

            /* Remove it from the list */
            _app_packer_list.erase(iter);
            return true;
        }
    }
    return false;
}


void APP_Register::uninstallAllApps(bool freeMemory)
{
    /* Free all the packers' memory */
    if (freeMemory)
    {
        for (const auto& i : _app_packer_list)
            delete i;
    }
    
    /* Clear the list */
    _app_packer_list.clear();
}


bool APP_Register::isAppInstalled(APP_PACKER_BASE* appPacker)
{
    if (appPacker == nullptr)
        return false;

    /* Iterate the shit out */
    for (const auto& i : _app_packer_list)
    {
        if (i == appPacker)
            return true;
    }
    return false;
}
