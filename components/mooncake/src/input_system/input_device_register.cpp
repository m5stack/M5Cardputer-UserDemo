/**
 * @file input_device_register.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "input_device_register.h"


using namespace MOONCAKE;


InputDevice_Register::~InputDevice_Register()
{
    /* Free all the input devices' memory */
    uninstallAllDevice();
}


bool InputDevice_Register::install(INPUT_DEVICE_BASE* inputDevice, void* userData)
{
    if (inputDevice == nullptr)
        return false;
    
    if (isDeviceInstalled(inputDevice))
        return false;

    /* Copy user data */
    inputDevice->setUserData(userData);

    /* Call input device's init */
    inputDevice->init();

    /* Push into list */
    _input_device_list.push_back(inputDevice);

    return true;
}


bool InputDevice_Register::uninstall(INPUT_DEVICE_BASE* inputDevice, bool freeMemory)
{
    if (inputDevice == nullptr)
        return false;
    
    /* Iterate the shit out */
    for (auto iter = _input_device_list.begin(); iter != _input_device_list.end(); iter++)
    {
        if (*iter == inputDevice)
        {
            /* Free input device's memory */
            if (freeMemory)
                delete (*iter);
            
            /* Remove it from the list */
            _input_device_list.erase(iter);
            return true;
        }
    }
    return false;
}


void InputDevice_Register::uninstallAllDevice(bool freeMemory)
{
    /* Free all the input deives' memory */
    if (freeMemory)
    {
        for (auto i : _input_device_list)
            delete i;
    }
    
    /* Clear the list */
    _input_device_list.clear();
}


bool InputDevice_Register::isDeviceInstalled(INPUT_DEVICE_BASE* inputDevice)
{
    if (inputDevice == nullptr)
        return false;

    /* Iterate the shit out */
    for (const auto& i : _input_device_list)
    {
        if (i == inputDevice)
            return true;
    }
    return false;
}


void InputDevice_Register::init()
{
    /* Call every input devices' init() */
    for (const auto& i : _input_device_list)
        i->init();
}


void InputDevice_Register::update()
{
    /* Call every input devices' update() */
    for (const auto& i : _input_device_list)
    {
        /* Skip the manually guys */
        if (i->isUpdateManually())
            continue;
        i->update();
    }
}
