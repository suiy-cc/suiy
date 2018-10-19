/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SurfaceID.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/09/05.
///

#include "SurfaceID.h"

AMPID GetAMPID(const Surface& surface)
{
    return GET_AMPID(surface.GetID());
}

E_SEAT GetSeat(const Surface& surface)
{
    return GET_SEAT(GetAMPID(surface));
}

E_APPID GetAppID(const Surface& surface)
{
    return GET_APPID(GetAMPID(surface));
}

E_LAYER GetLayer(const Surface& surface)
{
    return GET_LAYER(surface.GetLayerID());
}