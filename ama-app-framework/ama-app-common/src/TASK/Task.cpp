/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Task.cpp
/// @brief contains the implementation of class Task
///
/// Created by zs on 2016/6/7.
/// this file contains the implementation of class Task
///

#include <algorithm>

#include "TASK/TaskDispatcher.h"
#include "TASK/Task.h"

#include "log.h"

extern LOG_DECLARE_CONTEXT(TASK);

Task::Task()
    : priority(E_TASK_PRIORITY_NORMAL)
    , type(-1)
    , userData(0)
    , freeUserData(nullptr)
    , onReplyReceive(nullptr)
    , isReplyTask(false)
    , replyTask(nullptr)
    , parent()
    , children()
    , onPreTaskDone(nullptr)
    , onTaskDone(nullptr)
    , taskDoneCallbackData(0)
    , freeTaskDoneCallbackData(nullptr)
    , mIsWatchDogTask(false)
{
    ID = totalTaskID++;
    // logDebug("Task::Task()-->>create task taskID=",mTaskID);

    parent.SetValue(nullptr);
}

Task::~Task()
{
    if(freeTaskDoneCallbackData)
        freeTaskDoneCallbackData(taskDoneCallbackData);
    if(freeUserData)
        freeUserData(userData);
}

void Task::SetPriority(E_TASK_PRIORITY in_priority)
{
    priority = in_priority;
}

E_TASK_PRIORITY Task::GetPriority()const
{
    return priority;
}

void Task::SetThisIsWatchDogTask(void)
{
    mIsWatchDogTask=true;
}

bool Task::IsWatchDotTask(void)
{
    return mIsWatchDogTask;
}

void Task::SetType(int taskType)
{
    type = taskType;
}

int Task::GetType()const
{
    return type;
}

int Task::GetID(void)const
{
    return ID;
}

void Task::SetUserData(unsigned long long data, std::function<void(unsigned long long)> f)
{
    userData = data;
    freeUserData = f;
}

unsigned long long Task::GetUserData()
{
    return userData;
}

const function<void()> &Task::GetReplyReceiver() const
{
    return onReplyReceive;
}

void Task::SetReplyReceiver(const function<void()> &accept)
{
    onReplyReceive = accept;
}

const function<void(std::shared_ptr<Task>)> &Task::GetPreTaskDoneCB()const
{
    return onPreTaskDone;
}

void Task::SetPreTaskDoneCB(const function<void(std::shared_ptr<Task>)> &onPreTaskDone)
{
    this->onPreTaskDone = onPreTaskDone;
}

const function<void(std::shared_ptr<Task>)> &Task::GetTaskDoneCB()const
{
    return onTaskDone;
}

void Task::SetTaskDoneCB(const function<void(std::shared_ptr<Task>)> &onTaskDone)
{
    this->onTaskDone = onTaskDone;
}

unsigned long long Task::GetTaskDoneCBData()const
{
    return taskDoneCallbackData;
}

void Task::SetTaskDoneCBData(unsigned long long data, const function<void(unsigned long long)> f)
{
    taskDoneCallbackData = data;
    freeTaskDoneCallbackData = f;
}

void Task::SetReplyTask(bool isReplyTask)
{
    this->isReplyTask = isReplyTask;
}

bool Task::IsReplyTask()
{
    return isReplyTask;
}

void Task::_Reply()
{
    if(onReplyReceive && replyTask)
    {
        logDebugF(TASK, "Task::_Reply(): sending accept task(%d)...", GetID());
        auto parentTask = GetParent();
        if(parentTask)
        {
            replyTask->SetParent(parentTask);
            parentTask->ReplaceChild(this, replyTask);
        }
        replyTask->SetReplyReceiver(onReplyReceive);
        replyTask->SetReplyTask(true);
        TaskDispatcher::GetInstance()->Dispatch(replyTask);
        logDebugF(TASK, "Task::_Reply(): sending accept task(%d) OK", GetID());
    }
}

void Task::SendReply()
{
    if(!FrontChild())
    {
        _Reply();
    }
}

void Task::OnReceiveReply()
{
    if(onReplyReceive)
        onReplyReceive();

    parent.Operate([=](std::shared_ptr<Task> Parent){
        if(Parent)
        {
            logDebugF(TASK, "Task::OnReceiveReply(): parent(%d)->Reply OK", Parent->GetID());
            if(this->FrontChild())
            {
                Parent->ReplaceChild(this, this->children);
                ClearChild();
            }
            else
            {
                if(Parent->FrontChild())
                    logDebug(TASK, "Task::OnReceiveReply(): parent->FrontChild() = %d", Parent->FrontChild()->GetID());
                else
                    logDebug(TASK, "Task::OnReceiveReply(): parent->FrontChild() = nullptr");
                Parent->EraseChild(this);
            }
            Parent->SendReply();
        }
    });
}

std::shared_ptr<Task> Task::GetParent()
{
    return parent.GetValue();
}

void Task::SetParent(std::shared_ptr<Task> task)
{
    parent.SetValue(task);
}

std::shared_ptr<Task> Task::FrontChild()
{
    std::shared_ptr<Task> rtv  = nullptr;

    children.Operate([&](std::deque<std::shared_ptr<Task>>& Children){
        if(Children.size())
            rtv = Children.front();
    });

    return rtv;
}

void Task::PushChild(std::shared_ptr<Task> task)
{
    children.Operate([=](std::deque<std::shared_ptr<Task>>& Children){
        if(task)
            Children.push_back(task);
    });
}

void Task::PopChild()
{
    children.Operate([=](std::deque<std::shared_ptr<Task>>& Children){
        Children.pop_front();
    });
}

void Task::ClearChild()
{
    children.Operate([=](std::deque<std::shared_ptr<Task>>& Children){
        Children.clear();
    });
}

void Task::EraseChild(const Task *task)
{
    children.Operate([=](std::deque<std::shared_ptr<Task>>& Children){
        Children.erase(
            std::remove_if(Children.begin(), Children.end(),
                [=](std::shared_ptr<Task> child)->bool
                {
                    return child.get()==task;
                }),  Children.end());
    });
}

void Task::ReplaceChild(const Task *task, std::shared_ptr<Task> newChild)
{
    children.Operate([=](std::deque<std::shared_ptr<Task>>& Children){
        auto iter = std::find_if(Children.begin(), Children.end(),
            [=](std::shared_ptr<Task> child)->bool
            {
                return child.get()==task;
            });

        if(iter!=Children.end())
            *iter = newChild;
    });
}

void Task::ReplaceChild(const Task *task, const ThreadSafeValueKeeper<std::deque<std::shared_ptr<Task>>>& newChildren)
{
    newChildren.OperateConst([=](const std::deque<std::shared_ptr<Task>>& NewChildren){
        children.Operate([&](std::deque<std::shared_ptr<Task>>& Children){
            auto iter = std::find_if(Children.begin(), Children.end(),
                [=](std::shared_ptr<Task> child)->bool
                {
                    return child.get()==task;
                });

            if(iter!=Children.end())
                Children.insert(iter, NewChildren.begin(), NewChildren.end());
        });
    });
}

bool Task::IsAMissingChild()
{
    // a reply task always know its parent
    // if it has one. so it can never be a
    // missing child task with unknown
    // parent.
    // if a task have no accept-handler-function,
    // it can never be a child task.
    return !IsReplyTask() && GetReplyReceiver();
}

void Task::OnPreTaskDone()
{
    if(onPreTaskDone)
        onPreTaskDone(shared_from_this());
}

void Task::OnTaskDone()
{
    if(onTaskDone)
        onTaskDone(shared_from_this());
}
