/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ShutdownManager.cpp
/// @brief contains the implementation of class ShutdownManager
///
/// Created by zs on 2016/10/26.
/// this file contains the implementation of class ShutdownManager
///

#include "ShutdownManager.h"
#include "cs_task.h"
#include "HMICTask.h"
#include "PLCTask.h"
#include "SMTask.h"
#include "AudioTask.h"
#include "TaskDispatcher.h"
#include "ProcessInfo.h"
#include "Dependency.h"
#include "Capi.h"
#include "LCDManager.h"

#include <libShutdown.hpp>

std::string ShutdownManager::dpendentUnitName = "nodestatemanager-daemon.service";

void ShutdownCallback(LSD_ShutdownType type, void *userData)
{
    DomainVerboseLog chatter(PLC, formatText("ShutdownCallback(type=%d)", type));

    // if you got job to be done before IVI shutdown,
    // do it in ShutdownManager::HandleIVIShutdown()

    // send IVI-shutdown notify to PLC, and handle
    // it there
    auto plcTask = make_shared<PLCTask>();
    plcTask->SetType(PLC_NOTIFY_IVI_SHUTDOWN);
    plcTask->SetAMPID(INVALID_AMPID);
    plcTask->SetUserData(type);
    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}

bool InitLSD()
{
    DomainVerboseLog chatter(PLC, "InitLSD()");

    auto status = LSD_Init(ShutdownCallback, static_cast<LSD_ShutdownType>(LSD_SHUTDOWN_NORMAL|LSD_SHUTDOWN_FAST), 1000, nullptr);

    logInfo(PLC, "InitLSD(): status = ", ToHEX(static_cast<unsigned int>(status)));


    return LSD_RESULT_SUCCESS == status;
}

void OnReboot(const std::string& unitName)
{
    DomainVerboseLog chatter(PLC, formatText("OnReboot(%s)", unitName.c_str()));

    // destroy LSD
    logWarn(PLC, "OnReboot(): destroy LSD...");
    auto status = LSD_Destroy();
    logWarn(PLC, "OnReboot(): status = ", ToHEX(static_cast<unsigned>(status)));
    logWarn(PLC, "OnReboot(): done.");

    // notify apps to reboot libShutdown
    BroadcastNSMRebootNotify();

    // initialize LSD
    if(InitLSD())
        logWarn(PLC, "OnReboot(): LSD initialized again.");
    else
        logError(PLC, "OnReboot(): LSD failed initializing.");
}

ShutdownManager::ShutdownManager()
: isShutdown(false)
, isShuttingDown(false)
{
    Dependency dependency;

    dependency.SetName(dpendentUnitName);
    dependency.SetActive(InitLSD());
    dependency.SetRebootCB(OnReboot);
    RegisterDependency(dependency);
}

ShutdownManager::~ShutdownManager()
{
    DomainVerboseLog chatter(HMIC, "ShutdownManager::~ShutdownManager()");

    LSD_Destroy();
    UnregisterDependency(dpendentUnitName);
}

void ShutdownManager::HandleIVIShutdown(E_SHUTDOWN_TYPE type)
{
    FUNCTION_SCOPE_LOG_C(PLC);

    // mute
    std::thread mute([](){
        DomainVerboseLog chatter(PLC, "mute()");

        sendSetDeviceMuteTaskToAC(E_SEAT_IVI,true);
        sendSetDeviceMuteTaskToAC(E_SEAT_RSE1,true);
        sendSetDeviceMuteTaskToAC(E_SEAT_RSE2,true);
    });

    // screen off
    std::thread screenOff([](){
        DomainVerboseLog chatter(PLC, "screenOff()");

        auto screens = GetAvailableScreens();
        for(auto& screen: screens)
            LCDManager::GetInstance()->SetLCDPower(screen,false);
    });

    // save last source
    std::thread saveLastSource([](){
        DomainVerboseLog chatter(PLC, "saveLastSource()");

        auto csTask = std::make_shared<CSTask>();
        csTask->SetType(E_CS_TASK_TYPE_SAVE_CURRENT_SOURCE);
        SendQueryTaskNWait(csTask);
    });

    // wait shutdown jobs
    mute.join();
    screenOff.join();
    saveLastSource.join();

    logInfo(PLC, LOG_HEAD, "shutdown jobs are done.");

    // set shutting down flag
    isShuttingDown = true;

    // notify shutdown OK
    LSD_ShutdownComplete();

    return;
}

void ShutdownManager::ShutdownComplete(AMPID proc)
{
    DomainVerboseLog chatter(PLC, formatText("ShutdownManager::ShutdownComplete(%#x)", proc));

    return;

    // delete the record of processes that has done handling shutdown event
    processesToBeShutdown.erase(proc);

    // if every existed process had handled shutdown event in time,
    // notify the system.
    if(processesToBeShutdown.size()==0 && isShuttingDown)
    {
        logInfo(PLC, "ShutdownManager::ShutdownComplete(): all processes is shutdown, shutdown complete.");
        LSD_ShutdownComplete();
        isShutdown = true;
    }
}

bool ShutdownManager::IsShutdown()
{
    return isShutdown;
}

bool ShutdownManager::IsShuttingDown()
{
    return isShuttingDown;
}

ShutdownManager* ShutdownManager::GetInstance()
{
    static ShutdownManager shutdownManager;

    return &shutdownManager;
}
