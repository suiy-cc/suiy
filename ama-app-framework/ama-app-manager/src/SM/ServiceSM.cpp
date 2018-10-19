/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServiceSM.cpp
/// @brief contains the implementation of class SM
///
/// Created by zs on 2016/6/7.
/// this file contains the implementation of class SM
///

#include "ServiceSM.h"

using namespace std::placeholders;

ServiceSM::ServiceSM()
: taskHost(std::bind(&SMController::doTask, &SMCtrl, _1), std::make_shared<TaskQueueWithProirity>(), "SM")
{
    logVerbose(SM,"ServiceSM: service object is initialized!");
}

ServiceSM::~ServiceSM()
{
    logVerbose(SM,"ServiceSM: service object is destroyed!");
}

void ServiceSM::Start()
{
    logInfo(SM, "ServiceSM::Start(): service object is start!");
    taskHost.Start();
}

void ServiceSM::End()
{
    logInfo(SM, "ServiceSM::End(): service object is shutdown!");
	SMdeInit();
    taskHost.End();
}

bool ServiceSM::IsAbleToHandle(shared_ptr<Task> task)const
{
    return typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(SMTask);
}

void ServiceSM::Handle(shared_ptr<Task> task)
{
    taskHost.AddTask(task);
}

void ServiceSM::OnPreRegister()
{
	;
}

void ServiceSM::OnRegistered()
{
    ;
}

shared_ptr<Task> ServiceSM::GetFirstTask()
{
    return NULL;
}

shared_ptr<Task> ServiceSM::GetEmptyTask() const
{
    return std::make_shared<SMTask>();
}

thread::id ServiceSM::GetThreadHandle() const
{
    return taskHost.GetThreadHandle();
}

shared_ptr<Task> ServiceSM::GetCurrentTask() const
{
    return taskHost.GetCurrentTask();
}
