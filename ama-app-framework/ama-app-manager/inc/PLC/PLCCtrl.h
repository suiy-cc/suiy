/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef PLC_CONTROLLER_H
#define PLC_CONTROLLER_H

#include <memory>

#include "AppState.h"

using std::shared_ptr;

class Context;
class ProcessLifecycleManager;
class Task;
class PLCTask;

class PLController
{
public:
    PLController();
    ~PLController();

    void doTask(shared_ptr<Task> task);

private:
    shared_ptr<ProcessLifecycleManager> GetHandler(shared_ptr<Task> task);

    void OnModuleStartup(std::shared_ptr<PLCTask> task);
    void OnStartAutoBootProcesses(std::shared_ptr<PLCTask> task);
    void SetAppCrushCB();

    void RapidStart();

    // task handlers
    shared_ptr<ProcessLifecycleManager> appTaskHandler;
    shared_ptr<ProcessLifecycleManager> UIServiceTaskHandler;
    shared_ptr<ProcessLifecycleManager> CMDServiceTaskHandler;
    shared_ptr<ProcessLifecycleManager> defTaskHandler;

    // callback handle
    unsigned int appCrushCBHandle;
};

#endif // PLC_CONTROLLER_H
