/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file StartService.h
/// @brief contains 
/// 
/// Created by zs on 2017/05/10.
///

#ifndef STARTSERVICE_H
#define STARTSERVICE_H

#include <ama_types.h>

void StartService(AMPID service);
void StartService();

void StartAutoStartProcesses();

void SendAutoStartTask();

#endif // STARTSERVICE_H
