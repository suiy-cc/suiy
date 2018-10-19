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

#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <memory>

#include <TASK/Task.h>

using std::shared_ptr;

/// @class TaskHandler
/// @brief this class handles tasks
///
/// this class abstracts the task handler.
class TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    TaskHandler(){};
    virtual ~TaskHandler(){};
    /// @}

public:
    /// @returns return true if this object is able to
    /// handle the specified task.
    virtual bool IsAbleToHandle(shared_ptr<Task>)const=0;
    /// @brief handle a task
    virtual void Handle(shared_ptr<Task>)=0;
    /// @brief invoked when current handler is going to be
    /// registered.
    /// this function works in the thread you register
    /// current handler.
    /// at this point, TaskDispatcher::Dispatch() for
    /// current handler is NOT available.
    virtual void OnPreRegister()=0;
    /// @brief invoked when current handler is registered.
    /// this function works in the thread you register
    /// current handler.
    /// at this point, TaskDispatcher::Dispatch() for
    /// current handler is available.
    virtual void OnRegistered()=0;
    /// @brief return the first task of your handler.
    /// overwrite this function if you had any thing that
    /// should be done as soon as your handler is
    /// available.
    /// this function works in the thread you register
    /// current handler.
    virtual shared_ptr<Task> GetFirstTask(){return nullptr;};
    /// @brief return an empty task used to generate the reply task.
    virtual shared_ptr<Task> GetEmptyTask()const=0;
    /// @brief return the thread id of the task host.
    virtual std::thread::id GetThreadHandle()const=0;
    /// @brief return the task which is being handled by the handler.
    virtual shared_ptr<Task> GetCurrentTask()const=0;
    /// @brief start handler thread
    virtual void Start()=0;
    /// @brief end handler's thread
    virtual void End()=0;
};

#endif //TASKHANDLER_H
