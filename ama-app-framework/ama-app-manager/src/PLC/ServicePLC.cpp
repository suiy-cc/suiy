/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServicePLC.cpp
/// @brief contains the implementation of class PLC
///
/// Created by zs on 2016/6/7.
/// this file contains the implementation of class PLC
///

#include "ServicePLC.h"
#include "PLCTask.h"
#include "UnitManager.h"
#include "AppList.h"
#include "LastUserCtx.h"
#include "LastUserCtx.h"
#include "RecoverAppMgr.h"
#include "Configuration.h"

using namespace std::placeholders;

ServicePLC::ServicePLC()
: taskHost(std::bind(&PLController::doTask, &PLCtrl, _1), std::make_shared<TaskQueueWithProirity>(), "PLC")
{
    logVerbose("ServicePLC: service object is initialized!");
}

ServicePLC::~ServicePLC()
{
    UnitManager::GetInstance()->destroy();
    logVerbose("ServicePLC: service object is destroyed!");
}

void ServicePLC::Start()
{
    logInfo(PLC, "ServicePLC::Start(): service object is start!");
    taskHost.Start();
}

void ServicePLC::End()
{
    logInfo(PLC, "ServicePLC::End(): service object is shutdown!");
    taskHost.End();
}

bool ServicePLC::IsAbleToHandle(shared_ptr<Task> task)const
{
    return typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(PLCTask);
}

void ServicePLC::Handle(shared_ptr<Task> task)
{
    taskHost.AddTask(task);
}

void ServicePLC::OnPreRegister()
{
    ;
}

void ServicePLC::OnRegistered()
{
    ;
}

shared_ptr<Task> ServicePLC::GetFirstTask()
{
    auto plcTask = make_shared<PLCTask>();
    plcTask->SetType(PLC_NOTIFY_MODULE_STARTUP);
    return plcTask;
}

shared_ptr<Task> ServicePLC::GetEmptyTask() const
{
    return std::make_shared<PLCTask>();
}

thread::id ServicePLC::GetThreadHandle() const
{
    return taskHost.GetThreadHandle();
}

shared_ptr<Task> ServicePLC::GetCurrentTask() const
{
    return taskHost.GetCurrentTask();
}
