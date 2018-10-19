/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskQueue.h
/// @brief contains class TaskQueue
///
/// Created by zs on 2016/6/7.
/// this file contains the definination of class TaskQueue.
///

#include "TASK/TaskQueue.h"

void TaskQueue::PushBack(shared_ptr<Task> task)
{
    taskList.push_back(task);
}

void TaskQueue::PushFront(shared_ptr<Task> task)
{
    taskList.push_front(task);
}

void TaskQueue::PopBack()
{
    return taskList.pop_back();
}

void TaskQueue::PopFront()
{
    return taskList.pop_front();
}

shared_ptr<Task> TaskQueue::Front()
{
    return taskList.front();
}

shared_ptr<Task> TaskQueue::Back()
{
    return taskList.back();
}

void TaskQueue::Clear()
{
    taskList.clear();
}

bool TaskQueue::IsEmpty()
{
    return taskList.empty();
}

