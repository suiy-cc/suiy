/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLMCMDService.h
/// @brief contains class PLMCMDService
///
/// Created by zs on 2017/11/2.
/// this file contains the definition of class PLMCMDService
///

#ifndef PLMCMDSERVICE_H
#define PLMCMDSERVICE_H


#include "ProcessLifecycleManager.h"

class ActiveService;

class PLMCMDService
: public ProcessLifecycleManager
{
public:
    PLMCMDService();
    virtual ~PLMCMDService();
    void OnMessage(shared_ptr<PLCTask> task) override;

private:
    /// @name task handlers
    /// @{
    void OnStart(shared_ptr<PLCTask>task);
    void OnStop(shared_ptr<PLCTask>task);
    void OnKill(shared_ptr<PLCTask>task);
    void OnIntent(shared_ptr<PLCTask>task);
    void OnStartSucceeded(shared_ptr<PLCTask>task);
    void OnStartFailed(shared_ptr<PLCTask>task);
    void OnStopSucceeded(shared_ptr<PLCTask>task);
    void OnStopFailed(shared_ptr<PLCTask>task);
    void OnCrush(shared_ptr<PLCTask> task);
    /// @}

    shared_ptr<ActiveService> GetCMDService(AMPID ampid);

    // crushed services
    std::vector<AMPID> crushedServices;
};


#endif // PLMCMDSERVICE_H
