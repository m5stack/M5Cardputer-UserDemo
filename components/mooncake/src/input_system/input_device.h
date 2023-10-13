/**
 * @file input_device.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <string>


namespace MOONCAKE
{   
    /* This is a enum to indentify the tpye of the input device */
    enum InputDeviceType_t
    {
        Input_Custom = 0,
        Input_Button,
        Input_TouchPad,
        Input_Encoder,
        Input_Mouse,
        Input_Keyboard
    };

    /* Input device base */
    /* This is a base class for all kinds of input device */
    class INPUT_DEVICE_BASE
    {
        private:
            /* Device type */
            InputDeviceType_t _device_type;

            /* Device name */
            std::string _device_name;

            /* If you want to call the update whenever you want, instead framework do it automatically */
            bool _update_manually;

            /* User data */
            void* _user_data;


        protected:
            /* Can only be set internally */
            inline void setDeviceType(const InputDeviceType_t& deviceType) { _device_type = deviceType; }
            

        public:
            /* Virtual it for subclass's memory free */
            virtual ~INPUT_DEVICE_BASE() {}

            INPUT_DEVICE_BASE() :
                _device_type(Input_Custom),
                _update_manually(false),
                _user_data(nullptr)
                {}

            /* Basic data getter and setter */
            inline INPUT_DEVICE_BASE* getAddr() { return this; }
            inline void setDeviceName(const std::string& deviceName) { _device_name = deviceName; }
            inline const std::string& getDeviceName() { return _device_name; }
            inline const InputDeviceType_t& getDeviceType() { return _device_type; }
            inline void setUserData(void* userData) { _user_data = userData; }
            inline void* getUserData() { return _user_data; }
            inline void setUpdateManually(bool updateManually) { _update_manually = updateManually; }
            inline bool isUpdateManually() { return _update_manually; }


            /**
             * @brief Override this to init your input device
             * ,device name and type should be set here
             */
            virtual void init() {}
            
            /**
             * @brief Override this to update your input device's data 
             * , this will kept being called 
             */
            virtual void update() {}
    };
}
