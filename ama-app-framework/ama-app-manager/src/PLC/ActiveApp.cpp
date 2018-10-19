/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveApp.cpp
/// @brief contains the implementation of class ActiveApp
///
/// Created by zs on 2016/6/22.
/// this file contains the implementation of class ActiveApp
///

#include "ProcessInfo.h"
#include "ActiveApp.h"
#include "AppState.h"
#include "Capi.h"
#include "ID.h"

void ActiveApp::InitializeNotify(bool isVisible)
{
    Process::InitializeNotify(-1, isVisible);
}

void ActiveApp::SwitchTitlebarVisibilityNotify(bool isShow, int style)
{
    auto titlebarObj = GetTitlebar();
    if(titlebarObj)
        ::SwitchTitlebarVisibilityNotify(titlebarObj->GetAMPID(), isShow, style);
}

shared_ptr<UIProcess> ActiveApp::GetTitlebar() const
{
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto titlebar = dynamic_pointer_cast<UIProcess>(procInfo->FindProc(AMPID_SERVICE(GetSeat(), E_APPID_TITLEBAR)));
    procInfo->Unlock();

    return titlebar;
}
