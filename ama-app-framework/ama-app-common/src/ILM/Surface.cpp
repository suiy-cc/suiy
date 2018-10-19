/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Surface.cpp
/// @brief contains the implementation of class Surface
///
/// Created by zs on 2016/6/15.
/// this file contains the implementation of class Surface
///

#include <algorithm>
#include <string>
#include <memory>
#include <thread>

#include <ilm_control.h>
#include <ilm_client.h> // for create & remove surface. I don't know why they are there.
#include <ilm_input.h>
#include <COMMON/UniqueID.h>

#include "ILM/Layer.h"
#include "ILM/Surface.h"
#include "ILM/ILMClient.h"
#include "ILM/ILMException.h"

#include "COMMON/NotifyWaiter.h"
#include "COMMON/DomainInvoker.h"

#include "log.h"
#include "ama_types.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

LOG_DECLARE_CONTEXT(SURF)

ThreadSafeValueKeeper<Surface::CallbackMap> Surface::callbackKeeper;
unsigned long long Surface::objExistanceCBHandle = 0;

Surface::Surface()
: surfaceID(0)
{
    ;
}

Surface::Surface(SurfaceID ID)
: surfaceID(ID)
{
    ;
}

Surface::~Surface()
{
    ;
}

void Surface::Create(const Dimension dimension, const WindowHandle handle, const PixelFormat& pixelFormat)
{
    ilmErrorTypes errorTypes = ilm_surfaceCreate(handle, dimension.width, dimension.height, pixelFormat, &surfaceID);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Surface::Create()");
        throw e;
    }
}

void Surface::Destroy()
{
    ilmErrorTypes errorTypes = ilm_surfaceRemove(surfaceID);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << surfaceID;
        ILMException e(errorTypes, std::string("Surface::Destroy(")+i2s.str()+")");
        throw e;
    }
}

bool Surface::operator==(const Surface& surface)const
{
    return surfaceID==surface.surfaceID;
}


bool Surface::operator!=(const Surface& surface)const
{
    return !( surface==(*this) );
}

Dimension Surface::GetDimension()const
{
    SurfaceProperties surfaceProperties;
    _GetProperties(&surfaceProperties);

    Dimension dimension;
    dimension.width = surfaceProperties.origSourceWidth;
    dimension.height = surfaceProperties.origSourceHeight;

    return dimension;
}



static void VisibilityCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask, unsigned long long id)
{
    DomainVerboseLog chatter(SURF, formatText("VisibilityCallback(%#x, %#x)", surface, mask));

    if(mask&ILM_NOTIFICATION_VISIBILITY)
    {
        logDebug(SURF, "surface ", ToHEX(surface), " changed visibility.");
        Notify(id);
    }
}

void Surface::SetVisible(const bool isVisible)
{
    DomainVerboseLog chatter(SURF, formatText("Surface::SetVisible(%#x, %s)", surfaceID, isVisible?"true":"false"));

    auto allSurfaces = GetAllSurfaces();
    if(std::find(allSurfaces.begin(), allSurfaces.end(), *this)==allSurfaces.end())
        return;

    if(!IsConfigured())
    {
        logWarn(SURF, "Surface::SetVisible(): surface is not configured! operation abort.");
        return;
    }

    if(IsVisible()==isVisible)
        return;

    // register a waiter
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);
    logDebug(SURF, "RegisterWaiterID()");

    // set callback-function
    unsigned long long cbHandle = 0;
    DomainInvoker domainInvoker(
        [&cbHandle, this, id](){try{cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){VisibilityCallback(surface, p, mask, id);});}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetVisible()");}},
        [&cbHandle, this](){try{RemoveNotifyCallback(cbHandle);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetVisible()");}});
    //ILMClient::Commit();

    // set visibility
    ilmErrorTypes errorTypes = ilm_surfaceSetVisibility(surfaceID, isVisible);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, std::string("Surface::SetVisible(")+ToHEX(surfaceID)+", "+(isVisible?"true":"false")+")");
        throw e;
    }
    ILMClient::Commit();

    // wait for weston to confirm the operation
    if(!Wait(id))
        logDebug(SURF,"Surface::SetVisible(): wait timeout!");
    UnregisterWaiterID(id);

    // remove callback-function
    domainInvoker.InvokeDestroyFunction();
    //ILMClient::Commit();
}

