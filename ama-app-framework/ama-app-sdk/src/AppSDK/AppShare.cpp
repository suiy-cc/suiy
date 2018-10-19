/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppShare.cpp
/// @brief contains APP share APIs
///
/// Created by zs on 2016/12/14.
/// this file contains the definition of APP share APIs
///

#include "HomePLC.h"
#include "AppShare.h"
#include "AppManager.h"
#include "AppTools.h"

void StartShareApp(AMPID app, E_SEAT seat, bool isMove)
{
    AppManager::GetInstance()->StartShareApp(app, seat, isMove);
}

void StopShareApp(AMPID app, E_SEAT seat)
{
    AppManager::GetInstance()->StopShareApp(app, seat);
}

AMPID GetCurrentShare(E_SEAT seat)
{
    return AppManager::GetInstance()->GetCurrentShare(seat);
}

void Listen2SyncAppStateChange(AppShareStateChangeNotifyCB callback)
{
    return AppManager::GetInstance()->SetAppShareStateNotifyCB(callback);
}

std::vector<unsigned int> GetSourceSurfaces()
{
    return AppManager::GetInstance()->GetSourceSurfaces();
}

bool GetWorkingLayers(std::vector<unsigned int>& workingLayers)
{
    return AppManager::GetInstance()->GetWorkingLayers(workingLayers);
}
