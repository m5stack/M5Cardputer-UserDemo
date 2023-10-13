/**
 * @file app_register_test.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-08-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <app/app_register.h>


using namespace MOONCAKE;


/* ------------------ App packer 1111 ------------------ */
class App_1111_packer : public APP_PACKER_BASE
{
    std::string getAppName() override { return "App-1111"; }
    public: App_1111_packer() { std::cout << "App-1111 packer constructor\n"; }
    public: ~App_1111_packer() { std::cout << "App-1111 packer destructor\n"; }
};
/* ----------------------------------------------------- */

/* ------------------ App packer 2222 ------------------ */
class App_2222_packer : public APP_PACKER_BASE
{
    std::string getAppName() override { return "App-2222"; }
    public: App_2222_packer() { std::cout << "App-2222 packer constructor\n"; }
    public: ~App_2222_packer() { std::cout << "App-2222 packer destructor\n"; }
};
/* ----------------------------------------------------- */


int main()
{
    std::cout << "[App register test]\n";

    /* Create a app register */
    APP_Register app_register;


    /* Check the num of installed app packers */
    std::cout << "Register now has " << app_register.getInstalledAppNum() << " app packers\n";
    std::cout << "\n";
    /*
        Register now has 0 app packers
    */

    
    /* Install app packers */
    app_register.install(new App_1111_packer);
    app_register.install(new App_2222_packer);
    std::cout << "\n";
    /*
        App-1111 packer constructor
        App-2222 packer constructor
    */
    

    /* Check the num of installed app packers */
    std::cout << "Register now has " << app_register.getInstalledAppNum() << " app packers\n";
    std::cout << "\n";
    /*
        Register now has 2 app packers
    */


    /* APP_Register's destruction will help you to free all app packers */
    std::cout << "\n";
    return 0;
    /*
        App-1111 packer destructor
        App-2222 packer destructor
    */
}