bool Surface::IsVisible()const
{
    t_ilm_bool isVisible = ILM_FALSE;
    ilmErrorTypes errorTypes = ilm_surfaceGetVisibility(surfaceID, &isVisible);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, std::string("Surface::IsVisible(")+ToHEX(surfaceID)+")");
        throw e;
    }

    return isVisible==ILM_TRUE;
}

static void OpacityCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask, unsigned long long id)
{
    DomainVerboseLog chatter(SURF, formatText("OpacityCallback(%#x, %#x)", surface, mask));

    if(mask&ILM_NOTIFICATION_OPACITY)
    {
        logDebug(SURF, "surface ", ToHEX(surface), " changed opacity.");
        Notify(id);
    }
}

void Surface::SetOpacity(const Alpha alpha)
{
    logDebug(SURF, "Surface::SetOpacity(", ToHEX(surfaceID), ", ", alpha, ")--->IN");

    // register a waiter
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);

    // set callback-function
    unsigned long long cbHandle = 0;
    DomainInvoker domainInvoker(
        [&cbHandle, this, id](){try{cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){OpacityCallback(surface, p, mask, id);});}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetOpacity()");}},
        [&cbHandle, this](){try{RemoveNotifyCallback(cbHandle);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetOpacity()");}});
    //ILMClient::Commit();

    // set opacity
    ilmErrorTypes errorTypes = ilm_surfaceSetOpacity(surfaceID, alpha);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, std::string("Surface::SetOpacity(")+ToHEX(surfaceID)+", "+std::to_string(alpha)+")");
        throw e;
    }
    ILMClient::Commit();

    // wait for weston to confirm the operation
    if(!Wait(id))
        logDebug(SURF,"Surface::SetOpacity(): wait timeout!");
    UnregisterWaiterID(id);

    // remove callback-function
    domainInvoker.InvokeDestroyFunction();
    //ILMClient::Commit();

    logDebug(SURF, "Surface::SetOpacity(", ToHEX(surfaceID), ", ", alpha, ")--->OUT");
}

Alpha Surface::GetOpacity()const
{
    Alpha alpha;
    ilmErrorTypes errorTypes = ilm_surfaceGetOpacity(surfaceID, &alpha);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << surfaceID;
        ILMException e(errorTypes, std::string("Surface::GetOpacity(")+i2s.str()+")");
        throw e;
    }
    return alpha;
}

void SurfaceCallback(SurfaceID surface, SurfaceProperties* properties, NotificationMask mask)
{
    DomainVerboseLog chatter(SURF, formatText("SurfaceCallback(%#x, %#x)", surface, mask));

    bool isCallbackExists = false;
    Surface::CallbackSet* callbackSet = nullptr;
    Surface::callbackKeeper.Operate([&callbackSet, surface, &isCallbackExists](Surface::CallbackMap& id2cb){
        if(id2cb.find(surface)!=id2cb.end() && id2cb[surface].GetSize())
        {
            isCallbackExists = true;
            callbackSet = &(id2cb[surface]);
            logInfo(SURF, "SurfaceCallback(): callback is found.");
        }
    });

    if(isCallbackExists && callbackSet->GetSize())
        callbackSet->NotifyAll([surface, properties, mask](std::function<void(SurfaceID surface, SurfaceProperties* properties, NotificationMask mask)> function){
            function(surface, properties, mask);
        });
}

unsigned long long Surface::SetNotifyCallback(const std::function<void(SurfaceID, SurfaceProperties*, NotificationMask)>& function, bool forceCreate)
{
    bool isCreated = false;
    unsigned long long handle = 0;
    callbackKeeper.Operate([&handle, &isCreated, this, &function](Surface::CallbackMap& id2cb){
        isCreated = id2cb.find(surfaceID)!=id2cb.end();
        handle = id2cb[surfaceID].Register(function);
    });

    if(forceCreate)
        isCreated = false;

    if(!isCreated)
        _SetNotifyCallback(SurfaceCallback);

    return handle;
}

