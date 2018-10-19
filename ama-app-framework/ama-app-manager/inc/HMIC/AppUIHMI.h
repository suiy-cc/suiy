/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUIHMI.h
/// @brief contains class AppUIHMI
///
/// Created by zs on 2016/6/20
/// this file contains the definition of class AppUIHMI
///

#ifndef APPUIHMI_H
#define APPUIHMI_H


#include <ILM/Screen.h>
#include "AppUI.h"

/// @class AppUIHMI
/// @brief this class derived from AppUI.
/// every method in this class would not
/// be called outside HMIC. Other modules
/// should even not include this header.
class AppUIHMI
: virtual public AppUI
, virtual public std::enable_shared_from_this<AppUIHMI>
{
public:
    /// @name constructors & destroyers
    /// @{
    AppUIHMI();
    explicit AppUIHMI(const Surface& surface);
    virtual ~AppUIHMI();
    /// @}

    /// @name getters & setters
    /// @{
    Surface GetResumeUI()const;
    void SetResumeUI(const Surface &surface);
    Surface GetStartUI()const;
    void SetStartUI(const Surface &surface);
    void SetResumeUIVisible(const bool isVisible = true);
    bool IsResumeUIVisible()const;
    void SetStartUIVisible(const bool isVisible = true);
    bool IsStartUIVisible()const;
    bool IsTitlebarVisible()const;
    void SetTitlebarVisible(bool isVisible = true);
    int GetTitlebarVisibilitySwitchStyle()const;
    void SetTitlebarVisibilitySwitchStyle(int style);
    bool IsTouchable()const;
    void SetTouchable(bool enable = true);
    virtual std::vector<Surface> GetSynchronizableSurfaces()const;
    /// @}

    /// @name operations
    /// @{
    virtual void SetVisible(const bool isVisible = true);
    virtual void SetOpacity(Alpha alpha);
    virtual bool IsOnLayer()const;
    virtual bool IsVisible()const;
    virtual void SetSourceRectangle(const Section& section);
    virtual void SetDestinationRectangle(const Section& section);
    virtual void SetOrientation(const Orientation orientation);
    virtual void TakeScreenShot(const std::string& path);
    virtual bool IsMainSurface(const Surface &surface)const;
    virtual bool IsReady2Start()const;
    virtual bool IsExpectedSurface(const Surface &surface) const;
    virtual void RegisterSurface(Surface &surface);
    virtual void UnregisterSurface(Surface &surface);
    virtual void AddSurfaces2Layer();
    virtual void RemoveSurfacesFromLayer();
    virtual void AddSurfaces2PreShowLayer();
    virtual void RemoveSurfacesFromPreShowLayer();
    virtual void AddSurfaces2PreHideLayer();
    virtual void RemoveSurfacesFromPreHideLayer();
    virtual void OnSurfaceAvailable(Surface &surface, shared_ptr<const Screen> screen);
    virtual void OnSurfaceCreate(Surface &surface);
    virtual void OnSurfaceDestroy(Surface &surface);
    virtual void ConfigureExistedSurfaces();
    /// @}

protected:
    virtual void _ConfigSurface(Surface &surface, const Section &section);
    virtual void _AddSurfaces2ReleaseLayer();
    virtual void _RemoveSurfacesFromReleaseLayer();
    virtual Section _GetRectangle()const;

private:
    bool isMainSurfaceCreated;
    bool isResumeUIVisible;
    bool isStartUIVisible;
    Surface resumeUI;
    Surface startUI;
    bool isTitlebarVisible;
    int titlebarVisibilitySwitchStyle;
    bool isTouchable;
};


#endif // APPUIHMI_H
