/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ISService.h
/// @brief contains class ISService
///
/// Created by wangqi on 2016/11/21.
/// this file contains the definination of class ISService
///

#ifndef ISSERVICE_H
#define ISSERVICE_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "ISCtrl.h"

using std::shared_ptr;

/// @class ISService
/// @brief this class is the abstraction of InformingSound
///
/// this class is a listener to TaskDispatcher. it also
/// contains a task host to do the actuall work of handlling
/// a task. it passes an object of class ISService to
/// task host to tell task host how to handle a Audio
/// task.
class ISService:
        public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    ISService();
    virtual ~ISService();
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
    ISController* mpISCtrl;
};

#endif //ISSERVICE_H
