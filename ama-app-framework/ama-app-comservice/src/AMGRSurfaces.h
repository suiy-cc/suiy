/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AMGRSurfaces.h
/// @brief contains 
/// 
/// Created by zs on 2018/03/22.
///

#ifndef AMGRSURFACES_H
#define AMGRSURFACES_H

#include <vector>
#include <thread>
#include <memory>

#include <AppSDK.h>

typedef map<E_SEAT, std::shared_ptr<std::thread>> SurfaceHostMap;

class AMGRSurfaces{
public:
    AMGRSurfaces();
    virtual ~AMGRSurfaces();

    void CreateSurfaces();
    void DestroySurfaces();

private:
    SurfaceHostMap normalAppTouchForbidder;
    SurfaceHostMap childSafetyLockTF;
    SurfaceHostMap globalTouchForbidder;
    SurfaceHostMap background;
    SurfaceHostMap gestureRecognizer;
    SurfaceHostMap animationPlayer;

    std::shared_ptr<std::thread> mIVIMaskSurfaceThread;

};

#endif // AMGRSURFACES_H
