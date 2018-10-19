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

#include "service_cs.h"
#include "cs_task.h"
#include "TaskDispatcher.h"

RpcProxy rp;

using namespace std::placeholders;

ServiceCS::ServiceCS()
: taskHost(std::bind(&CSController::doTask, &keyCtrl, _1), std::make_shared<TaskQueueWithProirity>(), "CS")
{
    logVerbose("ServiceCS: service object is initialized!");
}

ServiceCS::~ServiceCS()
{
    logVerbose("ServiceCS: service object is destroyed!");
}

void ServiceCS::Start()
{
    taskHost.Start();
}

void ServiceCS::End()
{
    taskHost.End();
}

bool ServiceCS::IsAbleToHandle(shared_ptr<Task> task)const
{
    bool isCSTask = (typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(CSTask));
    return isCSTask;
}

void ServiceCS::Handle(shared_ptr<Task> task)
{
    taskHost.AddTask(task);
}

void ServiceCS::OnPreRegister()
{
    ;
}

void ServiceCS::OnRegistered()
{
    logDebug(CS, "RP init");
    // rpc init
    // RpcProxy rp;
    rp.registerKeyEvent((KeyID)(E_KEY_HOME | KEY_WHEEL_VOL_UP_DOWN
        | KEY_WHEEL_PHONE_MUTE | KEY_WHEEL_HUD_MODE), KEY_EVENT_DOWN, HardkeyFunCB);
    rp.registerRpcEvent(RPC_ALL, RpcRevFunCB);
}

shared_ptr<Task> ServiceCS::GetEmptyTask() const
{
    return std::make_shared<CSTask>();
}

thread::id ServiceCS::GetThreadHandle() const
{
    return taskHost.GetThreadHandle();
}

shared_ptr<Task> ServiceCS::GetCurrentTask() const
{
    return taskHost.GetCurrentTask();
}

shared_ptr<Task> ServiceCS::GetFirstTask()
{
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_MODULE_STARTUP);
    return csTask;
}