/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskHost.h
/// @brief contains class TaskHost
///
/// Created by zs on 2016/6/7.
/// this file contains the definination of class TaskHost.
///

#include <condition_variable>
#include <sys/prctl.h>

#include "log.h"
#include "TASK/TaskHost.h"

extern LOG_DECLARE_CONTEXT(TASK);
void InitializeLogContext();

TaskHost::TaskHost(HandlerFuncType handler, shared_ptr<TaskQueue> queue, const std::string &name)
: handlerFunc(handler)
, taskQueue(queue)
, threadHost(nullptr)
, mutexForTaskQueue()
, endFlag(false)
, endForTaskFlag(false)
, name(name)
, currentTask(nullptr)
{
    InitializeLogContext();
}

TaskHost::~TaskHost()
{
    ;
}

void TaskHost::Start()
{
    // start the thread
    threadHost = std::make_shared<std::thread>(std::bind(&TaskHost::Run, this));
}

void TaskHost::ClearTasks()
{
    DomainVerboseLog chatter(TASK, "TaskHost::ClearTasks()");

    taskQueue->Clear();
}

void TaskHost::End()
{
    DomainVerboseLog chatter(TASK, "TaskHost::End()");

    // prepare to end the thread.
    // 1. set end flag
    // 2. clear task queue
    mutexForTaskQueue.lock();
    endFlag = true;
    ClearTasks();
    mutexForTaskQueue.unlock();

    // wake thread host to let it finish
    // its on-going job.
    conditionVariable.notify_one();

    // wait thread host to end
    threadHost->join();
}

void TaskHost::EndForAllTaskDone()
{
    DomainVerboseLog chatter(TASK, "TaskHost::EndForAllTaskDone()");

    // prepare to end the thread.
    // 1. set end flag
    // 2. clear task queue
    mutexForTaskQueue.lock();
    endForTaskFlag = true;
    mutexForTaskQueue.unlock();

    // wake thread host to let it finish
    // its on-going job.
    conditionVariable.notify_one();

    // wait thread host to end
    threadHost->join();
}

void TaskHost::AddTask(shared_ptr<Task> task)
{
    std::unique_lock<std::mutex> uLock(mutexForTaskQueue);

    // add new task to task queue if the task host
    // is still on
    if(!endFlag)
    {
        // add new task to task queue
        taskQueue->PushBack(task);

        // wake thread host to handle the task
        conditionVariable.notify_one();
    }
}

void TaskHost::ClearAllTasksToDoThisTask(shared_ptr<Task> task)
{
    std::unique_lock<std::mutex> uLock(mutexForTaskQueue);

    // add new task to task queue if the task host
    // is still on
    if(!endFlag)
    {
        // clear all old tasks
        taskQueue->Clear();

        // add new task to task queue
        taskQueue->PushBack(task);

        // wake thread host to handle the task
        conditionVariable.notify_one();
    }
}

shared_ptr<Task> TaskHost::GetCurrentTask()const
{
    return currentTask;
}

std::thread::id TaskHost::GetThreadHandle()const
{
    return threadHost->get_id();
}

void TaskHost::Run()
{
    DomainVerboseLog chatter(TASK, "TaskHost::Run()");

    // set thread name
    if(!name.empty())
        prctl(PR_SET_NAME, name.c_str());

    while(true)
    {
        shared_ptr<Task> task = nullptr;
        // pick a task, take it off the queue
        {
            std::unique_lock<std::mutex> uLock(mutexForTaskQueue);

            if(taskQueue->IsEmpty() && endForTaskFlag)
                break;

            // suspend the thread host if the task queue
            // is empty.
            if(taskQueue->IsEmpty() && !endFlag)
                conditionVariable.wait(uLock);

            if(taskQueue->IsEmpty() && endForTaskFlag)
                break;

            // exit thread if end flag is set.
            if (endFlag)
                break;

            // get new task from task queue
            if( taskQueue->IsEmpty() || taskQueue->Front()==nullptr )
                continue;
            else
            {
                task = taskQueue->Front();
                taskQueue->PopFront();
            }
        }

        // handle a task
        try
        {
            currentTask = task;
            task->OnPreTaskDone();
        }
        catch (std::exception& e)
        {
            logError(TASK, LOG_HEAD, (&e)->what(), "is caught during task->OnPreTaskDone()!");
            logError(TASK, LOG_HEAD, "task class: ", typeid(*task).name(), " task id: ", task->GetID(), " task type: ", task->GetType(), " task type name: ", task->GetTaskTypeString());
        }
        catch (...)
        {
            logError(TASK, LOG_HEAD, "Unexpected exception!");
            logError(TASK, LOG_HEAD, "task class: ", typeid(*task).name(), " task id: ", task->GetID(), " task type: ", task->GetType(), " task type name: ", task->GetTaskTypeString());
        }

        try
        {
            if(task->IsReplyTask())
                task->OnReceiveReply();
            else
            {
                handlerFunc(task);
                task->SendReply();
            }
        }
        catch (std::exception& e)
        {
            logError(TASK, LOG_HEAD, (&e)->what(), "is caught during handlerFunc(task)!");
            logError(TASK, LOG_HEAD, "task class: ", typeid(*task).name(), " task id: ", task->GetID(), " task type: ", task->GetType(), " task type name: ", task->GetTaskTypeString());
        }
        catch (...)
        {
            logError(TASK, LOG_HEAD, "Unexpected exception!");
            logError(TASK, LOG_HEAD, "task class: ", typeid(*task).name(), " task id: ", task->GetID(), " task type: ", task->GetType(), " task type name: ", task->GetTaskTypeString());
        }

        try
        {
            task->OnTaskDone();
            currentTask = nullptr;
        }
        catch (std::exception& e)
        {
            logError(TASK, LOG_HEAD, (&e)->what(), "is caught during task->OnTaskDone()!");
            logError(TASK, LOG_HEAD, "task class: ", typeid(*task).name(), " task id: ", task->GetID(), " task type: ", task->GetType(), " task type name: ", task->GetTaskTypeString());
        }
        catch (...)
        {
            logError(TASK, LOG_HEAD, "Unexpected exception!");
            logError(TASK, LOG_HEAD, "task class: ", typeid(*task).name(), " task id: ", task->GetID(), " task type: ", task->GetType(), " task type name: ", task->GetTaskTypeString());
        }
    }
}
