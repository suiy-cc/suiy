/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SurfaceDestroyBroadcaster.h
/// @brief contains SurfaceDestroyBroadcaster
/// 
/// Created by zs on 2017/04/05.
///

#ifndef SURFACEDESTROYBROADCASTER_H
#define SURFACEDESTROYBROADCASTER_H

#include "Surface.h"
#include "Broadcaster.h"

typedef void(*SurfaceDestroyCallback)(const Surface& surface);
extern Broadcaster<SurfaceDestroyCallback> surfaceDestroyBroadcaster;

#endif // SURFACEDESTROYBROADCASTER_H
