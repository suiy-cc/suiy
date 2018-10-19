/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ILMClient.cpp
/// @brief contains the implementation of class ILMClient
///
/// Created by zs on 2016/6/15.
/// this file contains the implementation of class ILMClient
///

#include <ilm_common.h>
#include <unistd.h>
#include <sys/time.h>

#include "ILM/ILMClient.h"
#include "ILM/ILMException.h"

#include "log.h"

extern LOG_DECLARE_CONTEXT(SURF);
extern LOG_DECLARE_CONTEXT(SCRN);
extern LOG_DECLARE_CONTEXT(VTSM);
LOG_DECLARE_CONTEXT(ILMA);

bool ILMClient::isInputPeripheralChangedCBRegistered = false;

void ILMClient::Initialize()
{
    struct timespec   startTime;
    struct timespec   endTime;
    long start = 0;
    long end = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW,&startTime);


    ilmErrorTypes errorTypes = ilm_init();

    clock_gettime(CLOCK_MONOTONIC_RAW,&endTime);
    end=endTime.tv_sec*1000+endTime.tv_nsec/1000;
    start=startTime.tv_sec*1000+startTime.tv_nsec/1000;
    if (start>end) end=end+1000000;//一般单个函数执行时间不会超过1s 所以在此做简单加1s操作 并且可以根据dlt的时间戳做验证
    logInfo("ilm_init  cost: ",(end-start),"ns , end ",end,"start ",start);//输出时间为纳秒


     //TODO : need to redesign
    int retry = (10 * 1000) / 50;
    while((errorTypes != ILM_SUCCESS) && (retry > 0)) {
        errorTypes = ilm_init();
        usleep(50*1000);
        retry--;
    }

    logInfo("wait for weston ",((200-retry)*50)," ms");

    // register surface's log context
    RegisterContext(SURF, "SURF");
    // register screen's log context
    RegisterContext(SCRN, "SCRN");
    // register virtual screen manager's log context
    RegisterContext(VTSM, "VTSM");
    // register log context of ILM's abstraction
    // TODO: this log context will replace all other contexts above
    RegisterContext(ILMA, "ILMA");

    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "ILMClient::Initialize()");
        throw e;
    }

    isInputPeripheralChangedCBRegistered = false;
}

void ILMClient::Destroy()
{
    // unregister surface's log context
    UnregisterContext(SURF);
    // unregister screen's log context
    UnregisterContext(SCRN);
    // unregister virtual screen manager's log context
    UnregisterContext(VTSM);

    ilmErrorTypes errorTypes = ilm_destroy();
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "ILMClient::Destroy()");
        throw e;
    }

    isInputPeripheralChangedCBRegistered = false;
}

bool ILMClient::IsInitialized()
{
    return ilm_isInitialized();
}

void ILMClient::Commit()
{
    ilmErrorTypes errorTypes = ilm_commitChanges();
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "ILMClient::Commit()");
        throw e;
    }
}

static ILMClient::FatalErrorCB fatalErrorCB = nullptr;

static void rawFatalErrorCB(ilmExceptionTypes exception, void* usrData)
{
    if(fatalErrorCB)
        fatalErrorCB(exception);
}

void ILMClient::SetFatalErrorCB(const FatalErrorCB& f)
{
    fatalErrorCB = f;

    ilmErrorTypes error = ilm_registerException(rawFatalErrorCB, nullptr);
    if(error!=ILM_SUCCESS)
    {
        ILMException e(error, "ILMClient::SetFatalErrorCB()");
        throw e;
    }
}