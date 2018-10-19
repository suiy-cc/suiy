/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TimeStamp.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/11/21.
///

/// this file contains the implementation of class TimeStamp
///

#include "COMMON/TimeStamp.h"

std::string TimeStamp::GetString()
{
    std::string timeStamp;
    std::string postPoint;
    postPoint.reserve(12);
    timespec ts = {0,0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    timeStamp = std::to_string(ts.tv_sec) + '.';
    postPoint = std::to_string(ts.tv_nsec);
    if(postPoint.size()<9)
        postPoint.insert(0, 9-postPoint.size(), '0');
    timeStamp += postPoint;

    return std::move(timeStamp);
}

unsigned long long TimeStamp::GetLong()
{
    timespec ts = {0,0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    unsigned long long timeStamp_low = ts.tv_nsec;
    unsigned long long timeStamp_high = ts.tv_sec;
    return (timeStamp_high << (sizeof(ts.tv_nsec)*8)) + timeStamp_low;
}
