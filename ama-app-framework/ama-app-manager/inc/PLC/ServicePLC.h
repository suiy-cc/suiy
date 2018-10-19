/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServicePLC.h
/// @brief contains class ServicePLC
///
/// Created by zs on 2016/6/7.
/// this file contains the definination of class ServicePLC
///

#ifndef SERVICEPLC_H
#define SERVICEPLC_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "PLCCtrl.h"

using std::shared_ptr;

/// @class ServicePLC
/// @brief this class is the abstraction of PLC module
///
/// this class is a listener to TaskDispatcher. it also
/// contains a task host to do the actuall work of handlling
/// a task. it passes an object of class PLController to
/// task host to tell task host how to handle a PLC task.
class ServicePLC
:    public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    ServicePLC();
    virtual ~ServicePLC();
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
    virtual shared_ptr<Task> GetFirstTask();
    virtual shared_ptr<Task> GetEmptyTask() const override;
    virtual thread::id GetThreadHandle() const override;
    virtual shared_ptr<Task> GetCurrentTask() const override;
    /// @}

private:
    TaskHost taskHost;
    PLController PLCtrl;
};

#endif // SERVICEPLC_H