void Surface::RemoveNotifyCallback(unsigned long long handle)
{
    callbackKeeper.Operate([this, handle](Surface::CallbackMap& id2cb){
        if(id2cb.find(surfaceID)!=id2cb.end())
            id2cb[surfaceID].Unregister(handle); // so that the raw callback won't be created more that once.
    });
}

void Surface::_SetNotifyCallback(const SurfaceNotifyCallback& function)
{
    ilmErrorTypes errorTypes = ilm_surfaceAddNotification(surfaceID, function);
    // there's existing callback
    if(errorTypes==ILM_ERROR_INVALID_ARGUMENTS)
    {
        _RemoveNotifyCallback();
        errorTypes = ilm_surfaceAddNotification(surfaceID, function);
        logInfo(SURF, "");
    }

    // error report
    if(errorTypes!=ILM_SUCCESS)
    {
        logWarn(SURF, "Surface::_SetNotifyCallback(): ErrorType = ", errorTypes);
        ILMException e(errorTypes, std::string("Surface::_SetNotifyCallback(")+ToHEX(surfaceID)+")");
        throw e;
    }
}

void Surface::_RemoveNotifyCallback()
{
    ilmErrorTypes errorTypes = ilm_surfaceRemoveNotification(surfaceID);
    // success or no existing callback
    if(errorTypes!=ILM_SUCCESS && errorTypes!=ILM_ERROR_INVALID_ARGUMENTS)
    {
        ILMException e(errorTypes, std::string("Surface::_RemoveNotifyCallback(")+ToHEX(surfaceID)+")");
        throw e;
    }
}

void Surface::RegisterSurfaceDestroyCB()
{
    if(objExistanceCBHandle==0 || !IsRegistered(objExistanceCBHandle))
        objExistanceCBHandle = RegisterObjExistanceNotification([](ilmObjectType type, unsigned int id, bool create){
            DomainVerboseLog chatter(SURF, formatText(__STR_FUNCTION__+"(%u, %#x, %s)", type, id, create?"true":"false"));

            if(type==ILM_SURFACE)
            {
                if(!create)
                {
                    Surface::callbackKeeper.Operate([id](Surface::CallbackMap& callbackMap){
                        callbackMap.erase(id);
                        logInfo(SURF, LOG_HEAD, "surface ", ToHEX(id), " is removed from surface callback set.");
                    });
                }
            }
        });
}

void Surface::_GetProperties(SurfaceProperties *surfaceProperties)const
{
    DomainVerboseLog chatter(SURF, formatText("Surface::_GetProperties(%#x)", surfaceID));

    ilmErrorTypes errorTypes = ILM_FAILED;
    int retryTime = 7; // magic number
    while(errorTypes==ILM_FAILED && retryTime>0)
    {
        errorTypes = ilm_getPropertiesOfSurface(surfaceID, surfaceProperties);
        --retryTime;
        if(errorTypes==ILM_FAILED)
            logWarn(SURF, "Surface::_GetProperties(): error in getting properties, retry.");
    }
    if(errorTypes!=ILM_SUCCESS)
    {
        logWarn(SURF, "Surface::_GetProperties(): ErrorType = ", errorTypes);
        ILMException e(errorTypes, std::string("Surface::_GetProperties(")+ToHEX(surfaceID)+")");
        throw e;
    }
}

shared_ptr<SurfaceProperties> Surface::GetProperties()const
{
    auto surfaceProperties = std::make_shared<SurfaceProperties>();
    _GetProperties(surfaceProperties.get());

    return surfaceProperties;
}

static void SourceRectangleCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask, unsigned long long id)
{
    DomainVerboseLog chatter(SURF, formatText("SourceRectangleCallback(%#x, %#x)", surface, mask));

    if(mask&ILM_NOTIFICATION_SOURCE_RECT)
    {
        logDebug(SURF, "surface ", ToHEX(surface), " changed source rectangle.");
        Notify(id);
    }
}

