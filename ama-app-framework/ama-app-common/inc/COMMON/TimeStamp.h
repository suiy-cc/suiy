/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TimeStamp.h
/// @brief contains class TimeStamp
/// 
/// Created by zs on 2017/11/21.
///
/// this file contains the definition of class TimeStamp
/// 

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <string>

class TimeStamp
{
public:
    static std::string GetString();
    static unsigned long long GetLong();
};


#endif // TIMESTAMP_H
