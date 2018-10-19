/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HMIType.h
/// @brief contains useful types used by HMIC module
///
/// Created by zs on 2016/6/15.
///
///

#ifndef HMITYPE_H
#define HMITYPE_H

#include <ilm_types.h>

typedef ilmErrorTypes ErrorType;

typedef t_ilm_layercapabilities Capability;

typedef t_ilm_int Color;

typedef ilmOrientation Orientation;

typedef t_ilm_float Alpha;

typedef ilmLayerType LayerType;

typedef ilmScreenExtendedProperties ScreenExtendedProperties;

typedef ilmScreenProperties ScreenProperties;

typedef ilmLayerProperties LayerProperties;

typedef ilmSurfaceProperties SurfaceProperties;

typedef t_ilm_notification_mask NotificationMask;

typedef t_ilm_nativehandle WindowHandle;

typedef ilmPixelFormat PixelFormat;

typedef t_ilm_uint ScreenID;

typedef t_ilm_display Display;

typedef t_ilm_layer LayerID;

typedef t_ilm_surface SurfaceID;

struct Point
{
    unsigned int x;
    unsigned int y;
};

struct Size
{
    unsigned int width;
    unsigned int height;
};

struct Section
{
    int left;
    int top;
    unsigned int width;
    unsigned int height;
};

static bool operator==(const Section& section1, const Section& section2){
    return section1.left==section2.left
        && section1.top==section2.top
        && section1.width==section2.width
        && section1.height==section2.height;
};

static bool operator!=(const Section& section1, const Section& section2){
    return !(section1==section2);
};

typedef Size Dimension;

typedef Size Resolution;

typedef Point Position;

#endif // HMITYPE_H
