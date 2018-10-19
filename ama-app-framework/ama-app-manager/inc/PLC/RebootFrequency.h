/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file RebootFrequency.h
/// @brief contains 
/// 
/// Created by zs on 2018/05/02.
///
/// this file contains the definition of class RebootFrequency
/// 

#ifndef REBOOTFREQUENCY_H
#define REBOOTFREQUENCY_H

#include <unordered_map>
#include <chrono>

#include "ama_types.h"
#include "COMMON/ThreadSafeValueKeeper.h"

class RebootFrequency
{
public:
    static RebootFrequency * GetInstance();
    ~RebootFrequency();

    bool IsTooFrequent(AMPID app);
    void SetCrushed(AMPID app);

protected:
    RebootFrequency(); // singleton pattern
    void Refresh();

private:
    typedef std::unordered_multimap<AMPID, std::chrono::steady_clock::time_point> CrushTimeMap;
    typedef ThreadSafeValueKeeper<CrushTimeMap> TSCrushTimeMap;
    TSCrushTimeMap crushTime;

    const std::chrono::seconds timeout;
};


#endif // REBOOTFREQUENCY_H
