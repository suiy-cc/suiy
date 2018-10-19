/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef BPMSERVICE_H
#define BPMSERVICE_H

#include <memory>

#include "TaskHandler.h"
#include "TaskHost.h"
#include "AMGRLogContext.h"
#include "BPMCtrl.h"

using std::shared_ptr;

class BPMService: public TaskHandler
{
public:
    /// @name constructor and destructor
    /// @{
    BPMService();
    virtual ~BPMService();
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
    TaskHost taskHost;
    BPMCtrl bpmCtrl;
};

#endif // BPMSERVICE_H
