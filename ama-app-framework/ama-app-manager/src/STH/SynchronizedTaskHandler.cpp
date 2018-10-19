/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SynchronizedTaskHandler.cpp
/// @brief contains
///
/// Created by zs on 2017/03/11.
///

/// this file contains the implementation of class SynchronizedTaskHandler
///

#include <COMMON/UniqueID.h>
#include "AppShareManager.h"
#include "SynchronizedTaskHandler.h"
#include "HMICTask.h"
#include "NotifyWaiter.h"
#include "UIProcess.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

SynchronizedTaskHandler::SynchronizedTaskHandler()
{
    ;
}

SynchronizedTaskHandler::~SynchronizedTaskHandler()
{

}

void SynchronizedTaskHandler::doTask(shared_ptr<Task> task)
{
    auto pTask = dynamic_pointer_cast<STHTask>(task);

    if(!pTask)
    {
        logWarn(STH, "SynchronizedTaskHandler::doTask(): null task pointer!");
        return;
    }
//    // feed the dog
//    if (pTask->IsWatchDotTask()) {
//        wdSetState(E_TASK_ID_HMIC, WATCHDOG_SUCCESS);
//        return ;
//    }

    DomainVerboseLog chatter(STH, "SynchronizedTaskHandler::doTask()");

    // do the task
    switch(pTask->GetType())
    {
        case STH_START_SCREEN_SAHRE:
            OnStartScreenShare(pTask);
            break;
        case STH_STOP_SCREEN_SAHRE:
            OnStopScreenShare(pTask);
            break;
        default:
            break;
    };
}

void StartShareAppSuccess(AMPID app, E_PROC_STATE state, unsigned long long id)
{
    if(GET_APPID(app)==E_APPID_SYNC_APP && state==E_APP_STATE_SHOW)
        Notify(id);
}

void SynchronizedTaskHandler::OnStartScreenShare(shared_ptr<STHTask> task)
{
    DomainVerboseLog chatter(STH, "SynchronizedTaskHandler::OnStartScreenShare()");

    if(!task)
        return;

    auto screenShareParameter = reinterpret_cast<ScreenShareParameter*>(task->GetUserData());
    auto app = std::get<0>(*screenShareParameter);
    auto seat = std::get<1>(*screenShareParameter);
    auto isMove = std::get<2>(*screenShareParameter);
    //auto reply = std::get<3>(*screenShareParameter); // WARN: no relay function for start screen share
    delete screenShareParameter;
    screenShareParameter = nullptr;

    // if specified share is existing, return.
    auto appShareManager = AppShareManager::GetInstance();
    if(!appShareManager->GetContext([=](std::shared_ptr<AppShareContext> context)->bool{
        return context->sourceApp==app && context->destinationScreen==seat;
    }).empty())
    {
        return;
    }

    unsigned long long id = GetUniqueID();

    RegisterWaiterID(id);
    unsigned int callbackHandle = Process::appStateChangeBroadcaster.Register([id](AMPID app, E_PROC_STATE state){StartShareAppSuccess(app, state, id);});

    auto para = new std::tuple<E_SEAT, bool>(static_cast<E_SEAT>(seat), isMove);
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(app);
    hmicTask->SetType(HMIC_CMD_SYNC_APP_START);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(para));
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    if(!Wait(id, 1200))
        logWarn(CAPI, "OnStartScreenShare(): stop app share timeout!");

    Process::appStateChangeBroadcaster.Unregister(callbackHandle);
    UnregisterWaiterID(id);
}

void StopShareAppSuccess(AMPID app, E_PROC_STATE state, unsigned long long id)
{
    if(GET_APPID(app)==E_APPID_SYNC_APP && state==E_PROC_STATE_DEAD)
        Notify(id);
}

void SynchronizedTaskHandler::OnStopScreenShare(shared_ptr<STHTask> task)
{
    DomainVerboseLog chatter(STH, "SynchronizedTaskHandler::OnStopScreenShare()");

    if(!task)
        return;

    auto screenShareParameter = reinterpret_cast<ScreenShareParameter*>(task->GetUserData());
    auto app = std::get<0>(*screenShareParameter);
    auto seat = std::get<1>(*screenShareParameter);
    auto isMove = std::get<2>(*screenShareParameter);
    auto reply = std::get<3>(*screenShareParameter);
    delete screenShareParameter;
    screenShareParameter = nullptr;

    // if specified share is not existing, return.
    auto appShareManager = AppShareManager::GetInstance();
    if(appShareManager->GetContext([=](std::shared_ptr<AppShareContext> context)->bool{
        return context->sourceApp==app && context->destinationScreen==seat;
    }).empty())
    {
        if(reply)
            reply();
        return;
    }

    unsigned long long id = GetUniqueID();

    RegisterWaiterID(id);
    unsigned int callbackHandle = Process::appStateChangeBroadcaster.Register([id](AMPID app, E_PROC_STATE state){StopShareAppSuccess(app, state, id);});

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(app);
    hmicTask->SetType(HMIC_CMD_SYNC_APP_END);
    hmicTask->SetUserData(seat);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    if(!Wait(id, 1200))
        logWarn(CAPI, "StopShareAppReq(): stop app share timeout!");

    UIProcess::appStateChangeBroadcaster.Unregister(callbackHandle);
    UnregisterWaiterID(id);

    if(reply)
        reply();
}
