/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FastBootManager.h
/// @brief contains 
/// 
/// Created by zs on 2018/04/14.
///

/// this file contains the definition of class FastBootManager
/// 

#ifndef FASTBOOTMANAGER_H
#define FASTBOOTMANAGER_H

#include <memory>
#include <set>

#include "PLCTask.h"

class FastBootManager
: public std::enable_shared_from_this<FastBootManager>
{
public:
    FastBootManager();
    ~FastBootManager();
    bool IsReady2Start(AMPID app);
    void DelayStartTask(std::shared_ptr<PLCTask> task);
    void ResendStartTasks();
    void StartTiming();
    bool IsFastBootCompleted() const;
    void SetAppStarted(AMPID app);
    void RegisterCallback();

protected:
    bool _IsFastBootCompleted() const;
    static void _SendFastBootDoneNotify();

private:
    std::vector<std::shared_ptr<Task>> delayedTasks;
    std::set<AMPID> startedApps;
    std::set<AMPID> lastSourceSet;
    mutable std::mutex mutex1;
    unsigned int appStateChangeHandle;
};


#endif // FASTBOOTMANAGER_H
