/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppSM.cpp
/// @brief contains the implementation of State Manager's APIs
///
/// Created by zs on 2016/8/4.
/// this file contains the implementation of State Manager's APIs
///

#include "AppBroadcast.h"
#include "StateManagerImpl.h"
#include "LogContext.h"

void sendBroadcast(std::string event,std::string data)
{
    logVerbose(SDK_AMGR,__FUNCTION__,"-->>IN");
    StateManagerImpl::GetInstance()->reqSendBroadcast(event,data);
    logVerbose(SDK_AMGR,__FUNCTION__,"-->>OUT");
}

void listenBroadcast(handleBroadcastFun handler)
{
    logVerbose(SDK_AMGR,__FUNCTION__,"-->>IN");
    StateManagerImpl::GetInstance()->registBroadcastCallback(handler);
    logVerbose(SDK_AMGR,__FUNCTION__,"-->>OUT");
}

void sendBroadcastForAutoTest(std::string event,std::string data)
{
    logVerbose(SDK_AMGR,__FUNCTION__,"-->>IN");
    event += string("autoTest.");
    StateManagerImpl::GetInstance()->reqSendBroadcast(event,data);
    logVerbose(SDK_AMGR,__FUNCTION__,"-->>OUT");
}
