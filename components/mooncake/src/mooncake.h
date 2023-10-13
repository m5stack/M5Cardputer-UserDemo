/**
 * @file mooncake.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.2
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "app/app.h"
#include "app/app_register.h"
#include "app/app_manager.h"
#include "input_system/input_device_register.h"
#include "simplekv/simplekv.h"
#include "mc_conf_internal.h"

/* Disable some features */
#ifdef ESP_PLATFORM
#define SPDLOG_NO_EXCEPTIONS
#define SPDLOG_NO_THREAD_ID
#endif
#include "spdlog/include/spdlog/spdlog.h"


namespace MOONCAKE
{
    class Mooncake;

    /* Structure that contains userdata */
    /* Will be passed to every running apps */
    /* You can inherit this to create your own userdata */
    struct APP_UserData_t
    {
        /* Pointer to the framwork */
        Mooncake* framework = nullptr;

        /* Pointer to the database */
        SIMPLEKV::SimpleKV* database = nullptr;
    };


    /* Mooncake framework class */
    /* Inherit App Register to manager apps' install and uninstall */
    /* Contains a App Manager to control apps' lifecycles */
    class Mooncake
    {
        private:
            /* Component Input system */
            InputDevice_Register _input_device_register;

            /* Component App register */
            APP_Register _app_register;

            /* Component App manager */
            APP_Manager _app_manager;

            /* Component Database */
            SIMPLEKV::SimpleKV _database;

            /* User data pointer */
            APP_UserData_t* _user_data;

            /* Boot anim pointer */
            APP_PACKER_BASE* _boot_anim;
            void (*_database_setup_callback)(SIMPLEKV::SimpleKV&);

            /* Flag to free the memory, if they are created by framework */
            bool _flag_free_user_data;
            bool _flag_free_boot_anim;

            void _data_base_setup_internal();
            
        
        public:
            Mooncake() :
                _user_data(nullptr),
                _boot_anim(nullptr),
                _database_setup_callback(nullptr),
                _flag_free_user_data(false),
                _flag_free_boot_anim(false)
                {}
            ~Mooncake();

            /* Framework's components getter */
            inline InputDevice_Register& getInputDeviceRegister() { return _input_device_register; }
            inline APP_Register& getAppRegister() { return _app_register; }
            inline APP_Manager& getAppManager() { return _app_manager; }
            inline SIMPLEKV::SimpleKV& getDatabase() { return _database; }

            /**
             * @brief Set the user data, which will be passed to every apps
             * , if not set, framework will create a default one 
             * @param userData 
             */
            inline void setUserData(APP_UserData_t* userData) { _user_data = userData; }

            /**
             * @brief Get the User Data object
             * 
             * @return APP_UserData_t* 
             */
            inline APP_UserData_t* getUserData() { return _user_data; }

            /**
             * @brief Set the boot anim, which will be created and run in framework's init()
             * , if not set, framework will create a default one 
             * @param bootAnim 
             */
            inline void setBootAnim(APP_PACKER_BASE* bootAnim) { _boot_anim = bootAnim; }

            /**
             * @brief Set the Database Setup Callback
             * , the call back will be called after some basic data were set, you can add some custom data into database in the callback
             * @param databaseSetupCallback 
             */
            inline void setDatabaseSetupCallback(void (*databaseSetupCallback)(SIMPLEKV::SimpleKV&)) { _database_setup_callback = databaseSetupCallback; }

            /**
             * @brief Init framework
             * 
             */
            void init();

            /**
             * @brief Calling this to keep framework running 
             * 
             */
            void update();


            /* Framework wrap to the App register */

            /* *Important*: this wrap will pass user data pointer to the app packer */
            
            /**
             * @brief Install an app (Register an app packer)
             * 
             * @param appPacker 
             * @return true 
             * @return false 
             */
            inline bool installApp(APP_PACKER_BASE* appPacker) { return _app_register.install(appPacker, _user_data); }
            
            /**
             * @brief Uninstall an app (Remove it from the register)
             * 
             * @param appPacker 
             * @return true 
             * @return false 
             */
            inline bool uninstallApp(APP_PACKER_BASE* appPacker) { return _app_register.uninstall(appPacker); }

            /**
             * @brief Uninstall all apps 
             * 
             */
            inline void uninstallAllApps() { _app_register.uninstallAllApps(); }
            
            /**
             * @brief Get the total installed app num in app register 
             * 
             * @return std::size_t 
             */
            inline std::size_t getInstalledAppNum() { return _app_register.getInstalledAppNum(); }

            /**
             * @brief Get the reference to the installed app packer list
             * 
             * @return std::vector<APP_PACKER_BASE*>& 
             */
            inline std::vector<APP_PACKER_BASE*>& getInstalledAppList() { return _app_register.getInstalledAppList(); }


            /* Framework wrap to the App manager */

            /**
             * @brief Create an app
             * 
             * @param appPacker app's packer
             * @return APP_BASE* started app's pointer
             */
            inline APP_BASE* createApp(APP_PACKER_BASE* appPacker) { return _app_manager.createApp(appPacker); }
            
            /**
             * @brief Start an app, this method will only change passing app's state
             * 
             * @param app 
             * @return true 
             * @return false 
             */
            inline bool startApp(APP_BASE* app) { return _app_manager.startApp(app); }
            
            /**
             * @brief Close an app, this method will only change passing app's state
             * 
             * @param app App's pointer which you want to close 
             * @return true 
             * @return false 
             */
            inline bool closeApp(APP_BASE* app) { return _app_manager.closeApp(app); }
            
            /**
             * @brief Destroy an app, app will be deleted by it's app packer
             * 
             * @param app App's pointer which you want to destroy 
             * @return true 
             * @return false 
             */
            inline bool destroyApp(APP_BASE* app) { return _app_manager.destroyApp(app); }
            
            /**
             * @brief Destroy all apps 
             * 
             */
            inline void destroyAllApps() { _app_manager.destroyAllApps(); }
            
            /**
             * @brief Get total created app num in app manager
             * 
             * @return std::size_t 
             */
            inline std::size_t getCreatedAppNum() { return _app_manager.getCreatedAppNum(); }


            /* Framework wrap to the Input device register */

            /**
             * @brief Install an input device, it's init method will be called here 
             * 
             * @param inputDevice 
             * @param userData 
             * @return true 
             * @return false 
             */
            inline bool installIndev(INPUT_DEVICE_BASE* inputDevice, void* userData = nullptr) { return _input_device_register.install(inputDevice, userData); }

            /**
             * @brief Uninstall an input device
             * 
             * @param inputDevice 
             * @return true 
             * @return false 
             */
            inline bool uninstallIndev(INPUT_DEVICE_BASE* inputDevice) { return _input_device_register.uninstall(inputDevice); }

            /**
             * @brief Uninstall all input devices
             * 
             */
            inline void uninstallAllIndev() { _input_device_register.uninstallAllDevice(); }

            /**
             * @brief Get the total number of installed input device 
             * 
             * @return std::size_t 
             */
            inline std::size_t getInstalledIndevNum() { return _input_device_register.getInstalledDeviceNum(); }


    };
}
