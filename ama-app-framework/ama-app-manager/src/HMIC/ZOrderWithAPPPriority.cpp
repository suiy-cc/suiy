/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ZOrderProxyAPPPriority.cpp
/// @brief contains the implementation of class ZOrderProxyAPPPriority
///
/// Created by zs on 2016/6/30.
/// this file contains the implementation of class ZOrderProxyAPPPriority
///

#include "ZOrderWithAPPPriority.h"
#include "ID.h"

ZOrderWithAPPPriority::ZOrderWithAPPPriority()
{
    ;
}

ZOrderWithAPPPriority::~ZOrderWithAPPPriority()
{
    ;
}

AMPID ZOrderWithAPPPriority::GetTopApp()const
{
    if(zorder.empty())
        return INVALID_AMPID;
    else
    {
        for(auto iter = zorder.rbegin(); iter != zorder.rend(); ++iter)
            if(iter->second.GetSize()>0)
                return iter->second.GetTopApp();

        return INVALID_AMPID;
    }
}

void ZOrderWithAPPPriority::SetTopApp(AMPID app)
{
    RemoveApp(app);

    Priority priority = GetLayer(GET_APPID(app));
    if(priority!=E_LAYER_NONE)
        zorder[priority].SetTopApp(app);
}

void ZOrderWithAPPPriority::RemoveApp(AMPID app)
{
    for (auto &subZOrder: zorder)
        subZOrder.second.RemoveApp(app);
}

bool ZOrderWithAPPPriority::IsThere(AMPID app)const
{
    for(auto& subZOrder: zorder)
        if(subZOrder.second.IsThere(app))
            return true;

    return false;
}

unsigned int ZOrderWithAPPPriority::GetSize()const
{
    unsigned int total = 0;
    for(auto& subZOrder: zorder)
        total += subZOrder.second.GetSize();

    return total;
}

void ZOrderWithAPPPriority::Clear()
{
    for(auto& subZOrder: zorder)
        subZOrder.second.Clear();

    zorder.clear();
}

void ZOrderWithAPPPriority::Print(bool upSideDown)
{
    if(!upSideDown)
    {
        for(auto iter = zorder.rbegin(); iter!=zorder.rend(); ++iter)
            iter->second.Print(upSideDown);
    }
    else
    {
        for(auto iter = zorder.begin(); iter!=zorder.end(); ++iter)
            iter->second.Print(upSideDown);
    }
}