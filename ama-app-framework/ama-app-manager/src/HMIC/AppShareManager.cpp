/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppShareManager.cpp
/// @brief contains the implementation of class AppShareManager
///
/// Created by zs on 2016/12/19.
/// this file contains the implementation of class AppShareManager
///

#include "ProcessInfo.h"
#include "AppUIHMI.h"
#include "AppShareManager.h"
#include "SMTask.h"
#include "STHTask.h"
#include "Capi.h"
#include "SurfaceDestroyBroadcaster.h"

AppShareManager::AppShareManager()
{
    surfaceDestroyBroadcaster.Register(AppShareManager::OnSourceSurfaceDestroyed);
}

AppShareManager::~AppShareManager()
{
    surfaceDestroyBroadcaster.Unregister(AppShareManager::OnSourceSurfaceDestroyed);
}

AppShareManager *AppShareManager::GetInstance()
{
    static AppShareManager instance;

    return &instance;
}

void AppShareManager::AddContext(AMPID app, shared_ptr<AppShareContext> context)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::AddContext(%#x)", app));

    contexts.Operate([=](ContextMap& contextMap){
        contextMap[app] = context;
    });
}
void AppShareManager::RemoveContext(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::RemoveContext(%#x)", app));

    contexts.Operate([=](ContextMap& contextMap){
        contextMap.erase(app);
    });
}

std::vector<Surface> AppShareManager::GetSourceSurfaces(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::GetSourceSurfaces(%#x)", app));

    std::vector<Surface> rtv;
    auto context = GetContext(app);
    if(context)
    {
        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto appObj = procInfo->FindApp(context->sourceApp);
        procInfo->Unlock();

        if(appObj)
        {
            auto appUIHMI = dynamic_pointer_cast<AppUIHMI>(appObj->GetUI());
            if(appUIHMI)
            {
                return std::move(appUIHMI->GetSynchronizableSurfaces());
            }
            else
                return std::move(rtv);
        }
        else
            return std::move(rtv);
    }
    else
        return std::move(rtv);
}
std::vector<Layer> AppShareManager::GetWorkingLayers(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::GetWorkingLayers(%#x)", app));

    std::vector<Layer> rtv;
    std::vector<Layer> workingLayers = {Layer(0x40001000), Layer(0x40002000), Layer(0x40003000)};

    auto context = GetContext(app);
    if(context)
    {
        if(context->isMove)
        {
            auto totalLayers = GetSourceSurfaces(app).size();
            for(int i = 0; i < totalLayers && i < workingLayers.size(); ++i)
                rtv.push_back(workingLayers[i]);
        }
    }

    return std::move(rtv);
}

shared_ptr<AppShareContext> AppShareManager::GetContext(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::GetContext(%#x)", app));

    shared_ptr<AppShareContext> rtv = nullptr;

    contexts.Operate([&](ContextMap& contextMap){
        auto iter = contextMap.find(app);
        if(iter!=contextMap.end())
            rtv = iter->second;
    });

    return rtv;
}

ContextPtrs AppShareManager::GetContext(std::function<bool(shared_ptr<AppShareContext>)> filter)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::GetContext(filter)"));

    ContextPtrs rtv;

    contexts.Operate([&](ContextMap& contextMap){
        for(auto& item: contextMap)
            if(filter(item.second))
                rtv.push_back(item.second);
    });

    return std::move(rtv);
}

void AppShareManager::OnSourceSurfaceDestroyed(const Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText("AppShareManager::OnSourceSurfaceDestroyed(%#x)", surface.GetID()));

    auto invalidShares = GetInstance()->GetContext([&](std::shared_ptr<AppShareContext> context)->bool{
        return context->sourceApp == GetAMPID(surface);
    });
    if(invalidShares.empty())
        return;

    logWarn(HMIC, "AppShareManager::OnSourceSurfaceDestroyed(): source app's surface ", ToHEX(surface.GetID()), " destroyed! app share is going to be shut down!");

    for(auto& context: invalidShares)
    {
        DispatchScreenShareTask(context->sourceApp, context->destinationScreen, context->isMove, STH_STOP_SCREEN_SAHRE, nullptr);

        ::AppShareStateChangeNotify(context->sourceApp, context->destinationScreen, E_PROC_STATE_DEAD);
    }
}