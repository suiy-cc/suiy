/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioService.cpp
/// @brief contains the implementation of class AudioService
///
/// Created by wangqi on 2016/6/15.
/// this file contains the implementation of class AudioService
///

#include <memory>

#include "ProService.h"
#include "ProTask.h"
#include "ProCtrl.h"
#include "CommonServiceLog.h"

using std::shared_ptr;
using namespace std::placeholders;

ProjectService::ProjectService()
{
    logVerbose(PRO,"ProjectService::ProjectService()-->IN");
    taskHost = std::make_shared<TaskHost>(std::bind(&ProjectController::doTask,ProjectController::GetInstance(),_1), std::make_shared<TaskQueueWithProirity>(), "PRO");
    logVerbose(PRO,"ProjectService::ProjectService()-->OUT");
}

ProjectService::~ProjectService()
{
    logVerbose(PRO,"AudioService::~AudioService()-->IN");
    logVerbose(PRO,"AudioService::~AudioService()-->OUT");
}

void ProjectService::Start()
{
    logVerbose(PRO,"ProjectService::Start()-->IN");
    logDebug(PRO,"ProjectService::Start(): service is start!");
    taskHost->Start();
    logVerbose(PRO,"ProjectService::Start()-->OUT");
}

void ProjectService::End()
{
    logVerbose(PRO,"ProjectService::End()-->IN");
    logDebug(PRO, "ProjectService::End(): service is shutdown!");
    taskHost->End();
    logVerbose(PRO,"ProjectService::End()-->OUT");
}

bool ProjectService::IsAbleToHandle(shared_ptr<Task> task)const
{
    //logVerbose(PRO,"AudioService::IsAbleToHandle()-->IN");
    bool isAble = (typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(ProjectTask));

    //logVerbose(PRO,"AudioService::IsAbleToHandle()-->OUT isAble=",isAble);
    return isAble;
}

void ProjectService::Handle(shared_ptr<Task> task)
{
    logVerbose(PRO,"ProjectService::Handle()-->IN");
    taskHost->AddTask(task);
    logVerbose(PRO,"ProjectService::Handle()-->OUT");
}

void ProjectService::OnPreRegister()
{
    ;
}

void ProjectService::OnRegistered()
{
    ;
}

shared_ptr<Task> ProjectService::GetEmptyTask() const
{
    return std::make_shared<ProjectTask>();
}

thread::id ProjectService::GetThreadHandle() const
{
    return taskHost->GetThreadHandle();
}

shared_ptr<Task> ProjectService::GetCurrentTask() const
{
    return taskHost->GetCurrentTask();
}