void Surface::SetSourceRectangle(const Section& section)
{
    logDebug(SURF, "Surface::SetSourceRectangle(", ToHEX(surfaceID), ")--->IN");

    // register a waiter
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);

    // set callback-function
    unsigned long long cbHandle = 0;
    DomainInvoker domainInvoker(
        [&cbHandle, this, id](){try{cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){SourceRectangleCallback(surface, p, mask, id);});}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetSourceRectangle()");}},
        [&cbHandle, this](){try{RemoveNotifyCallback(cbHandle);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetSourceRectangle()");}});
    //ILMClient::Commit();

    // set source rectangle
    ilmErrorTypes errorTypes = ilm_surfaceSetSourceRectangle(surfaceID, section.left, section.top, section.width, section.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << surfaceID;
        ILMException e(errorTypes, std::string("Surface::SetSourceRectangle(")+i2s.str()+")");
        throw e;
    }
    ILMClient::Commit();

    // wait for weston to confirm the operation
    if(!Wait(id))
        logDebug(SURF,"Surface::SetSourceRectangle(): wait timeout!");
    UnregisterWaiterID(id);

    // remove callback-function
    domainInvoker.InvokeDestroyFunction();
    //ILMClient::Commit();

    logDebug(SURF, "Surface::SetSourceRectangle(", ToHEX(surfaceID), ")--->OUT");
}

Section Surface::GetSourceRectangle()const
{
    Section section;
    SurfaceProperties surfaceProperties;
    _GetProperties(&surfaceProperties);
    section.left = surfaceProperties.sourceX;
    section.top = surfaceProperties.sourceY;
    section.width = surfaceProperties.sourceWidth;
    section.height = surfaceProperties.sourceHeight;

    return section;
}

static void DestinationRectangleCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask, unsigned long long id)
{
    DomainVerboseLog chatter(SURF, formatText("DestinationRectangleCallback(%#x, %#x)", surface, mask));

    if(mask&ILM_NOTIFICATION_DEST_RECT)
    {
        logDebug(SURF, "surface ", ToHEX(surface), " changed destination rectangle.");
        Notify(id);
    }
}

