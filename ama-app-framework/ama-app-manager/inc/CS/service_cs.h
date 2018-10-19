/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _SERVICE_CS_H
#define _SERVICE_CS_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "cs_ctrl.h"

using std::shared_ptr;

class ServiceCS
:    public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    ServiceCS();
    virtual ~ServiceCS();
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
    virtual shared_ptr<Task> GetFirstTask() override;
    /// @}

private:
    TaskHost taskHost;
    CSController keyCtrl;
};

#endif
