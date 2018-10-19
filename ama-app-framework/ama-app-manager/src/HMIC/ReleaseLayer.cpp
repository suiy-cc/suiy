/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ReleaseLayer.cpp
/// @brief contains class ReleaseLayer
/// 
/// Created by zs on 2017/11/27.
///
/// this file contains the implementation of class ReleaseLayer
///

#include <algorithm>

#include "COMMON/HandleManagement.h"
#include "ILM/Surface.h"
#include "ILM/ILMException.h"
#include "log.h"
#include "ReleaseLayer.h"
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "HMICTask.h"


ThreadSafeValueKeeper<RectCache> ReleaseLayer::rectCache;
ThreadSafeValueKeeper<HandleMap> ReleaseLayer::surfaceCBHandles;

const unsigned int ReleaseLayer::width = 1280;
const unsigned int ReleaseLayer::height = 720;
const unsigned int ReleaseLayer::maxSurfacePos = ReleaseLayer::width*ReleaseLayer::height;
unsigned int ReleaseLayer::objExistanceCBHandle = 0;

void ReleaseLayer::RegisterObjCreateNotify()
{
    if(objExistanceCBHandle == 0 || !IsRegistered(objExistanceCBHandle))
        objExistanceCBHandle = RegisterObjExistanceNotification([](ilmObjectType type, unsigned int id, bool create){
            DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%u, %#x, %s)", type, id, create?"true":"false"));

            if(type==ILM_SURFACE)
            {
                if(!create)
                {
                    ReleaseLayer::rectCache.Operate([id](RectCache& cache){
                        cache.erase(Surface(id));
                        logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(id), " is removed from surface callback set.");
                    });
                }
            }
        });
}

bool ReleaseLayer::IsAlreadyOn(const Surface& surface)const
{
    bool isOnReleaseLayer = false;
    LayerID layerID = (surface.GetContainerLayer());
    if(GET_LAYER(layerID)==E_LAYER_RELEASE_HMI || GET_LAYER(layerID)==E_LAYER_RELEASE_PLAY)
        isOnReleaseLayer = true;

    bool isInRectCache = false;
    rectCache.OperateConst([&isInRectCache, surface](const RectCache& rectCache){
        isInRectCache = (rectCache.find(surface)!=rectCache.end());
    });

    return isInRectCache && isOnReleaseLayer;
}

ReleaseLayer::ReleaseLayer()
{
    RegisterObjCreateNotify();
}

ReleaseLayer::ReleaseLayer(unsigned int layerID)
: Layer(layerID)
{
    RegisterObjCreateNotify();
}

void ReleaseLayer::AddSurface(const Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", surface.GetID()));

    // return if specified surface is already on a release layer
    if(IsAlreadyOn(surface))
    {
        logWarn(HMIC, LOG_HEAD, "current surface is already on release layer! return.");
        return;
    }

    // cache surface's rectangle
    CacheSurfaceDestinationRectangle(surface);

    // change specified surface's size into one point
    Surface mutableSurface = surface;
    FitSurface2ItsSpot(mutableSurface);

    // monitor every change made to the destination rectangle of current surface
    surfaceCBHandles.Operate([&mutableSurface](HandleMap& handles){
        handles[mutableSurface] = mutableSurface.SetNotifyCallback(&ReleaseLayer::OnSurfacePropertiesChange);
    });

    // put sepcified surface onto current layer
    Layer::AddSurface(surface);
}

void ReleaseLayer::RemoveSurface(const Surface &surface)
{
    Layer::RemoveSurface(surface);
    Surface mutableSurface = surface;

    // cancel properties monitoring
    surfaceCBHandles.Operate([&mutableSurface](HandleMap& handles){
        if(handles.find(mutableSurface)!=handles.end())
        {
            mutableSurface.RemoveNotifyCallback(handles[mutableSurface]);
            handles.erase(mutableSurface);
        }
    });

    // restore surface's destination rectangle
    bool isFound = false;
    Section orignalRect = {0,0,0,0};
    rectCache.Operate([&orignalRect,surface, &isFound](RectCache& rectCache){
        if(rectCache.find(surface)!=rectCache.end())
        {
            orignalRect = rectCache.at(surface);
            isFound = true;
            rectCache.erase(surface);
        }
        else
            isFound = false;
    });

    if(isFound)
        mutableSurface.SetDestinationRectangle(orignalRect);
    else
        logError(HMIC, LOG_HEAD, "a double removed surface ", ToHEX(surface.GetID()), " is found!");
}

void ReleaseLayer::FixSurfaceDestinationRectangle(Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", surface.GetID()));

    // cache surface's rectangle
    CacheSurfaceDestinationRectangle(surface);

    // change specified surface's size into one point
    FitSurface2ItsSpot(surface);
}

void ReleaseLayer::DeleteRectCache(const Surface &surface)
{
    rectCache.Operate([surface](RectCache& rectCache){
        rectCache.erase(surface);
    });
}

Point ReleaseLayer::GetNextPos()
{
    HandleManager<unsigned int> handles;
    auto surfaces = Layer::GetAllSurfaces();
    for(auto& surface: surfaces)
    {
        auto rect = surface.GetDestinationRectangle();
        unsigned int pos = rect.left + rect.top*width;
        handles.CreateHandle(pos);
    }
    unsigned int pos = handles.GetMinAvailableHandle();
    return Point{pos%width, pos/width};
}

void ReleaseLayer::OnSurfacePropertiesChange(SurfaceID id, SurfaceProperties* properties, NotificationMask mask)
{
    // check if current notify is relative
    if(!(mask&ILM_NOTIFICATION_DEST_RECT))
        return;

    // check if current surface is in surface record.
    bool isInRecord = false;
    rectCache.OperateConst([&isInRecord, id](const RectCache& rectCache){
        isInRecord = rectCache.find(Surface(id))!=rectCache.end();
    });
    if(!isInRecord)
        return;

    if(properties->destHeight==1 && properties->destWidth==1)
        return;

    {
        DomainVerboseLog chatter(HMIC, formatText("ReleaseLayer::OnSurfacePropertiesChange(%#x)", id));

        auto userData = reinterpret_cast<unsigned long long>(new SurfaceProperties(*properties));

        auto hmicTask = std::make_shared<HMICTask>();
        hmicTask->SetSurfaceID(id);
        hmicTask->SetType(HMIC_NOTIFY_SURFACE_DESTINATION_RECTANGLE_CHANGED_ON_RELEASE_LAYER);
        hmicTask->SetUserData(userData, [](unsigned long long userData){delete reinterpret_cast<SurfaceProperties*>(userData);});

        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    }
}

void ReleaseLayer::CacheSurfaceDestinationRectangle(const Surface& surface)
{
    // cache surface's rectangle
    auto rect = surface.GetDestinationRectangle();
    rectCache.Operate([rect,surface](RectCache& rectCache){
        rectCache[surface] = rect;
        logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " is cached.");
        logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " 's rectangle: ", rect.left, " , ", rect.top, " , ", rect.width, " , ", rect.height, " .");
    });
}

void ReleaseLayer::FitSurface2ItsSpot(Surface& surface)
{
    auto pos = GetNextPos();

    // change specified surface's size into one point
    surface.SetDestinationRectangle(Section{pos.x, pos.y, 1,1});
}
