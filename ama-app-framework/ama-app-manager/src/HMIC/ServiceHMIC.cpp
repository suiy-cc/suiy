/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServiceHMIC.cpp
/// @brief contains the implementation of class ServiceHMIC
///
/// Created by zs on 2016/6/7.
/// this file contains the implementation of class ServiceHMIC
///

#include <memory>

#include "ServiceHMIC.h"
#include "HMICTask.h"
#include "HMICCtrl.h"
#include "TaskQueueWithProirity.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

using std::shared_ptr;
using namespace std::placeholders;

ServiceHMIC::ServiceHMIC()
{
    HMIController::SetInstance(new HMIController);
    HMICtrl = (HMIController::GetInstance());
    taskHost = std::make_shared<TaskHost>(std::bind(&HMIController::doTask,HMICtrl,_1), std::make_shared<TaskQueueWithProirity>(), "HMIC");
    logVerbose(HMIC, "ServiceHMIC: service object is initialized!");
}

ServiceHMIC::~ServiceHMIC()
{
    delete HMICtrl;
    logVerbose(HMIC, "ServiceHMIC: service object is destroyed!");
}

void ServiceHMIC::Start()
{
    logInfo(HMIC, "ServiceHMIC::Start(): service is start!");
    taskHost->Start();
}

void ServiceHMIC::End()
{
    logInfo(HMIC, "ServiceHMIC::End(): service is shutdown!");
    taskHost->End();
}

bool ServiceHMIC::IsAbleToHandle(shared_ptr<Task> task)const
{
    return typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(HMICTask);
}

void ServiceHMIC::Handle(shared_ptr<Task> task)
{
    taskHost->AddTask(task);
}

void ServiceHMIC::OnPreRegister()
{
    ;
}

void ServiceHMIC::OnRegistered()
{
    ;
}

shared_ptr<Task> ServiceHMIC::GetFirstTask()
{
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetPriority(E_TASK_PRIORITY_HIGH);
    hmicTask->SetType(HMIC_NOTIFY_MODULE_INITIALIZED);
    return hmicTask;
}

shared_ptr<Task> ServiceHMIC::GetEmptyTask() const
{
    return std::make_shared<HMICTask>();
}

thread::id ServiceHMIC::GetThreadHandle() const
{
    return taskHost->GetThreadHandle();
}

shared_ptr<Task> ServiceHMIC::GetCurrentTask() const
{
    return taskHost->GetCurrentTask();
}
