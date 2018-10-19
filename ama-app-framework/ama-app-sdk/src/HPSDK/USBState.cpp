/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file USBState.cpp
/// @brief contains the implementation of usb API
///
/// Created by cj on 2017/9/7.
/// this file contains the declaration of title bar API
///
#include "USBState.h"
#include "AppManager.h"



void USBControl(ama_USBOperate op)
{
     AppManager::GetInstance()->USBStateControll(op);
}
