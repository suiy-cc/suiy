/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file UniqueID.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/08/30.
///
/// this file contains the implementation of class UniqueID
///

#include "COMMON/UniqueID.h"
#include "COMMON/ThreadSafeValueKeeper.h"

static ThreadSafeValueKeeper<UniqueID> globalID;
extern "C" unsigned long long GetUniqueID()
{
    unsigned long long id = 0;
    globalID.Operate([&id](UniqueID& uniqueID){
        id = uniqueID.GetNewID();
    });
    return id;
}

unsigned long long UniqueID::GetNewID()
{
    id = id + 1;
    if(id==0)
        id = 1;

    return id;
}
