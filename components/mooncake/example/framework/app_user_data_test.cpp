/**
 * @file app_user_data_test.cpp
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
    void onCreate() override { startApp(); }
    void onRunning() override
    {
        /* Raw getting */
        // std::cout << getUserData() << "\n";
        // std::cout << ((Mooncake*)(((MC_USER_DATA_T)getUserData())->framework))->getInstalledAppList()[0]->getAppName() << "\n";
        // std::cout << ((SIMPLEKV::SimpleKV*)(((MC_USER_DATA_T)getUserData())->database))->Get(MC_DB_DISP_HOR)->value<int>() << "\n";
        
        /* Frame work define wrap */
        std::cout << mcAppGetFramework()->getInstalledAppList()[0]->getAppName() << "\n";
        std::cout << mcAppGetDatabase()->Get(MC_DB_DISP_HOR)->value<int>() << ", ";
        std::cout << mcAppGetDatabase()->Get(MC_DB_DISP_VER)->value<int>() << "\n";
    }
};
/* Create a new app packer with new resource and memory manager */
class App_1111_packer : public APP_PACKER_BASE
{
    public:
    std::string getAppName() override { return "App-1111"; }
    void * newApp() override { return new App_1111; }
    void deleteApp(void *app) override { delete (App_1111*)app; }
};
/* ------------------------------------------------------- */


int main()
{
    std::cout << "[App user data test]\n";

    Mooncake mooncake;
    mooncake.init();
    
    mooncake.installApp(new App_1111_packer);
    mooncake.createApp(mooncake.getInstalledAppList()[0]);

    for (int i = 0; i < 6; i++)
        mooncake.update();


    return 0;
}
