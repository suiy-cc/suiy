/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#define WATCHDOG_SUCCESS 1
#define WATCHDOG_FAILURE 0

enum WdTaskId
{
	E_TASK_ID_PLC   = 0,
	E_TASK_ID_CS,
	E_TASK_ID_HMIC,
	E_TASK_ID_SM,
	E_TASK_ID_AC,
	E_TASK_ID_PMC,
	// ...
	E_TASK_ID_NUM,
};

void wdInit(void);

void wdCheckAllStates(void);

void wdSetState(WdTaskId tid, int flag);

void wdSendWatchdogTask(void);

void wdNotifySystemdReady(void);

#endif
