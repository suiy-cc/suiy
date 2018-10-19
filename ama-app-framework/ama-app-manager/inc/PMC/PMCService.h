/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PMCSERVICE_H
#define PMCSERVICE_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "AMGRLogContext.h"
#include "PMCtrl.h"

using std::shared_ptr;

class PMCService:
        public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    PMCService();
    virtual ~PMCService();
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
    PowerManagerController* mpPMController;
};

#endif //PMCSERVICE_H
