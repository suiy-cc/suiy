/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskQueueWithProirity.h
/// @brief contains class TaskQueueWithProirity
/// 
/// Created by zs on 2017/04/26.
///
/// this file contains the definition of class TaskQueueWithProirity
/// 

#ifndef TASKQUEUEWITHPROIRITY_H
#define TASKQUEUEWITHPROIRITY_H

#include <TASK/TaskQueue.h>

typedef map<E_TASK_PRIORITY, shared_ptr<TaskQueue>> TaskQueueMap;

class TaskQueueWithProirity
: public TaskQueue
{
public:
    /// @name constructor and destructor
    /// @{
    TaskQueueWithProirity();
    virtual ~TaskQueueWithProirity(){};
    /// @}
public:
    /// @name interfaces of task queue
    /// @{
    virtual void PushBack(shared_ptr<Task>);
    virtual void PushFront(shared_ptr<Task>);
    virtual void PopBack();
    virtual void PopFront();
    virtual shared_ptr<Task> Front();
    virtual shared_ptr<Task> Back();
    virtual void Clear();
    virtual bool IsEmpty();
    /// @}
private:
    TaskQueueMap taskQueues;
};


#endif // TASKQUEUEWITHPROIRITY_H
