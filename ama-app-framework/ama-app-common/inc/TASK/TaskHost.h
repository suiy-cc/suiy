/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskHandler.h
/// @brief contains class TaskHandler
///
/// Created by zs on 2016/6/6.
/// this file contains the definination of class TaskHandler
///

#ifndef TASKHOST_H
#define TASKHOST_H

#include <thread>
#include <functional>
#include <mutex>
#include <memory>

#include <TASK/TaskHandler.h>
#include <TASK/TaskQueue.h>
#include <TASK/TaskQueueWithProirity.h>

using std::shared_ptr;

typedef std::function<void(shared_ptr<Task>)> HandlerFuncType;

/// @class TaskHost
/// @brief this class is the thread host of a moduler
///
/// this class abstracts the thread host of a moduler.
class TaskHost
{
public:
    /// @name constructor and destructor
    /// @{
    TaskHost(HandlerFuncType handler, shared_ptr<TaskQueue> queue, const std::string &name);
    virtual ~TaskHost();
    /// @}
public:
    /// @{
    void Start();
    void End();
    void EndForAllTaskDone();
    /// @}

    void AddTask(shared_ptr<Task>);
    void ClearAllTasksToDoThisTask(shared_ptr<Task>);

    shared_ptr<Task> GetCurrentTask()const;
    std::thread::id GetThreadHandle()const;

private:
    void Run();
    void ClearTasks();

private:
    /// @name forbidden interface
    /// @{
    void operator=(const TaskHost);
    void operator=(const TaskHost&);
    TaskHost(const TaskHost&);
    TaskHost(TaskHost&&);
    /// @}

private:
    std::string name;
    HandlerFuncType handlerFunc;
    shared_ptr<std::thread> threadHost;
    shared_ptr<TaskQueue> taskQueue;
    std::mutex mutexForTaskQueue;
    std::condition_variable conditionVariable;
    bool endFlag;
    bool endForTaskFlag;
    shared_ptr<Task> currentTask;
};

#endif //TASKHOST_H
