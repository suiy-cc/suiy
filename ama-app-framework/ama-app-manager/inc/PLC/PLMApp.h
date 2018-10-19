/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLMApp.h
/// @brief contains class PLMApp
///
/// Created by zs on 2016/10/17.
/// this file contains the definition of class PLMApp
///

#ifndef PLMAPP_H
#define PLMAPP_H

#include <vector>

#include "ama_types.h"
#include "ProcessLifecycleManager.h"

class ActiveApp;
class UIProcess;

class PLMApp
:public ProcessLifecycleManager
{
public:
    PLMApp();
    virtual ~PLMApp();
    virtual void OnMessage(shared_ptr<PLCTask> task) override;

private:
    void StartApp(shared_ptr<PLCTask> task);
    void StopApp(shared_ptr<PLCTask> task);
    void ShowApp(shared_ptr<PLCTask> task);
    void HideApp(shared_ptr<PLCTask> task);
    void KillApp(shared_ptr<PLCTask> task);
    void DriveApp(shared_ptr<PLCTask> task);
    void OnNestedIntent(shared_ptr<PLCTask> task);
    void OnRemoveAppFromZOrder(shared_ptr<PLCTask> task);
    void OnAppStartSucceeded(shared_ptr<PLCTask> task);
    void OnAppStartFailed(shared_ptr<PLCTask> task);
    void OnAppStopSucceeded(shared_ptr<PLCTask> task);
    void OnAppStopFailed(shared_ptr<PLCTask> task);
    void OnAppCrushed(shared_ptr<PLCTask> task);
    void OnAppBootTimeout(shared_ptr<PLCTask> task);

    void SetLastMode(shared_ptr<PLCTask> task);

    // utility
    shared_ptr<ActiveApp> GetApp(AMPID app) const;
    shared_ptr<UIProcess> GetProc(AMPID proc) const;

    // crushed apps
    std::vector<AMPID> crushedApps;
};


#endif // PLMAPP_H