void Surface::SetDestinationRectangle(const Section& section)
{
    logDebug(SURF, "Surface::SetDestinationRectangle(", ToHEX(surfaceID), ")--->IN");

    // register a waiter
    unsigned long long id = GetUniqueID();

    // set callback-function
    unsigned long long cbHandle = 0;
    DomainInvoker domainInvoker(
        [&cbHandle, this, id](){try{cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){DestinationRectangleCallback(surface, p, mask, id);});}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetDestinationRectangle()");}},
        [&cbHandle, this](){try{RemoveNotifyCallback(cbHandle);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetDestinationRectangle()");}});
    //ILMClient::Commit();

    // set destination rectangle
    ilmErrorTypes errorTypes = ilm_surfaceSetDestinationRectangle(surfaceID, section.left, section.top, section.width, section.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << surfaceID;
        ILMException e(errorTypes, std::string("Surface::SetDestinationRectangle(")+i2s.str()+")");
        throw e;
    }
    ILMClient::Commit();

    // wait for weston to confirm the operation
    if(!Wait(id))
        logDebug(SURF,"Surface::SetDestinationRectangle(): wait timeout!");
    UnregisterWaiterID(id);

    // remove callback-function
    domainInvoker.InvokeDestroyFunction();
    //ILMClient::Commit();

    logDebug(SURF, "Surface::SetDestinationRectangle(", ToHEX(surfaceID), ")--->OUT");
}

Section Surface::GetDestinationRectangle()const
{
    Section section;
    SurfaceProperties surfaceProperties;
    _GetProperties(&surfaceProperties);
    section.left = surfaceProperties.destX;
    section.top = surfaceProperties.destY;
    section.width = surfaceProperties.destWidth;
    section.height = surfaceProperties.destHeight;

    return section;
}


void Surface::SetBothRectangle(const Section& section)
{
    DomainVerboseLog chatter(SURF, formatText("Surface::SetBothRectangle(%#x, %d, %d, %d, %d)", surfaceID, section.left, section.top, section.width, section.height));

//    // register a waiter for source rectangle
//    unsigned long long sourceRectID = surfaceID;
//    sourceRectID = sourceRectID << 8*4;
//    sourceRectID |= ILM_NOTIFICATION_SOURCE_RECT;
//    RegisterWaiterID(sourceRectID);

    // register a waiter for destination rectangle
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);

//    // set callback-function of source rectangle
//    DomainInvoker domainInvokerSource(
//        [=](){try{SetNotifyCallback(SourceRectangleCallback);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetSourceRectangle()");}},
//        [=](){try{RemoveNotifyCallback();}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetSourceRectangle()");}});
//    ILMClient::Commit();

    // set callback-function of destination rectangle
    unsigned long long cbHandle = 0;
    DomainInvoker domainInvokerDestination(
        [&cbHandle, this, id](){try{cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){DestinationRectangleCallback(surface, p, mask, id);});}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetDestinationRectangle()");}},
        [&cbHandle, this](){try{RemoveNotifyCallback(cbHandle);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetDestinationRectangle()");}});
    //ILMClient::Commit();

    // set source rectangle
    ilmErrorTypes errorTypesSource = ilm_surfaceSetSourceRectangle(surfaceID, section.left, section.top, section.width, section.height);
    if(errorTypesSource!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << std::hex << surfaceID;
        ILMException e(errorTypesSource, std::string("Surface::SetBothRectangle(")+i2s.str()+"): during setting source rectangle");
        throw e;
    }

    // set destination rectangle
    ilmErrorTypes errorTypesDestination = ilm_surfaceSetDestinationRectangle(surfaceID, section.left, section.top, section.width, section.height);
    if(errorTypesDestination!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << std::hex << surfaceID;
        ILMException e(errorTypesDestination, std::string("Surface::SetBothRectangle(")+i2s.str()+"): during setting destination rectangle");
        throw e;
    }

    // commit setSourceRectangle & setDestinationRectangle
    ILMClient::Commit();

//    // wait for weston to confirm the set operation of source rectangle
//    if(!Wait(sourceRectID))
//        logDebug(SURF,"Surface::SetBothRectangle(): wait source rectangle timeout!");
//    UnregisterWaiterID(sourceRectID);

    // wait for weston to confirm the set operation of destination rectangle
    if(!Wait(id))
        logDebug(SURF,"Surface::SetBothRectangle(): wait destination rectangle timeout!");
    UnregisterWaiterID(id);

//    // remove callback-function
//    domainInvokerSource.InvokeDestroyFunction();

    // remove callback-function
    domainInvokerDestination.InvokeDestroyFunction();
    //ILMClient::Commit();
}

static void OrientationCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask, unsigned long long id)
{
    DomainVerboseLog chatter(SURF, formatText("OrientationCallback(%#x, %#x)", surface, mask));

    if(mask&ILM_NOTIFICATION_ORIENTATION)
    {
        logDebug(SURF, "surface ", ToHEX(surface), " changed orientation.");
        Notify(id);
    }
}

void Surface::SetOrientation(const Orientation orientation)
{
    logDebug(SURF, "Surface::SetOrientation(", ToHEX(surfaceID), ", ", orientation, ")--->IN");

    // register a waiter
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);

    // set callback-function
    unsigned long long cbHandle = 0;
    DomainInvoker domainInvoker(
        [&cbHandle, id, this](){try{cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){OrientationCallback(surface, p, mask, id);});}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetOrientation()");}},
        [&cbHandle, this](){try{RemoveNotifyCallback(cbHandle);}catch(ILMException e){logWarn(SURF, e.what(), " caught in Surface::SetOrientation()");}});
    //ILMClient::Commit();

    // set orientation
    ilmErrorTypes errorTypes = ilm_surfaceSetOrientation(surfaceID, orientation);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << surfaceID;
        ILMException e(errorTypes, std::string("Surface::SetOrientation(")+i2s.str()+")");
        throw e;
    }
    ILMClient::Commit();

    // wait for weston to confirm the operation
    if(!Wait(id))
        logDebug(SURF,"Surface::SetOrientation(): wait timeout!");
    UnregisterWaiterID(id);

    // remove callback-function
    domainInvoker.InvokeDestroyFunction();
    //ILMClient::Commit();

    logDebug(SURF, "Surface::SetOrientation(", ToHEX(surfaceID), ", ", orientation, ")--->OUT");
}

