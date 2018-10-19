/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServiceHMIC.h
/// @brief contains class ServiceHMIC
///
/// Created by zs on 2016/6/7.
/// this file contains the definition of class ServiceHMIC
///

#ifndef SERVICEHMIC_H
#define SERVICEHMIC_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"

using std::shared_ptr;
class HMIController;

/// @class ServiceHMIC
/// @brief this class is the abstraction of HMIC module
///
/// this class is a listener to TaskDispatcher. it also
/// contains a task host to do the actuall work of handlling
/// a task. it passes an object of class HMIController to
/// task host to tell task host how to handle a HMIC
/// task.
class ServiceHMIC:
        public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    ServiceHMIC();
    virtual ~ServiceHMIC();
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
    virtual shared_ptr<Task> GetFirstTask() override;
    virtual shared_ptr<Task> GetEmptyTask() const override;
    virtual thread::id GetThreadHandle() const override;
    virtual shared_ptr<Task> GetCurrentTask() const override;
    /// @}

private:
    std::shared_ptr<TaskHost> taskHost;
    HMIController* HMICtrl;
};

#endif // SERVICEHMIC_H
