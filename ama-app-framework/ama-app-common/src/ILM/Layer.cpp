/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Layer.cpp
/// @brief contains the implementation of class Layer
///
/// Created by zs on 2016/6/15.
/// this file contains the implementation of class Layer
///

#include <cassert>
#include <vector>
#include <algorithm>

#include <ilm_control.h>

#include "COMMON/Broadcaster.h"
#include "ILM/Layer.h"
#include "ILM/ILMException.h"
#include "log.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

extern LOG_DECLARE_CONTEXT(ILMA);

Layer::Layer()
: layerID(0)
{
    ;
}

Layer::Layer(LayerID ID)
: layerID(ID)
{
    ;
}

Layer::~Layer()
{
    ;
}

bool Layer::operator<(const Layer& otherLayer)const
{
    return layerID < otherLayer.layerID;
}

bool Layer::operator==(const Layer& otherLayer)const
{
    return layerID==otherLayer.layerID;
}

bool Layer::operator!=(const Layer& otherLayer)const
{
    return !((*this)==otherLayer);
}

void Layer::Create(const Dimension &dimension)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %u, %u)", layerID, dimension.width, dimension.height));

    ilmErrorTypes errorTypes = ilm_layerCreateWithDimension(&layerID, dimension.width, dimension.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::Create()");
        throw e;
    }
}

void Layer::Destroy()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    ilmErrorTypes errorTypes = ilm_layerRemove(layerID);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::Destroy()");
        throw e;
    }
}

void Layer::AddSurface(const Surface& surface)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %#x)", layerID, surface.GetID()));

    ilmErrorTypes errorTypes = ilm_layerAddSurface(layerID, surface.GetID());
    if(errorTypes==ILM_ERROR_INVALID_ARGUMENTS)
    {
        logWarn(ILMA, LOG_HEAD, "error: ILM_ERROR_INVALID_ARGUMENTS. ignored.");
        return;
    }
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::AddSurface()");
        throw e;
    }
}

void Layer::RemoveSurface(const Surface& surface)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %u)", layerID, surface.GetID()));

    auto allSurfaces = GetAllSurfaces();
    if(std::find(allSurfaces.begin(), allSurfaces.end(), surface)==allSurfaces.end())
        return;

    ilmErrorTypes errorTypes = ilm_layerRemoveSurface(layerID, surface.GetID());
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::RemoveSurface()");
        throw e;
    }
}

shared_ptr<LayerProperties> Layer::GetProperties()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    auto layerProperties = std::make_shared<LayerProperties>();
    _GetProperties(layerProperties.get());

    return layerProperties;
}

int Layer::GetSurfaceCount()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    int count = 0;
    t_ilm_surface* array = nullptr;
    ilmErrorTypes errorTypes = ilm_getSurfaceIDsOnLayer(layerID, &count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::GetSurfaceCount()");
        throw e;
    }
    ama_free(array);
}

std::vector<Surface> Layer::GetAllSurfaces()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    int count = 0;
    t_ilm_surface* array = nullptr;
    ilmErrorTypes errorTypes = ilm_getSurfaceIDsOnLayer(layerID, &count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::GetAllSurfaces()");
        throw e;
    }

    std::vector<Surface> surfaces;
    surfaces.reserve(count);
    for(int i=0; i<count; ++i)
        surfaces.push_back( Surface(array[i]) );

    ama_free(array);
    return std::move(surfaces);
}

LayerType Layer::GetLayerType()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    LayerProperties layerProperties;
    _GetProperties(&layerProperties);

    LayerType type = static_cast<LayerType>(layerProperties.type);

    return type;
}

void Layer::SetVisible(const bool isVisible)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %s)", layerID, isVisible?"true":"false"));

    ilmErrorTypes errorTypes = ilm_layerSetVisibility(layerID, isVisible);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetVisible()");
        throw e;
    }
}

bool Layer::IsVisible()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    t_ilm_bool isVisible = false;
    ilmErrorTypes errorTypes = ilm_layerGetVisibility(layerID, &isVisible);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::IsVisible()");
        throw e;
    }

    return isVisible==ILM_TRUE?true:false;
}

void Layer::SetOpacity(const Alpha alpha)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %f)", layerID, alpha));

    ilmErrorTypes errorTypes = ilm_layerSetOpacity(layerID, alpha);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetOpacity()");
        throw e;
    }
}

Alpha Layer::GetOpacity()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    Alpha alpha = 0.0;
    ilmErrorTypes errorTypes = ilm_layerGetOpacity(layerID, &alpha);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::GetOpacity()");
        throw e;
    }

    return alpha;
}

void Layer::SetSourceRectangle(const Section& section)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %u, %u, %u, %u)", layerID, section.left, section.top, section.width, section.height));

    ilmErrorTypes errorTypes = ilm_layerSetSourceRectangle(layerID, section.left, section.top, section.width, section.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetSourceRectangle()");
        throw e;
    }
}

void Layer::SetDestinationRectangle(const Section& section)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %u, %u, %u, %u)", layerID, section.left, section.top, section.width, section.height));

    ilmErrorTypes errorTypes = ilm_layerSetDestinationRectangle(layerID, section.left, section.top, section.width, section.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetDestinationRectangle()");
        throw e;
    }
}

Dimension Layer::GetDimension()const
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    LayerProperties layerProperties;
    _GetProperties(&layerProperties);

    Dimension dimension;
    dimension.width = layerProperties.origSourceWidth;
    dimension.height = layerProperties.origSourceHeight;

    return dimension;
}

Section Layer::GetRectangle()const
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    Dimension dimension = Layer::GetDimension();
    Section rect = {0,0,dimension.width,dimension.height};
    return rect;
}

