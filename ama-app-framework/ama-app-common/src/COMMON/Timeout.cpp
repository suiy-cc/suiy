/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Timeout.cpp
/// @brief contains class Timeout
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the implementation of class Timeout
///

#include "COMMON/Timeout.h"

Timeout::Timeout()
    : start()
    , timeout()
{}

Timeout::Timeout(const std::chrono::milliseconds &timeout)
    : start()
    , timeout(timeout)
{}

void Timeout::StartTiming()
{
    start = std::chrono::steady_clock::now();
}

void Timeout::SetTimeout(const std::chrono::milliseconds &timeout)
{
    Timeout::timeout = timeout;
}

const std::chrono::milliseconds &Timeout::GetTimeout() const
{
    return timeout;
}

bool Timeout::IsTimeout() const
{
    auto now = std::chrono::steady_clock::now();

    return now - start > timeout;
}
