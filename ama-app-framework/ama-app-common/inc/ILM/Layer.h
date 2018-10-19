/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Layer.h
/// @brief contains the definition of class Layer
///
/// Created by zs on 2016/6/15.
/// this file contains the definition of class Layer
///

#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <functional>

#include <ILM/Surface.h>

typedef layerNotificationFunc LayerNotifyCallback;

/// @class Layer
/// @brief this class is an abstraction of layer in ILM
class Layer
{
public:
    /// @name constructors & destructors
    /// @{
    Layer();
    explicit  Layer(LayerID);
    ~Layer();
    /// @}

    bool operator<(const Layer& otherLayer)const;
    bool operator==(const Layer& otherLayer)const;
    bool operator!=(const Layer& otherLayer)const;

    /// @{
    void Create(const Dimension &dimension);
    void Destroy();
    /// @}

    /// @name operations
    /// @{
    virtual void AddSurface(const Surface& surface);
    virtual void RemoveSurface(const Surface& surface);
    shared_ptr<LayerProperties> GetProperties();
    int GetSurfaceCount();
    std::vector<Surface> GetAllSurfaces();
    LayerType GetLayerType();
    void SetVisible(const bool isVisible = true);
    bool IsVisible();
    void SetOpacity(const Alpha alpha);
    Alpha GetOpacity();
    void SetSourceRectangle(const Section& section);
    void SetDestinationRectangle(const Section& section);
    Dimension GetDimension()const;
    Section GetRectangle()const;
    void SetOrientation(const Orientation orientation);
    Orientation GetOrientation();
    void SetRelativeRenderOrder(const std::vector<Surface>& surfaces);
    void SetRenderOrder(const std::vector<Surface>& surfaces);
    void TakeScreenShot(const std::string& path);
    bool SetNotifyCallback(const LayerNotifyCallback& function);
    void RemoveNotifyCallback();
    /// @}

    /// @name getters & setters
    /// @{
    LayerID GetID()const;
    void SetID(const LayerID ID);
    /// @}

private:
    void _GetProperties(LayerProperties *layerProperties)const;

private:
    LayerID layerID;
};

int GetLayerCount();
std::vector<Layer> GetAllLayers();
typedef notificationFunc NotifyFunc;
void RegisterNotification(NotifyFunc func, void* userData);
void UnregisterNotification();

typedef std::function<void(ilmObjectType objType, unsigned int id, bool create)> ILMObjExistanceNotifyFunc;
unsigned long long RegisterObjExistanceNotification(ILMObjExistanceNotifyFunc f);
void UnregisterObjExistanceNotification(unsigned long long handle);
bool IsRegistered(unsigned long long handle);

#endif // LAYER_H
