/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveService.cpp
/// @brief contains the implementation of class ActiveService
///
/// Created by zs on 2016/10/13.
/// this file contains the implementation of class ActiveService
///

#include "ActiveService.h"
#include "UnitManager.h"

void ActiveService::Stop()
{
    DomainVerboseLog chatter(PLC, formatText("ActiveService::Stop(%#x)", GetAMPID()));

    UnitManager::GetInstance()->StopUnit(GetAMPID());
}
