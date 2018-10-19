/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveUIService.cpp
/// @brief contains the implementation of class ActiveUIService
///
/// Created by zs on 2016/10/13.
/// this file contains the implementation of class ActiveUIService
///

#include "ActiveUIService.h"
#include "AlwaysVisibleServices.h"
#include "AppList.h"
#include "PLCTask.h"
bool ActiveUIService::IsAlwaysVisible()
{
    return IsAlwaysVisibleService(AppList::GetInstance()->GetItemID(GetAMPID()));
}


bool ActiveUIService::IsShowStartupScreen()const
{
    DomainVerboseLog chatter(PLC, "ActiveUIService::IsShowStartupScreen()");

    logInfo(PLC, "ActiveUIService::IsShowStartupScreen(): UI service never shows startup screen.");
    return false;
}