void Layer::SetOrientation(const Orientation orientation)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %d)", layerID, orientation));

    ilmErrorTypes errorTypes = ilm_layerSetOrientation(layerID, orientation);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetOrientation()");
        throw e;
    }
}

Orientation Layer::GetOrientation()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    Orientation orientation;
    ilmErrorTypes errorTypes = ilm_layerGetOrientation(layerID, &orientation);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::GetOrientation()");
        throw e;
    }

    return orientation;
}

void Layer::SetRelativeRenderOrder(const std::vector<Surface>& surfaces)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    if(surfaces.size()<=1)
        return;

    std::vector<Surface> zorder = GetAllSurfaces();

    for(int i=1; i < surfaces.size(); ++i)
    {
        zorder.erase(std::remove(zorder.begin(), zorder.end(), surfaces[i]));
    }
    auto iter = std::find(zorder.begin(), zorder.end(), surfaces[0]);
    zorder.insert(++iter, ++surfaces.begin(), surfaces.end());

    SetRenderOrder(zorder);
}

void Layer::SetRenderOrder(const std::vector<Surface>& surfaces)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    int count = static_cast<int>(surfaces.size());
    t_ilm_surface* array = new t_ilm_surface[count];

    for(int i=0; i<count; ++i)
        array[i] = surfaces[i].GetID();

    ilmErrorTypes errorTypes = ilm_layerSetRenderOrder(layerID, array, count);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetRenderOrder()");
        throw e;
    }

    delete[] array;
}

void Layer::TakeScreenShot(const std::string& path)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x, %s)", layerID, path.c_str()));

    ilmErrorTypes errorTypes = ilm_takeLayerScreenshot(path.c_str(), layerID);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::TakeScreenShot()");
        throw e;
    }
}

bool Layer::SetNotifyCallback(const LayerNotifyCallback& function)
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    ilmErrorTypes errorTypes = ilm_layerAddNotification(layerID, function);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::SetNotifyCallback()");
        throw e;
    }
}

void Layer::RemoveNotifyCallback()
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    ilmErrorTypes errorTypes = ilm_layerRemoveNotification(layerID);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::RemoveNotifyCallback()");
        throw e;
    }
}

LayerID Layer::GetID()const
{
    return layerID;
}

void Layer::SetID(const LayerID ID)
{
    layerID = ID;
}

void Layer::_GetProperties(LayerProperties *layerProperties)const
{
    DomainVerboseLog chatter(ILMA, formatText(__STR_FUNCTION__+"(%#x)", layerID));

    ilmErrorTypes errorTypes = ilm_getPropertiesOfLayer(layerID, layerProperties);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Layer::_GetProperties()");
        throw e;
    }
}

int GetLayerCount()
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    t_ilm_int count = 0;
    t_ilm_layer *array = nullptr;
    ilmErrorTypes errorTypes = ilm_getLayerIDs(&count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "GetLayerCount()");
        throw e;
    }

    ama_free(array);
    return count;
}

std::vector<Layer> GetAllLayers()
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    t_ilm_int count = 0;
    t_ilm_layer *array = nullptr;
    ilmErrorTypes errorTypes = ilm_getLayerIDs(&count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "GetAllLayers()");
        throw e;
    }

    std::vector<Layer> layers;
    layers.reserve(count);
    for(int i=0; i<count; ++i)
        layers.push_back( Layer(array[i]) );

    ama_free(array);
    return std::move(layers);
}

/// @fn RegisterNotification
/// @brief register for notification on property creation/deletion events for surfaces/layers
void RegisterNotification(NotifyFunc func, void* userData)
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    ilmErrorTypes errorTypes = ilm_registerNotification(func, userData);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "RegisterNotification()");

        throw e;
    }
}

/// @see RegisterNotification
void UnregisterNotification()
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    ilmErrorTypes errorTypes = ilm_unregisterNotification();
    if(errorTypes==ILM_ERROR_INVALID_ARGUMENTS)
    {
        ILMException e(errorTypes, "UnregisterNotification()");

        throw e;
    }
}

typedef Broadcaster<void(ilmObjectType objType, unsigned int id, bool create)> ILMObjExistanceNotifyBroadcasterType;
static ILMObjExistanceNotifyBroadcasterType broadcaster;
static void ILMObjExistanceCallback(ilmObjectType type, t_ilm_uint id, t_ilm_bool create, void* user_data)
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    broadcaster.NotifyAll([type, id, create](ILMObjExistanceNotifyFunc func){
        func(type, id, create);
    });
}

unsigned long long RegisterObjExistanceNotification(ILMObjExistanceNotifyFunc f)
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    if(broadcaster.GetSize()==0)
    {
        try
        {
            RegisterNotification(ILMObjExistanceCallback, nullptr);
        }
        catch(ILMException& e)
        {
            logWarn(ILMA, LOG_HEAD, "exception: ", (&e)->what());
        }
        catch(...)
        {
            logWarn(ILMA, LOG_HEAD, "unexpected exception!");
        }
    }

    auto handle = broadcaster.Register(f);

    // register surface destroy callback for class Surface
    static bool isRegistered = false;
    if(!isRegistered)
    {
        isRegistered = true;
        Surface::RegisterSurfaceDestroyCB();
    }

    return handle;
}

void UnregisterObjExistanceNotification(unsigned long long handle)
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    broadcaster.Unregister(handle);

    if(broadcaster.GetSize()==0)
        UnregisterNotification();
}

bool IsRegistered(unsigned long long handle)
{
    FUNCTION_SCOPE_LOG_C(ILMA);

    return broadcaster.IsRegistered(handle);
}
