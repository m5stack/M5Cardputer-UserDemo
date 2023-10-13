/**
 * @file app_manager.h
 * @author Forairaaaaa
 * @brief 
 * @version 0.2
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "app.h"
#include <cstddef>
#include <iostream>
#include <vector>


namespace MOONCAKE
{
    /* App manager */
    /* This class control app's opening, running or closing */
    class APP_Manager
    {
        public:
            /* This enum describe an app's current status of lifecycle */
            enum AppLifecycleState_t
            {
                ON_CREATE = 0,
                ON_RESUME,
                ON_RUNNING,
                ON_RUNNING_BG,
                ON_PAUSE,
                ON_DESTROY
            };

            /* A app lifecycle container */
            /* Holds an app and it's lifecycle status */
            struct AppLifecycle_t
            {
                APP_BASE* app = nullptr;
                AppLifecycleState_t state = ON_CREATE;
            };


        private:
            /* A list of apps' lifecycle */
            std::vector<AppLifecycle_t> _app_lifecycle_list;
            int _search_app_lifecycle_list(APP_BASE* app);

            /* A buffer of apps that about to add in lifecycle */
            /* To avoid push_back method while lifecycle list's iterating */
            std::vector<AppLifecycle_t> _app_create_buffer;
            int _search_app_create_buffer(APP_BASE* app);


        public:
            /* Free all the app's memory */
            ~APP_Manager();

            /**
             * @brief Create an app
             * 
             * @param appPacker app's packer
             * @return APP_BASE* started app's pointer
             */
            APP_BASE* createApp(APP_PACKER_BASE* appPacker);

            /**
             * @brief Start an app, this method will only change passing app's state
             * 
             * @param app 
             * @return true 
             * @return false 
             */
            bool startApp(APP_BASE* app);

            /**
             * @brief Close an app, this method will only change passing app's state
             * 
             * @param app App's pointer which you want to close 
             * @return true 
             * @return false 
             */
            bool closeApp(APP_BASE* app);

            /**
             * @brief Update app manager's FSM, so the running apps' lifecycle methods can be called
             * 
             */
            void update();

            /**
             * @brief Destroy an app, app will be deleted by it's app packer
             * 
             * @param app App's pointer which you want to destroy 
             * @return true 
             * @return false 
             */
            bool destroyApp(APP_BASE* app);

            /**
             * @brief Destroy all apps 
             * 
             */
            void destroyAllApps();
            
            /**
             * @brief Get total created app num in app manager
             * 
             * @return std::size_t 
             */
            inline std::size_t getCreatedAppNum() { return _app_lifecycle_list.size(); }

            /**
             * @brief Get the managing app lifecycle list 
             * 
             * @return const std::vector<AppLifecycle_t>*
             */
            inline const std::vector<AppLifecycle_t>* getAppLifecycleList() { return &_app_lifecycle_list; }
    };      
}
