/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Surface.h
/// @brief contains the definition of class Surface
///
/// Created by zs on 2016/6/15.
/// this file contains the definition of class Surface
///

#ifndef SURFACE_H
#define SURFACE_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <ILM/HMIType.h>
#include <ILM/InputPeripheral.h>
#include <COMMON/ThreadSafeValueKeeper.h>
#include <COMMON/ComparableFunction.h>
#include <COMMON/Broadcaster.h>

using std::shared_ptr;

typedef surfaceNotificationFunc SurfaceNotifyCallback;

/// @class Surface
/// @brief the class is an abstraction of surface in ILM
class Surface
{
public:
    typedef Broadcaster<void(SurfaceID, SurfaceProperties*, NotificationMask)> CallbackSet;
    typedef std::unordered_map<SurfaceID, CallbackSet> CallbackMap;
    typedef std::unordered_set<SurfaceID> SurfaceIDSet;

    /// @name constructors & destructors
    /// @{
    Surface();
    explicit Surface(SurfaceID ID);
    ~Surface();
    /// @}

    /// @{
    void Create(const Dimension dimension, const WindowHandle handle, const PixelFormat& pixelFormat);
    void Destroy();
    /// @}

    /// @name relational operations
    /// @{
    bool operator==(const Surface& surface)const;
    bool operator!=(const Surface& surface)const;
    bool operator<(const Surface &rhs) const;
    bool operator>(const Surface &rhs) const;
    bool operator<=(const Surface &rhs) const;
    bool operator>=(const Surface &rhs) const;
    /// @}

    /// @name operations
    /// @{
    void SetVisible(const bool isVisible = true);
    bool IsVisible()const;
    void SetOpacity(const Alpha alpha);
    Alpha GetOpacity()const;
    unsigned long long SetNotifyCallback(const std::function<void(SurfaceID, SurfaceProperties*, NotificationMask)>& function, bool forceCreate = false);
    void RemoveNotifyCallback(unsigned long long handle);
    shared_ptr<SurfaceProperties> GetProperties()const;
    void SetSourceRectangle(const Section& section);
    Section GetSourceRectangle()const;
    void SetDestinationRectangle(const Section& section);
    Section GetDestinationRectangle()const;
    void SetBothRectangle(const Section& section);
    Dimension GetDimension()const;
    void SetOrientation(const Orientation orientation);
    PixelFormat GetPixelFormat()const;
    void TakeScreenShot(const std::string& path);
    LayerID GetLayerID()const;
    int GetCreatorPID()const;
    LayerID GetContainerLayer()const;
    bool IsExist()const;
    void SetConfiguration(const Size& size);
    bool IsConfigured()const;
    bool Wait4SurfaceCongfigued(unsigned int timeout = 300);
    void SetInputDevice(const InputPeripheral& input);
    std::vector<InputPeripheral> GetInputDevice()const;
    /// @}

    /// @name getters & setters
    /// @{
    SurfaceID GetID()const;
    void SetID(const SurfaceID ID);
    /// @}

    friend unsigned long long RegisterObjExistanceNotification(std::function<void(ilmObjectType objType, unsigned int id, bool create)> f);
    friend void SurfaceCallback(SurfaceID surface, SurfaceProperties*, NotificationMask mask);

private:
    void _GetProperties(SurfaceProperties *surfaceProperties)const;
    void _SetNotifyCallback(const SurfaceNotifyCallback& function);
    void _RemoveNotifyCallback();
    static void RegisterSurfaceDestroyCB();

private:
    SurfaceID surfaceID;

    static ThreadSafeValueKeeper<CallbackMap> callbackKeeper;
    static unsigned long long objExistanceCBHandle;
};

#define INVALID_SURFACE Surface(0)

int GetSurfaceCount();
std::vector<Surface> GetAllSurfaces();

namespace std
{
template <>
struct hash<Surface>
{
    std::size_t operator()(const Surface &key) const
    {
        return key.GetID();
    }
};
}

#endif // SURFACE_H
