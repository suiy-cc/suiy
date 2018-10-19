/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskDispatcher.h
/// @brief contains class TaskDispatcher
///
/// Created by zs on 2016/6/6.
/// this file contains the definination of class TaskDispatcher
///

#ifndef TASKDISPATCHER_H
#define TASKDISPATCHER_H

#include <vector>
#include <memory>
#include <mutex>

#include <TASK/TaskHandler.h>
#include <COMMON/Broadcaster.h>

using std::shared_ptr;

/// @class TaskDispatcher
/// @brief dispatches tasks
///
/// this class is used to dispatch
class TaskDispatcher
{
private:
    /// @name constructor and destructor
    /// @{
    TaskDispatcher();
    ~TaskDispatcher();
    /// @}
public:
    /// @brief dispatches the task
    /// @param task the task to be dispatched
    void Dispatch(shared_ptr<Task>);

    /// @{
    void RegisterTaskHandler(shared_ptr<TaskHandler>);
    void UnregisterTaskHandler(shared_ptr<TaskHandler>);
    /// @}

    static TaskDispatcher* GetInstance();
    Broadcaster<bool(shared_ptr<Task>)> dispatchedTaskBroadcaster;

private:
    void _Dispatch(shared_ptr<Task>);

private:
    std::vector<shared_ptr<TaskHandler>> handlers;
    std::mutex mutexForHandlers;
};

bool SendQueryTaskNWait(std::shared_ptr<Task> task, unsigned int timeout = 300);
void SendQueryTaskNReplyAtTaskDone(std::shared_ptr<Task> task, const std::function<void(std::shared_ptr<Task>)>& replyer);

#endif //TASKDISPATCHER_H
