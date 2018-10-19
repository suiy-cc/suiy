/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ReleaseLayer.h
/// @brief contains class ReleaseLayer
/// 
/// Created by zs on 2017/11/27.
///
/// this file contains the definition of class ReleaseLayer
/// 

#ifndef RELEASELAYER_H
#define RELEASELAYER_H

#include <map>

#include "ILM/Layer.h"
#include "COMMON/ThreadSafeValueKeeper.h"

typedef std::map<Surface, Section> RectCache;
typedef std::map<Surface, unsigned long long> HandleMap;

class ReleaseLayer
:public Layer
{
public:
    ReleaseLayer();
    ReleaseLayer(LayerID);

    virtual void AddSurface(const Surface &surface) override;
    virtual void RemoveSurface(const Surface &surface) override;


    void FixSurfaceDestinationRectangle(Surface &surface);
    static void DeleteRectCache(const Surface &surface);

protected:
    Point GetNextPos();
    void TakePos(const Surface &surface, unsigned int pos);
    static void OnSurfacePropertiesChange(SurfaceID, SurfaceProperties*, NotificationMask);
    void CacheSurfaceDestinationRectangle(const Surface& surface);
    void FitSurface2ItsSpot(Surface& surface);
    void RegisterObjCreateNotify();
    bool IsAlreadyOn(const Surface& surface)const;

private:
    // static variable
    static ThreadSafeValueKeeper<RectCache> rectCache;
    static const unsigned int maxSurfacePos;
    static const unsigned int height;
    static const unsigned int width;
    static unsigned int objExistanceCBHandle;
    static ThreadSafeValueKeeper<HandleMap> surfaceCBHandles;
};


#endif // RELEASELAYER_H
