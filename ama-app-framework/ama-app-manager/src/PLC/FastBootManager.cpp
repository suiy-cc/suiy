/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FastBootManager.cpp
/// @brief contains
///
/// Created by zs on 2018/04/14.
///

/// this file contains the implementation of class FastBootManager
///

#include <PLC/UnitManager.h>
#include "TASK/TaskDispatcher.h"
#include "FastBootManager.h"
#include "AppList.h"
#include "UIProcess.h"

FastBootManager::FastBootManager()
: appStateChangeHandle(0)
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    // get a copy of fast boot list, so that the list's instant change
    // won't affect the flow of fast boot.
    auto apps = AppList::GetInstance()->fastBootList.GetFastBootApps();

    for(auto& app: apps)
        lastSourceSet.insert(app.getAMPID());
}

FastBootManager::~FastBootManager()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    Process::appStateChangeBroadcaster.Unregister(appStateChangeHandle);
}

bool FastBootManager::IsReady2Start(AMPID app)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", app));
    std::unique_lock<std::mutex> lock(mutex1);

    if(lastSourceSet.empty())
        return true;

    return lastSourceSet.find(app)!=lastSourceSet.end();
}

void FastBootManager::DelayStartTask(std::shared_ptr<PLCTask> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    delayedTasks.push_back(task);
}

void FastBootManager::ResendStartTasks()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    logDebug(PLC, LOG_HEAD, "total delayed tasks: ", delayedTasks.size());
    for(auto& task: delayedTasks)
        TaskDispatcher::GetInstance()->Dispatch(task);
}

void FastBootManager::StartTiming()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    std::weak_ptr<FastBootManager> _this = shared_from_this();
    std::thread thread1([_this](){
        try
        {
            int timeout = 9;
            std::this_thread::sleep_for(std::chrono::seconds(timeout));

            auto shared_this = _this.lock();
            if(shared_this && shared_this->IsFastBootCompleted())
            {
                logDebug(PLC, LOG_HEAD, "fast boot is fisished!");
                return;
            }

            // fast boot manager is already released
            if(!shared_this)
            {
                logDebug(PLC, LOG_HEAD, "fast boot manager is already released!");
                return;
            }

            logInfo(PLC, LOG_HEAD, "fast boot is timeout!");
            _SendFastBootDoneNotify();

        }
        catch(std::exception& e)
        {
            logError(PLC, LOG_HEAD, "exception is caught: ", (&e)->what());
        }
        catch(...)
        {
            logError(PLC, LOG_HEAD, "unexpected exception is caught!");
        }
    });

    thread1.detach();
}

bool FastBootManager::_IsFastBootCompleted() const
{
    for(const auto& app: lastSourceSet)
        if(startedApps.find(app)==startedApps.end())
        {
            logInfo(PLC, LOG_HEAD, "app ", ToHEX(app), " is not started yet!");
            return false;
        }

    return true;
}

bool FastBootManager::IsFastBootCompleted() const
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    return _IsFastBootCompleted();
}

void FastBootManager::SetAppStarted(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex1);
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", app));

    startedApps.insert(app);

    if(_IsFastBootCompleted())
        _SendFastBootDoneNotify();
}

void FastBootManager::RegisterCallback()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex1);

    std::weak_ptr<FastBootManager> _this = shared_from_this();

    appStateChangeHandle = Process::appStateChangeBroadcaster.Register([_this](AMPID app, E_PROC_STATE state){
        if(state!=E_APP_STATE_SHOW && state!=E_APP_STATE_HIDE)
            return;

        DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x, %d)", app, state));
        auto plcTask = std::make_shared<PLCTask>();
        plcTask->SetType(PLC_NOTIFY_FAST_BOOT_APP_STARTED);
        plcTask->SetAMPID(app);
        TaskDispatcher::GetInstance()->Dispatch(plcTask);
    });
}

void FastBootManager::_SendFastBootDoneNotify()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetType(PLC_CMD_RELEASE_FAST_BOOT_MANAGER);
    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}