/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <memory>

#include "BPMService.h"
#include "BPMTask.h"

using std::shared_ptr;
using namespace std::placeholders;

BPMService::BPMService()
    :taskHost(std::bind(&BPMCtrl::doTask, &bpmCtrl, _1),std::make_shared<TaskQueueWithProirity>(), "BPM")
{
    logVerbose(BPM,"BPMService::BPMService()-->IN");
    logVerbose(BPM,"BPMService::BPMService()-->OUT");
}

BPMService::~BPMService()
{
    logVerbose(BPM,"BPMService::~BPMService()-->IN");
    logVerbose(BPM,"BPMService::~BPMService()-->OUT");
}

void BPMService::Start()
{
    logVerbose(BPM,"BPMService::Start()-->IN");
    logDebug(BPM,"BPMService::Start(): service is start!");
     BPManager::GetInstance()->Init();
    taskHost.Start();

    logVerbose(BPM,"BPMService::Start()-->OUT");
}

void BPMService::End()
{
    logVerbose(BPM,"BPMService::End()-->IN");
    logDebug(BPM, "BPMService::End(): service is shutdown!");
    taskHost.End();
    logVerbose(BPM,"BPMService::End()-->OUT");
}

bool BPMService::IsAbleToHandle(shared_ptr<Task> task)const
{
    //logVerbose(BPM,"BPMService::IsAbleToHandle()-->IN");
    bool isAble = (typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(BPMTask));

    //logVerbose(BPM,"BPMService::IsAbleToHandle()-->OUT isAble=",isAble);
    return isAble;
}

void BPMService::Handle(shared_ptr<Task> task)
{
    logVerbose(BPM,"BPMService::Handle()-->IN");
    taskHost.AddTask(task);
    logVerbose(BPM,"BPMService::Handle()-->OUT");
}

void BPMService::OnPreRegister()
{
    ;
}

void BPMService::OnRegistered()
{
    ;
}

shared_ptr<Task> BPMService::GetEmptyTask() const
{
    return std::make_shared<BPMTask>();
}

thread::id BPMService::GetThreadHandle() const
{
    return taskHost.GetThreadHandle();
}

shared_ptr<Task> BPMService::GetCurrentTask() const
{
    return taskHost.GetCurrentTask();
}
