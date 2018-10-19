/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveAppHome.cpp
/// @brief contains the implementation of class ActiveProcess' factory method
///
/// Created by zs on 2016/10/13.
/// this file contains the implementation of class ActiveProcess' factory method
///

#include "ActiveAppHome.h"
#include "ID.h"
#include "ProcessFactory.h"
#include "AppUIFactory.h"

shared_ptr<ActiveApp> GetAppObject(AMPID app)
{
    logVerbose(PLC, "GetAppObject(", ToHEX(app), ")--->IN");

    auto newAppUI = GetUIObject(app);
    shared_ptr<ActiveApp> newAppObj = nullptr;
    if(GET_APPID(app)==E_APPID_HOME)
        newAppObj = make_shared<ActiveAppHome>(app);
    else
        newAppObj = make_shared<ActiveApp>(app);
    newAppObj->SetUI(newAppUI);
    newAppObj->SetAMPID(app);

    logVerbose(PLC, "GetAppObject(", ToHEX(app), ")--->OUT");
    return newAppObj;
}

shared_ptr<ActiveUIService> GetUIServiceObject(AMPID proc)
{
    logVerbose(PLC, "GetUIServiceObject(", ToHEX(proc), ")--->IN");

    auto ui = GetUIObject(proc);
    auto serviceObj = make_shared<ActiveUIService>();
    serviceObj->SetUI(ui);
    serviceObj->SetAMPID(proc);

    logVerbose(PLC, "GetUIServiceObject(", ToHEX(proc), ")--->OUT");
    return serviceObj;
}

shared_ptr<ActiveService> GetCMDServiceObject(AMPID proc)
{
    DomainVerboseLog chatter(PLC, formatText("GetCMDServiceObject(%#x)", proc));

    auto serviceObj = std::make_shared<ActiveService>();
    serviceObj->SetAMPID(proc);

    return serviceObj;
}