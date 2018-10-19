/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TitlebarAPI.cpp
/// @brief contains the implementation of title bar API
///
/// Created by zs on 2016/8/29.
/// this file contains the implementation of title bar API
///

#include "TitlebarAPI.h"
#include "AppManager.h"

void ListenToTitlebarVisibilitySwitchNotify(TitlebarVisibilitySwitchNotifyCB f)
{
    unsigned int titlebar = AppManager::GetInstance()->GetAMPID();
    if(titlebar)
        AppManager::GetInstance()->SetTitlebarVisibilitySwitchNotifyCB(f);
}

void ListenToTitlebarDropDownNotify(TitlebarDropDownNotifyCB f)
{
    unsigned int titlebar = AppManager::GetInstance()->GetAMPID();
    if(titlebar)
        AppManager::GetInstance()->SetTitlebarDropDownNotifyCB(f);
}

void NotifyTitlebarDropDown(bool isDropDown)
{
    unsigned int titlebar = AppManager::GetInstance()->GetAMPID();
    if(titlebar)
        AppManager::GetInstance()->MarkTitlebarDropDownState(isDropDown);
}

void ListenTitleBarScreenShotCompleteCB(TitleBarScreenShotCompleteCB f)
{
    unsigned int titlebar = AppManager::GetInstance()->GetAMPID();
    if(titlebar)
        AppManager::GetInstance()->setTitleBarScreenShotCompleteCB(f);
}
