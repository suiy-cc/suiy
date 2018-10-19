/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskQueueWithProirity.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/04/26.
///

/// this file contains the implementation of class TaskQueueWithProirity
///

#include "TASK/TaskQueueWithProirity.h"

TaskQueueWithProirity::TaskQueueWithProirity()
{
    taskQueues[E_TASK_PRIORITY_NORMAL] = std::make_shared<TaskQueue>();
    taskQueues[E_TASK_PRIORITY_HIGH] = std::make_shared<TaskQueue>();
}

void TaskQueueWithProirity::PushBack(shared_ptr<Task> task)
{
    taskQueues[task->GetPriority()]->PushBack(task);
}
void TaskQueueWithProirity::PushFront(shared_ptr<Task> task)
{
    taskQueues[task->GetPriority()]->PushBack(task);
}
void TaskQueueWithProirity::PopBack()
{
    if(!taskQueues[E_TASK_PRIORITY_NORMAL]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_NORMAL]->PopBack();
    if(!taskQueues[E_TASK_PRIORITY_HIGH]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_HIGH]->PopBack();
}
void TaskQueueWithProirity::PopFront()
{
    if(!taskQueues[E_TASK_PRIORITY_HIGH]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_HIGH]->PopFront();
    if(!taskQueues[E_TASK_PRIORITY_NORMAL]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_NORMAL]->PopFront();
}
shared_ptr<Task> TaskQueueWithProirity::Front()
{
    if(!taskQueues[E_TASK_PRIORITY_HIGH]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_HIGH]->Front();
    if(!taskQueues[E_TASK_PRIORITY_NORMAL]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_NORMAL]->Front();

    return nullptr;
}
shared_ptr<Task> TaskQueueWithProirity::Back()
{
    if(!taskQueues[E_TASK_PRIORITY_NORMAL]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_NORMAL]->Back();
    if(!taskQueues[E_TASK_PRIORITY_HIGH]->IsEmpty())
        return taskQueues[E_TASK_PRIORITY_HIGH]->Back();

    return nullptr;
}
void TaskQueueWithProirity::Clear()
{
    for(auto& pair: taskQueues)
        pair.second->Clear();
}
bool TaskQueueWithProirity::IsEmpty()
{
    for(auto& pair: taskQueues)
        if(!pair.second->IsEmpty())
            return false;

    return true;
}
