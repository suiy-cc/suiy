/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ProcessLifecycleManager.h
/// @brief contains class ProcessLifecycleManager
///
/// Created by zs on 2016/10/14.
/// this file contains the definition of class ProcessLifecycleManager
///

#ifndef PROCESSLIFECYCLEMANAGER_H
#define PROCESSLIFECYCLEMANAGER_H

#include <memory>

#include "FastBootManager.h"

using std::shared_ptr;

class PLCTask;

class ProcessLifecycleManager
{
    friend class PLController;
public:
    ProcessLifecycleManager();
    virtual ~ProcessLifecycleManager();

    virtual void OnMessage(std::shared_ptr<PLCTask> task);

    virtual void OnIVIShutdown(std::shared_ptr<PLCTask> task);
    virtual void OnAppShutdownOK(std::shared_ptr<PLCTask> task);
    virtual void OnReleaseFastBootManager(std::shared_ptr<PLCTask> task);
    virtual void OnFastBootAppStarted(std::shared_ptr<PLCTask> task);
    virtual void OnStartService(std::shared_ptr<PLCTask> task);
    virtual void OnQueryAppInfoList(std::shared_ptr<PLCTask> task);
    virtual void OnUpdateAppList(std::shared_ptr<PLCTask> task);

protected:
    static std::shared_ptr<FastBootManager> fastBootManager;
};


#endif // PROCESSLIFECYCLEMANAGER_H
