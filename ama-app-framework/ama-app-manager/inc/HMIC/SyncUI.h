/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SyncUI.h
/// @brief contains class TitlebarUI
///
/// Created by zs on 2016/12/19.
/// this file contains the definition of class TitlebarUI
///

#ifndef SYNCUI_H
#define SYNCUI_H


#include "VideoLikeUI.h"

class SyncUI
: public VideoLikeUI
{
public:
    SyncUI();
    SyncUI(const Surface &surface);
    virtual ~SyncUI();
    virtual std::vector<Surface> GetSynchronizableSurfaces() const override;
    virtual void AddSurfaces2Layer() override;
    virtual void RemoveSurfacesFromLayer() override;
    virtual void SetVisible(const bool isVisible) override;
    virtual bool IsReady2Start() const override;
    virtual void RegisterSurface(Surface &surface) override;

protected:
    virtual void _ConfigSurface(Surface &surface, const Section &section) override;
    virtual void _AddSurfaces2ReleaseLayer()override;
    virtual void _RemoveSurfacesFromReleaseLayer()override;

    void DoIfStarted(std::function<void(void)> job);
    void InitializeSourceSurfaces();

private:
    bool isSourceSurfaceInitialized;
    std::vector<Surface> sourceSurfaces;

};


#endif // SYNCUI_H
