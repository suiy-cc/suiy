/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServiceSM.h
/// @brief contains class ServiceSM
///
/// Created by zs on 2016/6/7.
/// this file contains the definination of class ServiceSM
///

#ifndef SERVICESM_H
#define SERVICESM_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "SMCtrl.h"

using std::shared_ptr;

extern void SMInit(void);
extern void SMdeInit(void);

/// @class ServiceSM
/// @brief this class is the abstraction of SM module
///
/// this class is a listener to TaskDispatcher. it also
/// contains a task host to do the actuall work of handlling
/// a task. it passes an object of class SMController to
/// task host to tell task host how to handle a SM task.
class ServiceSM
:    public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    ServiceSM();
    virtual ~ServiceSM();
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
    SMController SMCtrl;
};

#endif // SERVICESM_H
