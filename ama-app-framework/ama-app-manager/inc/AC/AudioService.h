/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioService.h
/// @brief contains class AudioService
///
/// Created by wangqi on 2016/6/15.
/// this file contains the definination of class AudioService
///

#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "AudioCtrl.h"
#include "AMGRLogContext.h"

using std::shared_ptr;

/// @class AudioService
/// @brief this class is the abstraction of Audio module
///
/// this class is a listener to TaskDispatcher. it also
/// contains a task host to do the actuall work of handlling
/// a task. it passes an object of class AudioController to
/// task host to tell task host how to handle a Audio
/// task.
class AudioService:
        public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    AudioService();
    virtual ~AudioService();
    /// @}

public:
    /// @{
    void Start();
    void End();
    /// @}

    /// @{
    virtual bool IsAbleToHandle(shared_ptr<Task>)const;
    virtual void Handle(shared_ptr<Task>);
    virtual void OnPreRegister();
    virtual void OnRegistered();
    virtual shared_ptr<Task> GetEmptyTask() const override;
    virtual thread::id GetThreadHandle() const override;
    virtual shared_ptr<Task> GetCurrentTask() const override;
    /// @}

private:
    std::shared_ptr<TaskHost> taskHost;
    AudioController* mpAudioCtrl;
};

#endif //AUDIOSERVICE_H
