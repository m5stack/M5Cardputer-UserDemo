/**
 * @file input_device_register_test.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <iostream>
#include <input_system/input_device_register.h>


using namespace MOONCAKE;


/* ---------------------- Input device 1111 ---------------------- */
class InDev1111 : public INPUT_DEVICE_BASE
{
    void init() override
    {
        setDeviceName("InDev-1111");
        setDeviceType(Input_Button);

        std::cout << getDeviceName() << " > init\n";
    }
    void update() override
    {
        std::cout << getDeviceName() << " > aaaa\n";
    }
    public: InDev1111() { std::cout << "InDev-1111 constructor\n"; }
    public: ~InDev1111() { std::cout << getDeviceName() << " destructor\n"; }
};
/* --------------------------------------------------------------- */

/* ---------------------- Input device 2222 ---------------------- */
class InDev2222 : public INPUT_DEVICE_BASE
{
    void init() override
    {
        setDeviceName("InDev-2222");
        setDeviceType(Input_Button);

        std::cout << getDeviceName() << " > init\n";
    }
    void update() override
    {
        std::cout << getDeviceName() << " > bbbb\n";
    }
    public: InDev2222() { std::cout << "InDev-2222 constructor\n"; }
    public: ~InDev2222() { std::cout << getDeviceName() << " destructor\n"; }
};
/* --------------------------------------------------------------- */





int main()
{
    std::cout << "[Input device register test]\n";

    /* Create a register */
    InputDevice_Register indev_register;


    /* Check the num of installed device */
    std::cout << "Register now has " << indev_register.getInstalledDeviceNum() << " devices\n";
    std::cout << "\n";
    /*
        Register now has 0 devices
    */


    /* Install devices */
    indev_register.install(new InDev1111);
    indev_register.install(new InDev2222);
    std::cout << "\n";
    /*
        InDev-1111 constructor
        InDev-2222 constructor
    */

    
    /* Check the num of installed device */
    std::cout << "Register now has " << indev_register.getInstalledDeviceNum() << " devices\n";
    std::cout << "\n";
    /*
        Register now has 2 devices
    */
    

    /* Init all the input devices */
    indev_register.init();
    std::cout << "\n";
    /*
        InDev-1111 > init
        InDev-2222 > init
    */
    

    /* Update all the input devices */
    for (int i = 0; i < 2; i++)
        indev_register.update();
    /*
        InDev-1111 > aaaa
        InDev-2222 > bbbb
        InDev-1111 > aaaa
        InDev-2222 > bbbb
    */
    

    /* InputDevice_Register's destruction will help you to free all devices */
    std::cout << "\n";
    return 0;
    /*
        InDev-1111 destructor
        InDev-2222 destructor
    */
}
