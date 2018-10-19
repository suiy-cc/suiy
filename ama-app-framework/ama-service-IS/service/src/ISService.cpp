/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ISService.cpp
/// @brief contains the implementation of class ISService
///
/// Created by wangqi on 2016/11/21.
/// this file contains the implementation of class ISService
///

#include <memory>

#include "ISService.h"
#include "ISTask.h"
#include "logDef.h"

using std::shared_ptr;
using namespace std::placeholders;

ISService::ISService()
{
    logVerbose(IS,"ISService::ISService()-->IN");
    mpISCtrl = (ISController::GetInstance());
    taskHost = std::make_shared<TaskHost>(std::bind(&ISController::doTask,mpISCtrl,_1), std::make_shared<TaskQueueWithProirity>(), "IS");
    logVerbose(IS,"ISService::ISService()-->OUT");
}

ISService::~ISService()
{
    logVerbose(IS,"ISService::~ISService()-->IN");
    logVerbose(IS,"ISService::~ISService()-->OUT");
}

void ISService::Start()
{
    logVerbose(IS,"ISService::Start()-->IN");
    logInfo(IS,"ISService::Start(): service is start!");
    taskHost->Start();
    logVerbose(IS,"ISService::Start()-->OUT");
}

void ISService::End()
{
    logVerbose(IS,"ISService::End()-->IN");
    logInfo(IS, "ISService::End(): service is shutdown!");
    taskHost->End();
    logVerbose(IS,"ISService::End()-->OUT");
}

bool ISService::IsAbleToHandle(shared_ptr<Task> task)const
{
    //logVerbose(AC,"AudioService::IsAbleToHandle()-->IN");
    bool isAble = (typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(ISTask));

    //logVerbose(AC,"AudioService::IsAbleToHandle()-->OUT isAble=",isAble);
    return isAble;
}

void ISService::Handle(shared_ptr<Task> task)
{
    logVerbose(IS,"ISService::Handle()-->IN");
    taskHost->AddTask(task);
    // taskHost->ClearAllTasksToDoThisTask(task);
    logVerbose(IS,"ISService::Handle()-->OUT");
}

void ISService::OnPreRegister()
{
    ;
}

void ISService::OnRegistered()
{
    ;
}

shared_ptr<Task> ISService::GetEmptyTask() const
{
    return std::make_shared<ISTask>();
}

thread::id ISService::GetThreadHandle() const
{
    return taskHost->GetThreadHandle();
}

shared_ptr<Task> ISService::GetCurrentTask() const
{
    return taskHost->GetCurrentTask();
}
