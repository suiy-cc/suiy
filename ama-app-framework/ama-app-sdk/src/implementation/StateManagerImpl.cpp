/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "StateManagerImpl.h"
#include "log.h"

#include <v0_1/org/neusoft/AppMgrProxy.hpp>

extern std::shared_ptr<v0_1::org::neusoft::AppMgrProxy<>> appManager;
LOG_DECLARE_CONTEXT(SDK_SM);

StateManagerImpl::StateManagerImpl()
:mIsInited(false)
,mBroadcastCallback(nullptr)
{
    RegisterContext(SDK_SM, "SSM");
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>IN");

    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>OUT");
}

StateManagerImpl::~StateManagerImpl()
{
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>IN");
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>OUT");
}

void StateManagerImpl::init(void)
{
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>IN");

    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>OUT");
}

void StateManagerImpl::reqSendBroadcast(std::string event,std::string data)
{
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>IN");
    logVerbose(SDK_SM,"event:",event," data:",data);

    CommonAPI::CallStatus callStatus;

	//async fire&forget
    if(appManager){
        appManager->SM_SetState(BROADCASTFLAG,event,data,callStatus);
    	if (callStatus != CommonAPI::CallStatus::SUCCESS)
    	{
    		logError(SDK_SM,"CommonAPI Remote call failed!");
    	}
    }

    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>OUT");
}

void StateManagerImpl::registBroadcastCallback(handleBroadcastFun f)
{
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>IN");

    mBroadcastCallback = f;

    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>OUT");
}

void StateManagerImpl::handleBroadcast(std::string event,std::string data)
{
    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>IN");

    if(mBroadcastCallback){
        mBroadcastCallback(event,data);
    }

    logVerbose(SDK_SM,"StateManagerImpl::",__FUNCTION__,"-->>OUT");
}