PixelFormat Surface::GetPixelFormat()const
{
    PixelFormat pixelFormat;
    ilmErrorTypes errorTypes = ilm_surfaceGetPixelformat(surfaceID, &pixelFormat);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << std::hex << surfaceID;
        ILMException e(errorTypes, std::string("Surface::GetPixelFormat(")+i2s.str()+")");
        throw e;
    }
    return pixelFormat;
}

void Surface::TakeScreenShot(const std::string& path)
{
    ilmErrorTypes errorTypes = ilm_takeSurfaceScreenshot(path.c_str(), surfaceID);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << surfaceID;
        ILMException e(errorTypes, std::string("Surface::TakeScreenShot(")+i2s.str()+")");
        throw e;
    }
}

LayerID Surface::GetLayerID()const
{
    return GET_LAYERID(surfaceID);
}

int Surface::GetCreatorPID()const
{
    SurfaceProperties surfaceProperties;
    _GetProperties(&surfaceProperties);

    return surfaceProperties.creatorPid;
}

LayerID Surface::GetContainerLayer()const
{
    ilmSurfaceExtendedProperties properties;
    properties.layer = INVALID_ID;
    auto errorTypes = ilm_getExtendedPropertiesOfSurface(surfaceID, &properties);
    if(errorTypes!=ILM_SUCCESS)
    {
        logWarn(SURF, "Surface::GetContainerLayer(): ErrorType = ", errorTypes);
        ILMException e(errorTypes, std::string("Surface::GetContainerLayer(")+ToHEX(surfaceID)+")");
        throw e;
    }

    return properties.layer;
}

bool Surface::IsExist()const
{
    auto surfaces = GetAllSurfaces();
    auto iter = std::find(surfaces.begin(), surfaces.end(), *this);

    return iter!=surfaces.end();
}

void Surface::SetConfiguration(const Size& size)
{
    DomainVerboseLog chatter(SURF, formatText("Surface::SetConfiguration(%#x, %u, %u)", surfaceID, size.width, size.height));

    ilmErrorTypes errorTypes = ilm_surfaceSetConfiguration(surfaceID, size.width, size.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << std::hex << surfaceID;
        ILMException e(errorTypes, std::string("Surface::SetConfiguration(")+i2s.str()+")");
        throw e;
    }
}

bool Surface::IsConfigured()const
{
    DomainVerboseLog chatter(SURF, "Surface::IsConfigured()");

    Dimension dimension = GetDimension();
    logInfo(SURF, "Surface::IsConfigured(): surface = ", ToHEX(surfaceID), " dimension = ", dimension.width, ",", dimension.height);
    return dimension.width!=0 && dimension.height!=0;
}

static void ConfiguredCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask, unsigned long long id)
{
    DomainVerboseLog chatter(SURF, formatText("ConfiguredCallback(%#x, %#x)", surface, mask));

    if(mask&ILM_NOTIFICATION_CONFIGURED)
    {
        logDebug(SURF, "ConfiguredCallback(): surface ", ToHEX(surface), " is configured.");
        Notify(id);
    }
}

bool Surface::Wait4SurfaceCongfigued(unsigned int timeout)
{
    DomainVerboseLog chatter(SURF, formatText("Surface::Wait4SurfaceCongfigued(%ud)", timeout));

    // register waiter ID
    unsigned long long id = GetUniqueID();
    DomainInvoker invoker1([id](){RegisterWaiterID(id);}, [id](){UnregisterWaiterID(id);});

    unsigned long long cbHandle = 0;
    DomainInvoker invoker2(
        [&cbHandle, id, this](){cbHandle = SetNotifyCallback([id](SurfaceID surface, SurfaceProperties* p, NotificationMask mask){ConfiguredCallback(surface, p, mask, id);});},
        [&cbHandle, this](){RemoveNotifyCallback(cbHandle);});

    if(IsConfigured())
    {
        logInfo(SURF, "Surface::Wait4SurfaceCongfigued(): surface already configured.");
        return true;
    }

    bool rtv = true;
    // wait for ILM_NOTIFICATION_CONFIGURED notification
    if(timeout==0)
    {
        while(!Wait(id))
            logWarn(SURF, "Surface::Wait4SurfaceCongfigued(): wait for configured notify for too long! keep waiting...");
    }
    else
    {
        if(!Wait(id, timeout))
        {
            rtv = false;
            logWarn(SURF, "Surface::Wait4SurfaceCongfigued(): timeout!");
        }
    }

    // remove surface notification callback
    invoker2.InvokeDestroyFunction();

    // unregister waiter ID
    invoker1.InvokeDestroyFunction();

    return rtv;
}

