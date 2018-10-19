/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUI.cpp
/// @brief contains the implementation of class AppUI
///
/// Created by zs on 2016/6/20.
/// this file contains the implementation of class AppUI
///

#include <algorithm>
#include <cassert>
#include "AppUI.h"

AppUI::AppUI()
{
    ;
}

AppUI::~AppUI()
{
    ;
}

void AppUI::AddSurface(const Surface& surface)
{
    surfaces.push_back(surface);
}

void AppUI::RemoveSurface(const Surface& surface)
{
    surfaces.erase(std::remove(surfaces.begin(), surfaces.end(), surface), surfaces.end());
}

bool AppUI::IsInThisApp(const Surface& surface)const
{
    return std::find(surfaces.begin(), surfaces.end(), surface) != surfaces.end();
}

std::vector<Surface> AppUI::GetSurfaces()const
{
    auto copy = surfaces;
    return std::move(copy);
}

E_SEAT AppUI::GetSeat()const
{
    if(surfaces.empty())
    {
        assert(false);
        return E_SEAT_NONE;
    }
    else
        return ::GetSeat(surfaces.front());
}

AMPID AppUI::GetAMPID()const
{
    if(surfaces.empty())
    {
        assert(false);
        return 0;
    }
    else
        return ::GetAMPID(surfaces.front());
}

LayerID AppUI::GetLayerID()const
{
    if(surfaces.empty())
    {
        assert(false);
        return 0;
    }
    else
        return surfaces.front().GetLayerID();
}

bool AppUI::IsTheSame(const shared_ptr<const AppUI> UI) const
{
    if(!this || !UI)
        return false;
    if(surfaces.size() != UI->surfaces.size())
        return false;

    for(int i=0; i<surfaces.size(); ++i)
        if(surfaces[i] != UI->surfaces[i])
            return false;

    return true;
}