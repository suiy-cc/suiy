/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ServiceSTH.h
/// @brief contains class ServiceSTH
/// 
/// Created by zs on 2017/03/11.
///
/// this file contains the definition of class ServiceSTH
/// 

#ifndef SERVICESTH_H
#define SERVICESTH_H

#include "TaskHost.h"
#include "TaskHandler.h"
#include "SynchronizedTaskHandler.h"

class ServiceSTH
: public TaskHandler
{
public:
    ServiceSTH();
    /// @{
    virtual bool IsAbleToHandle(shared_ptr<Task> ptr) const override;
    virtual void Handle(shared_ptr<Task> ptr) override;
    virtual void OnPreRegister() override;
    virtual void OnRegistered() override;
    virtual shared_ptr<Task> GetFirstTask() override;
    virtual shared_ptr<Task> GetEmptyTask() const override;
    virtual thread::id GetThreadHandle() const override;
    virtual shared_ptr<Task> GetCurrentTask() const override;
    /// @}

    /// @{
    void Start();
    void End();
    /// @}

private:
    TaskHost taskHost;
    SynchronizedTaskHandler STH;
};


#endif //AMA_APP_FRAMEWORK_SERVICESTH_H
