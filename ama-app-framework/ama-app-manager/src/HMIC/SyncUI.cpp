/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SyncUI.cpp
/// @brief contains the implementation of class SyncUI
///
/// Created by zs on 2016/12/19.
/// this file contains the implementation of class SyncUI
///

#include <HMIC/AppShareManager.h>
#include <ILM/ILMClient.h>
#include <algorithm>
#include "log.h"
#include "AMGRLogContext.h"
#include "SyncUI.h"
#include "ProcessInfo.h"

SyncUI::SyncUI()
: isSourceSurfaceInitialized(false)
{}

SyncUI::SyncUI(const Surface &surface)
    : VideoLikeUI(surface)
    , isSourceSurfaceInitialized(false)
{}

SyncUI::~SyncUI()
{

}

void SyncUI::_ConfigSurface(Surface &surface, const Section &section)
{
    DomainVerboseLog chatter(HMIC, "SyncUI::_ConfigSurface()");

    DoIfStarted([&](){VideoLikeUI::_ConfigSurface(surface, section);});


//    AMPID ampid = GET_AMPID(GetSurfaces().front().GetID());
//    auto context = AppShareManager::GetInstance()->GetContext(ampid);
//    if(context)
//    {
//        if(!context->isMove)
//        {
//            // if current layer is on any layer before,
//            // remove it from the layer at first.
//            auto layerID = surface.GetContainerLayer();
//            if(layerID)
//            {
//                logInfo(HMIC, "SyncUI::_ConfigSurface(): find container layer: ", ToHEX(layerID));
//                Layer(layerID).RemoveSurface(surface);
//                ILMClient::Commit();
//            }
//
//            // configure surface
//            VideoLikeUI::_ConfigSurface(surface, section);
//        }
//    }
}

void SyncUI::_AddSurfaces2ReleaseLayer()
{
    DoIfStarted([this](){VideoLikeUI::_AddSurfaces2ReleaseLayer();});
}

void SyncUI::_RemoveSurfacesFromReleaseLayer()
{
    DoIfStarted([this](){VideoLikeUI::_RemoveSurfacesFromReleaseLayer();});
}

void SyncUI::AddSurfaces2Layer()
{
    DomainVerboseLog chatter(HMIC, "SyncUI::AddSurfaces2Layer()");

    DoIfStarted([=](){VideoLikeUI::AddSurfaces2Layer();});
}

void SyncUI::RemoveSurfacesFromLayer()
{
    DomainVerboseLog chatter(HMIC, "SyncUI::RemoveSurfacesFromLayer()");

    DoIfStarted([=](){VideoLikeUI::RemoveSurfacesFromLayer();});
}

void SyncUI::SetVisible(const bool isVisible)
{
    DomainVerboseLog chatter(HMIC, formatText("SyncUI::SetVisible(%s)", isVisible?"true":"false"));

    DoIfStarted([=](){VideoLikeUI::SetVisible(isVisible);});
}

std::vector<Surface> SyncUI::GetSynchronizableSurfaces() const
{
    DomainVerboseLog chatter(HMIC, "SyncUI::GetSynchronizableSurfaces()");

    return std::vector<Surface>();
}

void SyncUI::DoIfStarted(std::function<void(void)> job)
{
    DomainVerboseLog chatter(HMIC, "SyncUI::DoIfStarted()");

    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appObj = procInfo->FindApp(GetAMPID());
    procInfo->Unlock();
    if(appObj)
    {
        if(!appObj->IsStarting())
            job();
        else
            logInfo(HMIC, "SyncUI::DoIfStarted(): specified operation is skipped when sync app is starting.");
    }
}

bool SyncUI::IsReady2Start() const
{
    if(isSourceSurfaceInitialized)
    {
        return sourceSurfaces.empty();
    }
    else
        return AppUIHMI::IsReady2Start();
}

void SyncUI::InitializeSourceSurfaces()
{
    DomainVerboseLog chatter(HMIC, "SyncUI::InitializeSourceSurfaces()");

    if(isSourceSurfaceInitialized)
    {
        logInfo(HMIC, "SyncUI::InitializeSourceSurfaces(): already initialized! exit.");
        return;
    }

    auto contexts = AppShareManager::GetInstance()->GetContext([=](std::shared_ptr<AppShareContext> appSC)->bool{
        return appSC->destinationScreen==GetSeat();
    });

    if(contexts.empty())
    {
        logError(HMIC, "SyncUI::InitializeSourceSurfaces(): no valid app-share-context!");
        return;
    }

    if(contexts.size()!=1)
    {
        logError(HMIC, "SyncUI::InitializeSourceSurfaces(): more than one app-share-context of a single share!");
        return;
    }

    auto context = contexts.front();

    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appObj = procInfo->FindApp(context->sourceApp);
    procInfo->Unlock();

    if(appObj)
    {
        auto appUIHMI = dynamic_pointer_cast<AppUIHMI>(appObj->GetUI());
        if(appUIHMI)
        {
            sourceSurfaces = appUIHMI->GetSynchronizableSurfaces();
            logInfo(HMIC, "SyncUI::InitializeSourceSurfaces(): source surfaces initialized.");
            isSourceSurfaceInitialized = true;
        }
    }
}

void SyncUI::RegisterSurface(Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText("SyncUI::RegisterSurface(%#x)", surface.GetID()));

    VideoLikeUI::RegisterSurface(surface);

    InitializeSourceSurfaces();

    logInfo(HMIC, "SyncUI::RegisterSurface(): source surface count = ", sourceSurfaces.size());
    sourceSurfaces.erase(std::remove_if(sourceSurfaces.begin(), sourceSurfaces.end(), [=](const Surface& s)->bool{
        return GetLayer(s)==GetLayer(surface);
    }), sourceSurfaces.end());
    logInfo(HMIC, "SyncUI::RegisterSurface(): source surface count = ", sourceSurfaces.size());
}
