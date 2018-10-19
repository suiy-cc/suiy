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

#include "AudioService.h"
#include "AudioTask.h"

using std::shared_ptr;
using namespace std::placeholders;

AudioService::AudioService()
{
    logVerbose(AC,"AudioService::AudioService()-->IN");
    mpAudioCtrl = (AudioController::GetInstance());
    taskHost = std::make_shared<TaskHost>(std::bind(&AudioController::doTask,mpAudioCtrl,_1), std::make_shared<TaskQueueWithProirity>(), "AC");
    logVerbose(AC,"AudioService::AudioService()-->OUT");
}

AudioService::~AudioService()
{
    logVerbose(AC,"AudioService::~AudioService()-->IN");
    logVerbose(AC,"AudioService::~AudioService()-->OUT");
}

void AudioService::Start()
{
    logVerbose(AC,"AudioService::Start()-->IN");
    logDebug(AC,"AudioService::Start(): service is start!");
    taskHost->Start();
    AudioController::GetInstance()->earlyInit();
    logVerbose(AC,"AudioService::Start()-->OUT");
}

void AudioService::End()
{
    logVerbose(AC,"AudioService::End()-->IN");
    logDebug(AC, "AudioService::End(): service is shutdown!");
    taskHost->End();
    logVerbose(AC,"AudioService::End()-->OUT");
}

bool AudioService::IsAbleToHandle(shared_ptr<Task> task)const
{
    // logVerbose(AC,"AudioService::IsAbleToHandle()-->IN");
    bool isAble = (typeid( *(const_pointer_cast<Task>(task).get()) )==typeid(AudioTask));

    // logVerbose(AC,"AudioService::IsAbleToHandle()-->OUT isAble=",isAble);
    return isAble;
}

void AudioService::Handle(shared_ptr<Task> task)
{
    logVerbose(AC,"AudioService::Handle()-->IN");
    taskHost->AddTask(task);
    logVerbose(AC,"AudioService::Handle()-->OUT");
}

void AudioService::OnPreRegister()
{
    ;
}

void AudioService::OnRegistered()
{
    ;
}

shared_ptr<Task> AudioService::GetEmptyTask() const
{
    return std::make_shared<AudioTask>();
}

thread::id AudioService::GetThreadHandle() const
{
    return taskHost->GetThreadHandle();
}

shared_ptr<Task> AudioService::GetCurrentTask() const
{
    return taskHost->GetCurrentTask();
}
