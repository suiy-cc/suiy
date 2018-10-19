/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLMUIService.h
/// @brief contains class PLMUIService
///
/// Created by zs on 2016/10/17.
/// this file contains the definition of class PLMUIService
///

#ifndef PLMUISERVICE_H
#define PLMUISERVICE_H


#include "ProcessLifecycleManager.h"

class ActiveUIService;

class PLMUIService
: public ProcessLifecycleManager
{
public:
    PLMUIService();
    virtual ~PLMUIService();
    void OnMessage(shared_ptr<PLCTask> task) override;

private:
    /// @name task handlers
    /// @{
    void OnStart(shared_ptr<PLCTask>task);
    void OnStop(shared_ptr<PLCTask>task);
    void OnShow(shared_ptr<PLCTask>task);
    void OnHide(shared_ptr<PLCTask>task);
    void OnKill(shared_ptr<PLCTask>task);
    void OnIntent(shared_ptr<PLCTask>task);
    void OnStartSucceeded(shared_ptr<PLCTask>task);
    void OnStartFailed(shared_ptr<PLCTask>task);
    void OnStopSucceeded(shared_ptr<PLCTask>task);
    void OnStopFailed(shared_ptr<PLCTask>task);
    void OnCrush(shared_ptr<PLCTask> task);
    /// @}

    /// @name
    /// @{
    shared_ptr<ActiveUIService> GetUIService(AMPID ampid);
    /// @}

    // crushed services
    std::vector<AMPID> crushedServices;
};


#endif // PLMUISERVICE_H
