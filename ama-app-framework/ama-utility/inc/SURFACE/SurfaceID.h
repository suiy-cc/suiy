/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SurfaceID.h
/// @brief contains 
/// 
/// Created by zs on 2017/09/05.
///

#ifndef SURFACEID_H
#define SURFACEID_H

#include "ama_types.h"
#include "Surface.h"

AMPID GetAMPID(const Surface& surface);
E_SEAT GetSeat(const Surface& surface);
E_APPID GetAppID(const Surface& surface);
E_LAYER GetLayer(const Surface& surface);

#endif // SURFACEID_H
