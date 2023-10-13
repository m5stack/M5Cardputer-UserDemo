/**
 * @file input_device_register.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "input_device.h"
#include <vector>


namespace MOONCAKE
{
    /* Input device register */
    /* This class contains a list of INPUT_DEVICE_BASE */
    /* You can install input device into the lise */
    /* Register will help you to init them, and keep their updating */
    class InputDevice_Register
    {
        private:
            std::vector<INPUT_DEVICE_BASE*> _input_device_list;

        public:
            /* Free all the input devices' memory */
            ~InputDevice_Register();

            /**
             * @brief Get a list of installed input device 
             * 
             * @return const std::vector<INPUT_DEVICE_BASE*>* 
             */
            inline const std::vector<INPUT_DEVICE_BASE*>* getInstalledDeviceList() { return &_input_device_list; }

            /**
             * @brief Get the total number of installed input device 
             * 
             * @return std::size_t 
             */
            inline std::size_t getInstalledDeviceNum() { return _input_device_list.size(); }

            /**
             * @brief Install an input device, it's init method will be called here 
             * 
             * @param inputDevice 
             * @param userData 
             * @return true 
             * @return false 
             */
            bool install(INPUT_DEVICE_BASE* inputDevice, void* userData = nullptr);

            /**
             * @brief Uninstall an input device
             * 
             * @param inputDevice 
             * @param freeMemory help you to free the memory or not 
             * @return true 
             * @return false 
             */
            bool uninstall(INPUT_DEVICE_BASE* inputDevice, bool freeMemory = true);

            /**
             * @brief Uninstall all input devices
             * 
             * @param freeMemory help you to free the memory or not 
             */
            void uninstallAllDevice(bool freeMemory = true);

            /**
             * @brief Check if input device is installed
             * 
             * @param inputDevice 
             * @return true 
             * @return false 
             */
            bool isDeviceInstalled(INPUT_DEVICE_BASE* inputDevice);

            /**
             * @brief Call this to init all installed input device
             * 
             */
            void init();

            /**
             * @brief Call this to update all installed input device
             * 
             */
            void update();
    };
}
