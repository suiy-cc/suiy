/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ZOrder.cpp
/// @brief contains the implementation of class ZOrder
///
/// Created by zs on 2016/6/30.
/// this file contains the implementation of class ZOrder
///

#include <algorithm>
#include <iostream>

#include "ZOrder.h"

ZOrder::ZOrder()
{
    ;
}

ZOrder::~ZOrder()
{
    ;
}

AMPID ZOrder::GetTopApp()const
{
    if(apps.empty())
        return INVALID_AMPID;
    else
        return apps.back();
}

void ZOrder::SetTopApp(AMPID app)
{
    // take appUI off the list
    RemoveApp(app);
    // put it back on the top of the list
    apps.push_back(app);
}

void ZOrder::RemoveApp(AMPID app)
{
    apps.remove_if([app](AMPID ampid)->bool{return ampid==app;});
}

bool ZOrder::IsThere(AMPID app)const
{
    return std::find_if(apps.begin(), apps.end(), [app](const AMPID ampid)->bool{return ampid==app;})!=apps.end();
}

unsigned int ZOrder::GetSize()const
{
    return apps.size();
}

void ZOrder::Clear()
{
    apps.clear();
}

void ZOrder::Print(bool upSideDown)
{
    if(!upSideDown)
    {
        for(auto iter= apps.rbegin(); iter != apps.rend(); ++iter)
            std::cout << *iter << std::endl;
    }
    else
    {
        for(auto iter= apps.begin(); iter != apps.end(); ++iter)
            std::cout << *iter << std::endl;
    }
}