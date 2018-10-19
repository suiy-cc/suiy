/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file RebootFrequency.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/05/02.
///
/// this file contains the implementation of class RebootFrequency
///

#include "RebootFrequency.h"

RebootFrequency * RebootFrequency::GetInstance()
{
    static std::mutex mutex1;
    std::unique_lock<std::mutex> lock1(mutex1);

    static RebootFrequency instance;

    return &instance;
}

RebootFrequency::RebootFrequency()
: timeout(60)
{

}

RebootFrequency::~RebootFrequency()
{

}

bool RebootFrequency::IsTooFrequent(AMPID app)
{
    Refresh();

    bool rtv = false;

    crushTime.Operate([app, &rtv](CrushTimeMap& crushTimeMap){
        rtv = (crushTimeMap.count(app) > 1);
    });

    return rtv;
}

void RebootFrequency::SetCrushed(AMPID app)
{
    Refresh();

    crushTime.Operate([app](CrushTimeMap& crushTimeMap){
        auto now = std::chrono::steady_clock::now();
        crushTimeMap.insert(std::make_pair(app, now));
    });
}

void RebootFrequency::Refresh()
{
    crushTime.Operate([this](CrushTimeMap& crushTimeMap){

        auto now = std::chrono::steady_clock::now();

        CrushTimeMap newCrushTime;

        for(auto& record: crushTimeMap)
            if(now - record.second < timeout)
            {
                newCrushTime.insert(record);
            }

        crushTimeMap.swap(newCrushTime);
    });
}