void Surface::SetInputDevice(const InputPeripheral& input)
{
    DomainVerboseLog chatter(SURF, formatText("Surface::SetInputDevice(%#x, %s)", surfaceID, input.GetName().c_str()));

    const unsigned int seatCount = 1;
    auto seat = input.GetName().c_str();
    char* mutableSeat = (char*)ama_malloc(strlen(seat)+1);
    strncpy(mutableSeat, seat, strlen(seat)+1);
    auto errorTypes = ILM_SUCCESS;
#ifndef CREATE_LAYERS_BY_AMGR
    errorTypes = ilm_setInputAcceptanceOn(surfaceID, seatCount, &mutableSeat);
#endif
    ama_free(mutableSeat);
    if(errorTypes!=ILM_SUCCESS)
    {
        std::stringstream i2s;
        i2s << std::hex << surfaceID;
        ILMException e(errorTypes, std::string("Surface::SetInputDevice(")+i2s.str()+")");
        throw e;
    }
}

std::vector<InputPeripheral> Surface::GetInputDevice()const
{
    DomainVerboseLog chatter(SURF, formatText("Surface::GetInputDevice(%#x)", surfaceID));

    unsigned int seatCount = 0;
    char** seat = nullptr;
    auto errorTypes = ILM_SUCCESS;
#ifndef CREATE_LAYERS_BY_AMGR
    errorTypes = ilm_getInputAcceptanceOn(surfaceID, &seatCount, &seat);
#endif
    if(errorTypes!=ILM_SUCCESS)
    {
        if(seat && seatCount>0)
            ama_free(seat);

        std::stringstream i2s;
        i2s << std::hex << surfaceID;
        ILMException e(errorTypes, std::string("Surface::GetInputDevice(")+i2s.str()+")");
        throw e;
    }

    std::vector<InputPeripheral> inputs;
    for(int i = 0; i < seatCount; ++i)
        inputs.push_back(InputPeripheral(seat[i]));

    if(seat && seatCount>0)
        ama_free(seat);
}

SurfaceID Surface::GetID()const
{
    return surfaceID;
}

void Surface::SetID(const SurfaceID ID)
{
    surfaceID = ID;
}
bool Surface::operator<(const Surface &rhs) const
{
    return surfaceID < rhs.surfaceID;
}
bool Surface::operator>(const Surface &rhs) const
{
    return rhs < *this;
}
bool Surface::operator<=(const Surface &rhs) const
{
    return !(rhs < *this);
}
bool Surface::operator>=(const Surface &rhs) const
{
    return !(*this < rhs);
}

int GetSurfaceCount()
{
    int count = 0;
    t_ilm_surface* array = nullptr;
    ilmErrorTypes errorTypes = ilm_getSurfaceIDs(&count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "GetSurfaceCount()");
        throw e;
    }

    ama_free(array);
    return count;
}

std::vector<Surface> GetAllSurfaces()
{
    DomainVerboseLog chatter(SURF, "GetAllSurfaces()");

    int count = 0;
    t_ilm_surface* array = nullptr;
    ilmErrorTypes errorTypes = ilm_getSurfaceIDs(&count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "GetAllSurfaces()");
        throw e;
    }

    std::vector<Surface> surfaces;
    surfaces.reserve(count);
    for(int i=0; i<count; ++i)
        surfaces.push_back( Surface(array[i]) );

    ama_free(array);
    return std::move(surfaces);
}
