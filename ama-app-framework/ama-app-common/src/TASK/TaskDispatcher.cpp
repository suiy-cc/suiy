/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskDispatcher.cpp
/// @brief contains the implementation of class TaskDispatcher
///
/// Created by zs on 2016/6/7.
/// this file contains the implementation of class TaskDispatcher
///

#include <algorithm>
#include <COMMON/UniqueID.h>

#include "TASK/TaskDispatcher.h"
#include "COMMON/NotifyWaiter.h"

#include "log.h"

extern LOG_DECLARE_CONTEXT(TASK);
void InitializeLogContext();
void DestroyLogContext();

TaskDispatcher::TaskDispatcher()
{
    InitializeLogContext();
}

TaskDispatcher::~TaskDispatcher()
{
    DestroyLogContext();
}

void TaskDispatcher::Dispatch(shared_ptr<Task> task)
{
    std::unique_lock<std::mutex> uLock(mutexForHandlers);

    _Dispatch(task);
}

void TaskDispatcher::RegisterTaskHandler(shared_ptr<TaskHandler> handler)
{
    std::unique_lock<std::mutex> uLock(mutexForHandlers, std::defer_lock);

    if(handler)
    {
        // send register message
        handler->OnPreRegister();

        // lock handler records
        uLock.lock();

        // record current handler
        handlers.push_back(handler);

        // add first task to handler
        auto firstTask = handler->GetFirstTask();
        if(firstTask)
            handler->Handle(firstTask);

        // print log
        logInfo(TASK, "TaskDispatcher::RegisterTaskHandler():"
                , "handler", typeid(*(handler.get())).name()
                , "is registered!");
        logVerbose(TASK, "TaskDispatcher::RegisterTaskHandler():"
                   , "Total handlers are:", handlers.size());

        // unlock handler records
        uLock.unlock();

        // send register message
        handler->OnRegistered();
    }
}

void TaskDispatcher::UnregisterTaskHandler(shared_ptr<TaskHandler> handler)
{
    std::unique_lock<std::mutex> uLock(mutexForHandlers);
    auto iter = std::find(handlers.begin(), handlers.end(), handler);
    if(iter != handlers.end())
    {
        handlers.erase(iter);

        logInfo(TASK, "TaskDispatcher::UnregisterTaskHandler():"
                , " handler ", typeid(*(handler.get())).name()
                , " is unregistered!");
        logVerbose(TASK, "TaskDispatcher::UnregisterTaskHandler():"
                   , " Total handlers are: ", handlers.size());
    }
    else
    {
        logWarn(TASK, "TaskDispatcher::UnregisterTaskHandler(): unknown handler: ", typeid(*(handler.get())).name());
    }
}

static std::mutex mutex4TaskDispatcherInitialization;
TaskDispatcher* TaskDispatcher::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    std::unique_lock<std::mutex> lock(mutex4TaskDispatcherInitialization);

    static TaskDispatcher onlyInstance;
    return &onlyInstance;
}

void TaskDispatcher::_Dispatch(shared_ptr<Task> task)
{
    if(!task)
    {
        logWarn(TASK, "TaskDispatcher::_Dispatch(): null task pointer!");
        return;
    }
    //DomainVerboseLog chatter(TASK, formatText(__STR_FUNCTION__+"(%s)", task->GetTaskTypeString().c_str()));

    //logDebug(TASK, LOG_HEAD, "task's class: ", typeid(*(task.get())).name());
    std::vector<unsigned int> toBeReleased;
    // broadcast task-dispatched message
    dispatchedTaskBroadcaster.NotifyAll([task, &toBeReleased](unsigned int handle, std::function<bool(shared_ptr<Task>)> taskCallback){
        //DomainVerboseLog chatter(TASK, formatText(__STR_FUNCTION__+"Broadcaster(%s)", task->GetTaskTypeString().c_str()));

        bool isDone = taskCallback(task);
        if(isDone)
            toBeReleased.push_back(handle);
    });

    // release callbacks
    for(auto& handle: toBeReleased)
        dispatchedTaskBroadcaster.Unregister(handle);

    if(task->IsAMissingChild())
    {
        logWarnF(TASK, "a missing child task(%d) is found!", task->GetID());
        // try to find the parent of this task
        for(auto handler: handlers)
        {
            auto currentThread = std::this_thread::get_id();

            // parent found!
            if(currentThread == handler->GetThreadHandle())
            {
                // 1. create reply task for current task
                if(task->GetReplyReceiver())
                    task->replyTask = handler->GetEmptyTask();

                // 2. connect parent and child
                auto parent = handler->GetCurrentTask();
                if(parent)
                {
                    logWarnF(TASK, "child task's parent(%d) is found!", parent->GetID());
                    task->SetParent(parent);
                    parent->PushChild(task);
                }

                // 3. quit looping
                break;
            }
        }
    }
    else if(task->IsReplyTask())
        logWarnF(TASK, "a reply task(%d) is found!", task->GetID());

    // dispatch the task
    int dispatchCount = 0;
    for(auto handler: handlers)
    {
        if(handler->IsAbleToHandle(task))
        {
            handler->Handle(task);
            ++dispatchCount;
        }
    }

    // print a warning if a task is not handled
    if(dispatchCount==0)
        logError(TASK, LOG_HEAD, "unhandled task: ", task->GetID(), " class: ", typeid(*task).name(), " type name: ", task->GetTaskTypeString(), " type: ", task->GetType());
}

bool SendQueryTaskNWait(std::shared_ptr<Task> task, unsigned int timeout)
{
    DomainVerboseLog chatter(TASK, "SendQueryTaskNWait()");

    bool rtv = true;

    unsigned long long waiterID = GetUniqueID();
    RegisterWaiterID(waiterID);

    task->SetTaskDoneCB([waiterID](std::shared_ptr<Task>){
        Notify(waiterID);
    });

    // send task
    TaskDispatcher::GetInstance()->Dispatch(task);

    if(timeout==0)
    {
        while(!Wait(waiterID))
            logWarn(TASK, "SendQueryTaskNWait(): current query task takes too long.");
    }
    else
    {
        if(!Wait(waiterID, timeout))
        {
            logWarn(TASK, "SendQueryTaskNWait(): query task timeout!");
            rtv = false;
        }
    }

    UnregisterWaiterID(waiterID);
    return rtv;
}

void SendQueryTaskNReplyAtTaskDone(std::shared_ptr<Task> task, const std::function<void(std::shared_ptr<Task>)>& replyer)
{
    DomainVerboseLog chatter(TASK, "SendQueryTaskNPeplyAtTaskDone()");

    if(!task)
    {
        logWarn(TASK, "SendQueryTaskNPeplyAtTaskDone(): task pointer is null!");
        return;
    }

    if(!replyer)
    {
        logWarn(TASK, "SendQueryTaskNPeplyAtTaskDone(): replyer pointer is null!");
        return;
    }


    task->SetTaskDoneCB(replyer);

    // send task
    TaskDispatcher::GetInstance()->Dispatch(task);
}
