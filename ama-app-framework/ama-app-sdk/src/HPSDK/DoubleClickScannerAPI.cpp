/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DoubleClickScannerAPI.cpp
/// @brief contains APIs for double-click scanner
/// 
/// Created by zs on 2017/07/26.
///
/// this file contains the implementation of double-click scanner
///

#include "DoubleClickScannerAPI.h"
#include "AppManager.h"

void LightUpScreen(E_SEAT seat)
{
    AppManager::GetInstance()->LightUpScreenReq(seat);
}
