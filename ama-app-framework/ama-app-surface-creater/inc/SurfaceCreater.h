/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SurfaceCreater.h
/// @brief contains 
/// 
/// Created by zs on 2017/02/21.
///

#ifndef SURFACECREATER_H
#define SURFACECREATER_H

#ifdef __cplusplus
extern "C" {
#endif

enum E_TOUCH{
    E_TOUCH_DOWN,
    E_TOUCH_UP,
    E_TOUCH_MOVE,
};

typedef void (*RedrawCallback)(unsigned int* pixels, unsigned int width, unsigned int height);

typedef void (*TouchCallback)(unsigned int surfaceID, enum E_TOUCH touch_type, float x, float y, int finger_id, unsigned int seat_id);

void CreateSurface(unsigned int surfaceID, unsigned int width, unsigned int height, RedrawCallback callback, TouchCallback tCallback);

void DestroySurface(unsigned int surfaceID);

void ForceRedrawSurface(unsigned int surfaceID);

void Transparent(unsigned int* pixels, unsigned int width, unsigned int height);

void Black(unsigned int* pixels, unsigned int width, unsigned int height);

#ifdef __cplusplus
};
#endif

#endif // SURFACECREATER_H
