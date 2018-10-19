/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Timeout.h
/// @brief contains class Timeout
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the definition of class Timeout
/// 

#ifndef TIMEOUT_H
#define TIMEOUT_H


#include <chrono>

class Timeout
{
public:
    Timeout();
    Timeout(const std::chrono::milliseconds &timeout);

    void StartTiming();
    void SetTimeout(const std::chrono::milliseconds &timeout);
    const std::chrono::milliseconds &GetTimeout() const;
    bool IsTimeout()const;

private:
    std::chrono::steady_clock::time_point start;
    std::chrono::milliseconds timeout;
};


#endif // TIMEOUT_H
