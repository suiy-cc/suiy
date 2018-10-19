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
/// Created by zs on 2016/6/6.
/// this file contains the definination of class TaskQueue
///

#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <list>
#include <memory>

#include <TASK/Task.h>

using std::shared_ptr;

/// @class TaskQueue
/// @brief this class is the abstraction of task queue
///
/// this class abstracts the task queue.
/// if you want to change the sequence of tasks,
/// subclass this class.
class TaskQueue
{
public:
    /// @name constructor and destructor
    /// @{
    TaskQueue(){};
    virtual ~TaskQueue(){};
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
    std::list<shared_ptr<Task> > taskList;
};

#endif //TASKQUEUE_H
