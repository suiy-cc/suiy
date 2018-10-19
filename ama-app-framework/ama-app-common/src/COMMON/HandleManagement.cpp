/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HandleManagement.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/02/23.
///

#include <map>
#include <memory.h>

#include "COMMON/HandleManagement.h"
#include "COMMON/ThreadSafeValueKeeper.h"

static HandleManager<void*> defaultHM;

extern "C" unsigned int GetMinAvailableHandle()
{
    return defaultHM.GetMinAvailableHandle();
}

extern "C" void CreateHandle(unsigned int handle)
{
    defaultHM.CreateHandle(handle);
}

extern "C" bool IsHandleExists(unsigned int handle)
{
    return defaultHM.IsHandleExists(handle);
}

extern "C" void AssociateObject2Handle(unsigned int handle, void* obj)
{
    defaultHM.AssociateObject2Handle(handle, obj);
}

extern "C" void* GetObjectFromHandle(unsigned int handle)
{
    if(!defaultHM.IsHandleExists(handle))
        return nullptr;
    else
        return defaultHM.GetObjectFromHandle(handle);
}

extern "C" void DeleteHandle(unsigned int handle)
{
    defaultHM.DeleteHandle(handle);
}
