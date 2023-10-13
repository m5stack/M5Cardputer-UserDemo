/**
 * @file mooncake_framework_test.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <iostream>
#include <mooncake.h>


using namespace MOONCAKE;


/* ---------------------- App_1111 ---------------------- */
/* Create a new app with new lifecycle methods */
class App_1111 : public APP_BASE
{
    void onCreate() override { startApp(); std::cout << getAppName() << " > onCreate\n"; }
    void onResume() override { std::cout << getAppName() << " > onResume\n"; }
    void onRunning() override { std::cout << getAppName() << " > onRunning\n"; }
    public: App_1111() { std::cout << "App-1111 constructor\n"; }
    public: ~App_1111() { std::cout << "App-1111 destructor\n"; }
};
/* Create a new app packer with new resource and memory manager */
class App_1111_packer : public APP_PACKER_BASE
{
    public:
    std::string getAppName() override { return "App-1111"; }
    void * newApp() override { return new App_1111; }
    void deleteApp(void *app) override { delete (App_1111*)app; }
    public: App_1111_packer() { std::cout << "App-1111 packer constructor\n"; }
    public: ~App_1111_packer() { std::cout << "App-1111 packer destructor\n"; }
};
/* --------------------------------------------------- */


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
        std::cout << getDeviceName() << " > update\n";
    }
    public: InDev1111() { std::cout << "InDev-1111 constructor\n"; }
    public: ~InDev1111() { std::cout << getDeviceName() << " destructor\n"; }
};
/* --------------------------------------------------------------- */


int main()
{
    /* Create mooncake */
    Mooncake mooncake;


    /* Init framework */
    mooncake.init();
    std::cout << "\n";

    
    /* Install an app */
    mooncake.installApp(new App_1111_packer);
    std::cout << "\n";
    /*
        App-1111 packer constructor
    */

    
    /* Create an App-1111 */
    /* Since we just installed one app packer, who is first one in the list */
    mooncake.createApp(mooncake.getInstalledAppList()[0]);
    std::cout << "\n";
    /*
        App-1111 constructor
        App-1111 > onCreate
    */


    /* Install a input device */
    mooncake.installIndev(new InDev1111);
    std::cout << "\n";
    /*
        InDev-1111 constructor
        InDev-1111 > init
    */


    /* Call update to keep framework running */
    for (int i = 0; i < 2; i++)
        mooncake.update();
    std::cout << "\n";
    /*
        InDev-1111 > update
        App-1111 > onResume
        InDev-1111 > update
        App-1111 > onRunning
    */
   

    /* *App should always created in heap* */
    /* Because framework will try to free them during the destruction */
    /* Create in stack like this will cause error when mooncake reach the end of it's lifetime */

    
    /* Mooncake's destruction will help you to free all the apps and app packers */
    return 0;
    /*
        App-1111 destructor
        App-1111 packer destructor
        InDev-1111 destructor
    */
}
