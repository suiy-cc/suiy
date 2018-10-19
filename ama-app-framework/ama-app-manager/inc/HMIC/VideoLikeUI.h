/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file VideoUI.h
/// @brief contains class VideoUI
///
/// Created by zs on 2016/8/23.
/// this file contains the definition of class VideoUI
///

#ifndef VIDEOUI_H
#define VIDEOUI_H

#include "AppUIHMI.h"

class VideoLikeUI
: public AppUIHMI
{
public:
    VideoLikeUI();
    VideoLikeUI(const Surface &surface);
    virtual ~VideoLikeUI();
    virtual void OnSurfaceCreate(Surface &surface)override;
    virtual void OnSurfaceDestroy(Surface &surface)override;
    virtual void SetVisible(const bool isVisible)override;
    virtual bool IsExpectedSurface(const Surface &surface) const override;
    virtual void RegisterSurface(Surface &surface) override;
    virtual void UnregisterSurface(Surface &surface) override;
    virtual void AddSurfaces2Layer()override;
    virtual void RemoveSurfacesFromLayer()override;

    virtual std::vector<Surface> GetSynchronizableSurfaces()const override;

    // unique interfaces
    virtual void SetUnderLayerCenter(bool isCenter = true);
    virtual bool IsUnderLayerCenter()const;

protected:
    virtual void _ConfigSurface(Surface &surface, const Section &section)override;
    virtual void _AddSurfaces2ReleaseLayer()override;
    virtual void _RemoveSurfacesFromReleaseLayer()override;

private:
    Surface playWindow;
    Surface backgroundWindow;
    bool isCenterUnderLayerSurface;
};


#endif //AMA_APP_FRAMEWORK_VIDEOUIHMI_H
