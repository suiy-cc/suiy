/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <memory>

#include "PMCService.h"
#include "PMCTask.h"

using std::shared_ptr;
using namespace std::placeholders;

PMCService::PMCService()
{
    logVerbose(PMC,"PMCService::PMCService()-->IN");
    mpPMController = (PowerManagerController::GetInstance());
    taskHost = std::make_shared<TaskHost>(std::bind(&PowerManagerController::doTask,mpPMController,_1), std::make_shared<TaskQueueWithProirity>(), "PMC");
    logVerbose(PMC,"PMCService::PMCService()-->OUT");
}

PMCService::~PMCService()
{
    logVerbose(PMC,"PMCService::~PMCService()-->IN");
    logVerbose(PMC,"PMCService::~PMCService()-->OUT");
}

void PMCService::Start()
{
    logVerbose(PMC,"PMCService::Start()-->IN");
    logInfo(PMC,"PMCService::Start(): service is start!");
    taskHost->Start();
    logVerbose(PMC,"PMCService::Start()-->OUT");
}

void PMCService::End()
{
    logVerbose(PMC,"PMCService::End()-->IN");
    logInfo(PMC, "PMCService::End(): service is shutdown!");
    taskHost->End();
    logVerbose(PMC,"PMCService::End()-->OUT");
}

bool PMCService::IsAbleToHandle(shared_ptr<Task> task)const
{
    //logVerbose(PMC,"PMCService::IsAbleToHandle()-->IN");
    bool isAble = (typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(PMCTask));

    //logVerbose(PMC,"PMCService::IsAbleToHandle()-->OUT isAble=",isAble);
    return isAble;
}

void PMCService::Handle(shared_ptr<Task> task)
{
    logVerbose(PMC,"PMCService::Handle()-->IN");
    taskHost->AddTask(task);
    logVerbose(PMC,"PMCService::Handle()-->OUT");
}

void PMCService::OnPreRegister()
{
    ;
}

void PMCService::OnRegistered()
{
    ;
}

shared_ptr<Task> PMCService::GetEmptyTask() const
{
    return std::make_shared<PMCTask>();
}

thread::id PMCService::GetThreadHandle() const
{
    return taskHost->GetThreadHandle();
}

shared_ptr<Task> PMCService::GetCurrentTask() const
{
    return taskHost->GetCurrentTask();
}
