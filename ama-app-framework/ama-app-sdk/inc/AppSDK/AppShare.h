/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppShare.h
/// @brief contains APP share APIs
///
/// Created by zs on 2016/12/14.
/// this file contains the declaration of APP share APIs
///

#ifndef APPSHARE_H
#define APPSHARE_H

#include <ama_types.h>

/// @name app-share
/// @{
void StartShareApp(AMPID app, E_SEAT seat, bool isMove = true);
void StopShareApp(AMPID app, E_SEAT seat);
AMPID GetCurrentShare(E_SEAT seat);

typedef void(*AppShareStateChangeNotifyCB)(AMPID app, E_SEAT seat, unsigned int);
void Listen2SyncAppStateChange(AppShareStateChangeNotifyCB callback);
/// @}

/// @name for sync app
/// @{
std::vector<unsigned int> GetSourceSurfaces();
bool GetWorkingLayers(std::vector<unsigned int>& workingLayers);
/// @}

#endif // APPSHARE_H
