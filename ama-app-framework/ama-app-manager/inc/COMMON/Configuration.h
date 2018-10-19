/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Configuration.h
/// @brief contains configurations of ama-app-manager.
/// configuration file is "/etc/ama.d/frameworkConfig.xml"
/// 
/// Created by zs on 2017/05/25.
///

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

bool IsEnableDLTLocalPrint();
int GetRearTimerToCloseLCD();
bool IsFastBootEnabled();
int GetAppBootTimeoutInSec();
bool IsRearAlwaysON();
bool IsScreenDisabled(unsigned int seat);
bool IsAutoStartDisabled();
bool IsSysUIMemOptmz();
int GetMaxBackgroundProcess(int seatid);
int GetMinimumMemoryCanAccept();
bool IsLastSourceEnabled();

#endif // CONFIGURATION_H
