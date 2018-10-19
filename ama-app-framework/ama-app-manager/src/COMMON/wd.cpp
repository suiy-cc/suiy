/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "wd.h"

#include <cassert>

#include <iostream>
#include <algorithm>
#include <map>
#include <mutex>

#include <systemd/sd-daemon.h>

#include "TaskDispatcher.h"
#include "cs_task.h"
#include "HMICTask.h"
#include "PLCTask.h"
#include "SMTask.h"
#include "AudioTask.h"
#include "PMCTask.h"

using namespace std;

extern LOG_DECLARE_CONTEXT(PROF);

std::vector<int> gTaskStates(E_TASK_ID_NUM);
std::mutex gMtx;

void wdInit(void)
{
	logVerbose("wdInit IN");
	for (auto & ts : gTaskStates) {
		ts = WATCHDOG_FAILURE;
	}
	logVerbose("wdInit OUT");
}

void wdCheckAllStates(void)
{
	//logVerbose("wdCheckAllStates IN");
#ifdef ENABLE_DEBUG_MODE
	logDebug("wdCheckAllStates()-->IN");
#endif
	bool flags = true;
	{
		std::lock_guard<std::mutex> lck (gMtx);
		for(int i = 0; i < gTaskStates.size(); ++i)
        {
            if(gTaskStates[i]!=WATCHDOG_SUCCESS)
            {
                logError(PROF, "wdCheckAllStates(): watchdog check failure! module number: ", i);
                flags = false;
                //break; // don't stop so that you can figure out all the threads who didn't feed dog.
            }
		}
		for (auto & ts : gTaskStates) {
			ts = WATCHDOG_FAILURE;
		}
    }
	if (flags) {
			sd_notify(0, "WATCHDOG=1");
			#ifdef ENABLE_DEBUG_MODE
				logDebug("wdCheckAllStates()-->send WATCHDOG=1");
			#endif
	} else {
			logError(PROF, "Watchdog is timeout !");
	}

#ifdef ENABLE_DEBUG_MODE
	logDebug("wdCheckAllStates()-->OUT");
#endif
}

void wdSetState(WdTaskId tid, int flag)
{
#ifdef ENABLE_DEBUG_MODE
	logDebug("wdSetState()-->IN tid:",tid ,"flag:",flag);
#endif
	std::lock_guard<std::mutex> lck (gMtx);
	assert(0 <= tid && tid < E_TASK_ID_NUM);
	assert(flag == WATCHDOG_SUCCESS || flag == WATCHDOG_FAILURE);
	gTaskStates[tid] = flag;
#ifdef ENABLE_DEBUG_MODE
	logDebug("wdSetState()-->OUT");
#endif
}

void wdSendWatchdogTask(void)
{
#ifdef ENABLE_DEBUG_MODE
	logDebug("wdSendWatchdogTask()-->IN");
#endif
	auto csTask = make_shared<CSTask>();
	csTask->SetPriority(E_TASK_PRIORITY_HIGH);
	csTask->SetThisIsWatchDogTask();
	TaskDispatcher::GetInstance()->Dispatch(csTask);
	auto plcTask = make_shared<PLCTask>();
	plcTask->SetPriority(E_TASK_PRIORITY_HIGH);
	plcTask->SetThisIsWatchDogTask();
	TaskDispatcher::GetInstance()->Dispatch(plcTask);
	auto hmicTask = make_shared<HMICTask>();
	hmicTask->SetPriority(E_TASK_PRIORITY_HIGH);
	hmicTask->SetThisIsWatchDogTask();
	TaskDispatcher::GetInstance()->Dispatch(hmicTask);
	auto smTask = make_shared<SMTask>();
	smTask->SetPriority(E_TASK_PRIORITY_HIGH);
	smTask->SetThisIsWatchDogTask();
	TaskDispatcher::GetInstance()->Dispatch(smTask);
	auto acTask = make_shared<AudioTask>();
	acTask->SetPriority(E_TASK_PRIORITY_HIGH);
	acTask->SetThisIsWatchDogTask();
	TaskDispatcher::GetInstance()->Dispatch(acTask);
	auto pmcTask = make_shared<PMCTask>();
	pmcTask->SetPriority(E_TASK_PRIORITY_HIGH);
	pmcTask->SetThisIsWatchDogTask();
	TaskDispatcher::GetInstance()->Dispatch(pmcTask);

#ifdef ENABLE_DEBUG_MODE
	logDebug("wdSendWatchdogTask()-->OUT");
#endif
}

void wdNotifySystemdReady(void)
{
	logVerbose("wdNotifySystemdReady IN");
	sd_notify(0, "READY=1");
	logVerbose("wdNotifySystemdReady OUT");
}
