/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file main.cpp
/// @brief contains main() function
///
/// Created by zs on 2016/12/21.
/// this file contains contains main() function
///

#include <ilm_control.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <map>

#define TO_STRING(ENUM) #ENUM

void SignalHandler(int sig);
void ILMCallback(ilmObjectType object, t_ilm_uint id, t_ilm_bool created, void* user_data);

int main()
{
    std::cout << "surface watcher: in" << std::endl;
    signal(SIGTERM, SignalHandler);
    signal(SIGINT, SignalHandler);

    ilm_init();

    ilm_registerNotification(ILMCallback, nullptr);

    ilm_commitChanges();

    pause();

    std::cout << "exit" << std::endl;
    return ilm_destroy();
}

void SignalHandler(int sig)
{
    ;
}

static std::map<unsigned, std::string> bit2str = {
    {ILM_NOTIFICATION_VISIBILITY, "ILM_NOTIFICATION_VISIBILITY"},
    {ILM_NOTIFICATION_OPACITY, "ILM_NOTIFICATION_OPACITY"},
    {ILM_NOTIFICATION_ORIENTATION, "ILM_NOTIFICATION_ORIENTATION"},
    {ILM_NOTIFICATION_SOURCE_RECT, "ILM_NOTIFICATION_SOURCE_RECT"},
    {ILM_NOTIFICATION_DEST_RECT, "ILM_NOTIFICATION_DEST_RECT"},
    {ILM_NOTIFICATION_CONTENT_AVAILABLE, "ILM_NOTIFICATION_CONTENT_AVAILABLE"},
    {ILM_NOTIFICATION_CONTENT_REMOVED, "ILM_NOTIFICATION_CONTENT_REMOVED"},
    {ILM_NOTIFICATION_CONFIGURED, "ILM_NOTIFICATION_CONFIGURED"},
};
void SurfaceCallback(t_ilm_surface surface, struct ilmSurfaceProperties*, t_ilm_notification_mask mask)
{
    std::cout << "surface 0x" << std::hex << surface << " is changed!" << std::endl;
    if(mask&ILM_NOTIFICATION_VISIBILITY)
        std::cout << "mask = " << bit2str[ILM_NOTIFICATION_VISIBILITY] << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_OPACITY)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_VISIBILITY) << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_ORIENTATION)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_ORIENTATION) << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_SOURCE_RECT)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_SOURCE_RECT) << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_DEST_RECT)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_DEST_RECT) << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_CONTENT_AVAILABLE)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_CONTENT_AVAILABLE) << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_CONTENT_REMOVED)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_CONTENT_REMOVED) << " !" << std::endl;
    if(mask&ILM_NOTIFICATION_CONFIGURED)
        std::cout << "mask = " << TO_STRING(ILM_NOTIFICATION_CONFIGURED) << " !" << std::endl;
}

void ILMCallback(ilmObjectType object, t_ilm_uint id, t_ilm_bool created, void* user_data)
{
    if(object==ILM_SURFACE)
    {
        if(created)
        {
            std::cout << "surface 0x" << std::hex << id << " is created!" << std::endl;

            ilmSurfaceProperties surfaceProperties;
            ilm_getPropertiesOfSurface(id, &surfaceProperties);
            if(surfaceProperties.origSourceWidth==0 && surfaceProperties.origSourceHeight==0)
            {
                std::cout << "surface is not configured. register callback." << std::endl;
                ilm_surfaceAddNotification(id, SurfaceCallback);
            }
        }
        else
        {
            std::cout << "surface 0x" << std::hex << id << " is destroyed!" << std::endl;
        }
    }

    if(object==ILM_LAYER)
    {
        if(created)
        {
            std::cout << "layer 0x" << std::hex << id << " is created!" << std::endl;

            ilmLayerProperties layerProperties;
            ilm_getPropertiesOfLayer(id, &layerProperties);
            if(layerProperties.origSourceWidth==0 && layerProperties.origSourceHeight==0)
            {
                std::cout << "layer is craeted. register callback." << std::endl;
                ilm_surfaceAddNotification(id, SurfaceCallback);
            }
        }
        else
        {
            ;
        }
    }
}