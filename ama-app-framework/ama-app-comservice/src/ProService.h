/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PROSERVICE_H
#define PROSERVICE_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"

using std::shared_ptr;

class ProjectService:
        public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    ProjectService();
    virtual ~ProjectService();
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
};

#endif //PROSERVICE_H
