/**
 * @file app_manager.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.2
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "app_manager.h"


using namespace MOONCAKE;


APP_Manager::~APP_Manager()
{
    /* Free all the app's memory */
    destroyAllApps();
}


APP_BASE* APP_Manager::createApp(APP_PACKER_BASE* appPacker)
{
    if (appPacker == nullptr)
        return nullptr;

    /* Create a new app with app packer */
    APP_BASE* new_app = (APP_BASE*)appPacker->newApp();
    if (new_app == nullptr)
        return nullptr;

    /* Pass the app packer to the new app */
    new_app->setAppPacker(appPacker);

    /* Call app's onCreate method */
    new_app->onCreate();

    /* Create a new lifecycle container */
    AppLifecycle_t new_lifecycle;
    new_lifecycle.app = new_app;
    new_lifecycle.state = ON_CREATE;

    /* Push into lifecycle list */
    _app_create_buffer.push_back(new_lifecycle);

    /* Return the app pointer for further mangement */
    return new_app;
}


int APP_Manager::_search_app_lifecycle_list(APP_BASE* app)
{
    if (app == nullptr)
        return -1;

    for (int i = 0; i < _app_lifecycle_list.size(); i++)
    {
        if (_app_lifecycle_list[i].app == app)
            return i;
    }
    return -1;
}


int APP_Manager::_search_app_create_buffer(APP_BASE* app)
{
    if (app == nullptr)
        return -1;

    for (int i = 0; i < _app_create_buffer.size(); i++)
    {
        if (_app_create_buffer[i].app == app)
            return i;
    }
    return -1;
}


bool APP_Manager::startApp(APP_BASE* app)
{
    // If not pushed into lifecycle yet 
    // Like call createApp() and then startApp() inside an app
    int index = _search_app_create_buffer(app);
    if (index >= 0)
    {
        _app_create_buffer[index].state = ON_RESUME;
        return true;
    }

    index = _search_app_lifecycle_list(app);
    if (index < 0)
        return false;

    /* Update state */
    switch (_app_lifecycle_list[index].state)
    {
        case ON_CREATE:
            _app_lifecycle_list[index].state = ON_RESUME;
            break;
        case ON_RESUME:
            /* Do nothing */
            break;
        case ON_RUNNING:
            /* Do nothing */
            break;
        case ON_RUNNING_BG:
            _app_lifecycle_list[index].state = ON_RESUME;
            break;
        case ON_PAUSE:
            _app_lifecycle_list[index].state = ON_RESUME;
            break;
        case ON_DESTROY:
            /* Not gonna happen */
            break;
        default:
            break;
    }

    return true;
}


bool APP_Manager::closeApp(APP_BASE* app)
{
    // If not pushed into lifecycle yet 
    // Like call createApp() and then closeApp() inside an app
    int index = _search_app_create_buffer(app);
    if (index >= 0)
    {
        _app_create_buffer[index].state = ON_PAUSE;
        return true;
    }

    index = _search_app_lifecycle_list(app);
    if (index < 0)
        return false;
    
    /* Update state */
    switch (_app_lifecycle_list[index].state)
    {
        case ON_CREATE:
            /* Do nothing */
            break;
        case ON_RESUME:
            _app_lifecycle_list[index].state = ON_PAUSE;
            break;
        case ON_RUNNING:
            _app_lifecycle_list[index].state = ON_PAUSE;
            break;
        case ON_RUNNING_BG:
            /* Do nothing */
            break;
        case ON_PAUSE:
            /* Do nothing */
            break;
        case ON_DESTROY:
            /* Not gonna happen */
            break;
        default:
            break;
    }

    return true;
}


void APP_Manager::update()
{
    /* Iterate the shit out */
    for (auto iter = _app_lifecycle_list.begin(); iter != _app_lifecycle_list.end();)
    {
        /* If app wants to be started */
        if (iter->app->isGoingStart())
        {
            /* Reset flag */
            iter->app->resetGoingStartFlag();

            /* Update state */
            iter->state = ON_RESUME;
        }

        /* If app wants to be closed */
        if (iter->app->isGoingClose())
        {
            /* Reset flag */
            iter->app->resetGoingCloseFlag();

            /* Update state */
            if (iter->app->isAllowBgRunning())
                iter->state = ON_PAUSE;
            else
                iter->state = ON_DESTROY;
        }

        /* If app wants to be destroyed */
        if (iter->app->isGoingDestroy())
        {
            /* Reset flag */
            iter->app->resetGoingDestroyFlag();

            /* Update state */
            iter->state = ON_DESTROY;
        }


        /* Lifecycle FSM */
        switch (iter->state)
        {
            case ON_CREATE:
                /* Do nothing */
                break;
            case ON_RESUME:
                iter->app->onResume();
                iter->state = ON_RUNNING;
                break;
            case ON_RUNNING:
                iter->app->onRunning();
                break;
            case ON_RUNNING_BG:
                iter->app->onRunningBG();
                break;
            case ON_PAUSE:
                iter->app->onPause();
                iter->state = ON_RUNNING_BG;
                break;
            case ON_DESTROY:
                /* Same as destroyApp() */
                iter->app->onPause();
                iter->app->onDestroy();
                iter->app->getAppPacker()->deleteApp(iter->app);
                /* Remove and update iterator */
                iter = _app_lifecycle_list.erase(iter);
                continue;
            default:
                break;
        }

        /* Next */
        iter++;
    }

    /* Push created apps buffer into lifecycle list */
    if (_app_create_buffer.size() != 0)
    {
        for (const auto& app : _app_create_buffer)
        {
            _app_lifecycle_list.push_back(app);
        }
        _app_create_buffer.clear();
    }
}


bool APP_Manager::destroyApp(APP_BASE* app)
{
    if (app == nullptr)
        return false;

    /* If not push into lifecycle list yet */
    for (auto iter = _app_create_buffer.begin(); iter != _app_create_buffer.end(); iter++)
    {
        if (iter->app == app)
        {
            _app_create_buffer.erase(iter);
            return true;
        }
    }

    /* Iterate the shit out */
    for (auto iter = _app_lifecycle_list.begin(); iter != _app_lifecycle_list.end(); iter++)
    {
        if (iter->app == app)
        {
            /* Call app's onPause method */
            iter->app->onPause();

            /* Call app's onDestroy method */
            iter->app->onDestroy();

            /* Delete this app by it's app packer */
            iter->app->getAppPacker()->deleteApp(iter->app);

            /* Remove it from the lifecycle list */
            _app_lifecycle_list.erase(iter);

            return true;
        }
    }

    return false;
}


void APP_Manager::destroyAllApps()
{
    /* Iterate the shit out */
    for (auto& i : _app_lifecycle_list)
    {
        /* Call app's onPause method */
        i.app->onPause();

        /* Call app's onDestroy method */
        i.app->onDestroy();

        /* Delete this app by it's app packer */
        i.app->getAppPacker()->deleteApp(i.app);
    }

    _app_lifecycle_list.clear();
    _app_create_buffer.clear();
}
