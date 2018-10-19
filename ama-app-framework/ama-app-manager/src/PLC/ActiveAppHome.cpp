/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveAppHome.cpp
/// @brief contains the implementation of class ActiveAppHome
///
/// Created by zs on 2016/10/13.
/// this file contains the implementation of class ActiveAppHome
///

#include "Capi.h"
#include "ActiveAppHome.h"
#include "SplashLOGO.h"
#include "ID.h"

void ActiveAppHome::ShowNotify()
{
    UIProcess::ShowNotify();
    HomeReadyNotify();
}

void ActiveAppHome::InstalledPKGsChangedNotify()
{
    DomainVerboseLog chatter(PLC, "ActiveAppHome::InstalledPKGsChangedNotify()");
    ::InstalledPKGsChangedNotify(GetAMPID());
}

void ActiveAppHome::HomeReadyNotify()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    SplashLOGO::GetInstance()->NotifyHomeReady(GetAMPID());

    auto seat = GET_SEAT(GetAMPID());
    if(seat==E_SEAT_RSE1 || seat==E_SEAT_RSE2)
    {
        std::string log = "echo \"rear seat " + std::to_string(seat) + " is operational\" > /dev/kmsg";
        system(log.c_str());
    }
}

bool ActiveAppHome::IsShowStartupScreen() const
{
    DomainVerboseLog chatter(PLC, "ActiveAppHome::IsShowStartupScreen()");

    logInfo(PLC, "ActiveAppHome::IsShowStartupScreen(): home never shows startup screen.");
    return false;
}
