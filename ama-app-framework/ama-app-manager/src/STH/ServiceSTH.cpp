/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServiceSTH.cpp
/// @brief contains class ServiceSTH
///
/// Created by zs on 2017/03/11.
///
/// this file contains the implementation of class ServiceSTH
///

#include "ServiceSTH.h"
#include "STHTask.h"
#include "SynchronizedTaskHandler.h"

using namespace std::placeholders;

ServiceSTH::ServiceSTH()
: TaskHandler()
, STH()
, taskHost(std::bind(&SynchronizedTaskHandler::doTask, &STH, _1), std::make_shared<TaskQueueWithProirity>(), "STH")
{
    ;
}

bool ServiceSTH::IsAbleToHandle(shared_ptr<Task> ptr) const
{
    return typeid( *(const_pointer_cast<Task>(ptr).get()) )==typeid(STHTask);
}

void ServiceSTH::Handle(shared_ptr<Task> ptr)
{
    taskHost.AddTask(ptr);
}

void ServiceSTH::OnPreRegister()
{

}

void ServiceSTH::OnRegistered()
{

}

shared_ptr<Task> ServiceSTH::GetFirstTask()
{
    return nullptr;
}

shared_ptr<Task> ServiceSTH::GetEmptyTask() const
{
    return std::make_shared<STHTask>();
}

thread::id ServiceSTH::GetThreadHandle() const
{
    return taskHost.GetThreadHandle();
}

shared_ptr<Task> ServiceSTH::GetCurrentTask() const
{
    return taskHost.GetCurrentTask();
}

void ServiceSTH::Start()
{
    taskHost.Start();
}

void ServiceSTH::End()
{
    taskHost.End();
}
