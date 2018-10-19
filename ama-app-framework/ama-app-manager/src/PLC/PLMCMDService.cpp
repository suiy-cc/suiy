/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLMCMDService.cpp
/// @brief contains the implementation of class PLMCMDService
///
/// Created by zs on 2017/11/2.
/// this file contains the implementation of class PLMCMDService
///

#include "UnitManager.h"
#include "ResidualProcessList.h"
#include "ProcessInfo.h"
#include "PLMCMDService.h"
#include "ProcessFactory.h"
#include "ActiveService.h"
#include "TaskDispatcher.h"
#include "ShutdownManager.h"
#include "Process.h"
#include "RebootFrequency.h"

PLMCMDService::PLMCMDService()
{}

PLMCMDService::~PLMCMDService()
{}

void PLMCMDService::OnMessage(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLMCMDService::OnMessage()");

    if(!task)
    {
        logWarn(PLC, "PLMCMDService::OnMessage(): null task pointer!");
        return;
    }

    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    PLC_CMD cmd = (PLC_CMD)plcTask->GetType();
    logInfo(PLC,"PLMCMDService::OnMessage(): taskType=", plcTask->GetTaskTypeString()," AMPID=", ToHEX(plcTask->GetAMPID()));
    switch (cmd)
    {
        case PLC_CMD_START:
            OnStart(plcTask);
            break;
        case PLC_CMD_STOP:
            OnStop(plcTask);
            break;
        case PLC_CMD_KILL:
            OnKill(plcTask);
            break;
        case PLC_CMD_INTENT:
            OnIntent(plcTask);
            break;
        case PLC_NOTIFY_START_OK:
            OnStartSucceeded(plcTask);
            break;
        case PLC_NOTIFY_START_NG:
            OnStartFailed(plcTask);
            break;
        case PLC_NOTIFY_STOP_OK:
            OnStopSucceeded(plcTask);
            break;
        case PLC_NOTIFY_STOP_NG:
            OnStopFailed(plcTask);
            break;
        case PLC_NOTIFY_APPCRUSH:
            OnCrush(plcTask);
            break;
        case PLC_NOTIFY_IVI_SHUTDOWN:
            OnIVIShutdown(plcTask);
            break;
        case PLC_NOTIFY_APP_SHUTDOWN_OK:
            OnAppShutdownOK(plcTask);
            break;
        case PLC_NOTIFY_FAST_BOOT_APP_STARTED:
            OnFastBootAppStarted(task);
            break;
        default:
            logWarn(PLC, "PLMCMDService::OnMessage(): invalid command: ", plcTask->GetTaskTypeString());
            break;
    }
}

void PLMCMDService::OnStart(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMCMDService::OnStart()");

    // block all the start commands if shutdown notify is received.
    bool isShutdown = ShutdownManager::GetInstance()->IsShuttingDown();
    if(isShutdown)
    {
        logInfo(PLC, "PLMCMDService::OnStart(): no service is allowed to start after shutdown notify is received.");
        return;
    }

    AMPID proc = task->GetAMPID();
    E_SEAT seat = GET_SEAT(proc);
    logDebug(PLC,"PLMCMDService::OnStart(): AMPID=", ToHEX(proc));

    if(fastBootManager && !fastBootManager->IsReady2Start(proc))
    {
        fastBootManager->DelayStartTask(task);
        logWarn(PLC, LOG_HEAD, "app ", ToHEX(proc), " is not in fast boot list. starting is delayed.");
        return;
    }

    logDebug(PLC, "PLMCMDService::OnStart(): looking for specified Service...");
    ProcessInfo* appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    auto procObj = appInfo->FindProc(proc);
    auto CMDServiceObj = dynamic_pointer_cast<ActiveService>(procObj);
    appInfo->Unlock();
    if (CMDServiceObj)
    {
        logDebug(PLC,"PLMCMDService::OnStart(): found");
    }
    else
    {
        logDebug(PLC,"PLMCMDService::OnStart(): not found, creating...");
        auto serviceObj = GetCMDServiceObject(proc);
        appInfo->Lock();
        appInfo->AddProc(seat, serviceObj);
        appInfo->Unlock();
        logDebug(PLC, "PLMCMDService::OnStart(): done");

        CMDServiceObj = serviceObj;
    }

    // send start request
    if(!CMDServiceObj->GetState()->StartReq(CMDServiceObj, task))
    {
        appInfo->Lock();
        appInfo->RemoveProc(proc);
        appInfo->Unlock();
        logWarn(PLC, "PLMCMDService::OnStart(): specified app failed in booting. remove.");
    }
}

void PLMCMDService::OnStop(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMCMDService::OnStop()");

    AMPID proc = task->GetAMPID();

    auto CMDService = GetCMDService(proc);
    if (CMDService)
    {
        // stop service
        CMDService->GetState()->StopReq(CMDService, task);
    }
    else
    {
        logInfo(PLC, "PLMCMDService::OnStop(): no service found!");
    }
}

void PLMCMDService::OnKill(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLMCMDService::OnKill()");

    AMPID proc = task->GetAMPID();

    auto serviceObj = GetCMDService(proc);
    if (serviceObj)
    {
        serviceObj->GetState()->KillReq(serviceObj, task);
    }
    else
    {
        logInfo(PLC, "PLMCMDService::OnKill(): no active app found!");
    }
}

void PLMCMDService::OnIntent(shared_ptr<PLCTask> task)
{
    AMPID proc = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMCMDService::OnIntent(%#x)", proc));

    if(task)
    {
        auto serviceObj = GetCMDService(task->GetAMPID());

        // get intent information
        IntentInfo *intentInfo = reinterpret_cast<IntentInfo *>(task->GetUserData());
        IntentInfo intent;
        if(intentInfo)
            intent = *intentInfo;
        logInfo(PLC, "PLMCMDService::OnIntent(): AMPID: ", ToHEX(proc), " action: ", intent.action, " data: ", intent.data, " quiet mode: ", intent.isQuiet);

        if(serviceObj)
        {
            serviceObj->GetState()->IntentReq(serviceObj, task);
        }
        else
        {
            logInfo(PLC, "PLMCMDService::OnIntent(): service is not found, send start task.");
            // prepare start task for current service
            auto plcTask = make_shared<PLCTask>();
            plcTask->SetAMPID(task->GetAMPID());
            plcTask->SetType(PLC_CMD_START);

            // set task-done callback, so that current intent
            // can be sent to service when the service is ready.
            plcTask->SetTaskDoneCB([this, proc, task](std::shared_ptr<Task>){
                auto serviceObj = GetCMDService(proc);
                if(serviceObj)
                    serviceObj->AddDeferredTask(task);
            });

            // send the task
            TaskDispatcher::GetInstance()->Dispatch(plcTask);
        }
    }
}

void PLMCMDService::OnStartSucceeded(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMCMDService::OnStartSucceeded()");

    logInfo(PLC, "PLMCMDService::OnAppStartSucceeded(): app ", ToHEX(task->GetAMPID()), " is successfully boot.");

    auto serviceObj = GetCMDService(task->GetAMPID());
    if(serviceObj)
    {
        // store boot app
        ResidualProcessList::Save(serviceObj->GetBootCMD());

        // modify service state
        serviceObj->SetState(std::make_shared<ProcActiveState>());

        if(fastBootManager)
            fastBootManager->SetAppStarted(task->GetAMPID());
    }
}

void PLMCMDService::OnStartFailed(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMCMDService::OnStartFailed()");

    AMPID ampid = task->GetAMPID();
    if(ampid!=INVALID_AMPID)
    {
        logError(PLC, "PLMCMDService::OnAppStartFailed(): app ", ToHEX(task->GetAMPID()), " is failed in booting.");

        AMPID proc = task->GetAMPID();
        ProcessInfo* procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto service = procInfo->FindProc(proc);
        procInfo->Unlock();
        if (service)
        {
            procInfo->Lock();
            procInfo->RemoveProc(proc);
            procInfo->Unlock();
            logInfo(PLC, "PLMCMDService::OnAppStartFailed(): service record is deleted!");
        }
        else
        {
            logInfo(PLC, "PLMCMDService::OnAppStartFailed(): service record is not found!");
        }
    }
}

void PLMCMDService::OnStopSucceeded(shared_ptr<PLCTask> task)
{
    AMPID service = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMCMDService::OnStopSucceeded(%#x)", service));

    std::string* unitName = reinterpret_cast<std::string*>(task->GetUserData());

    // delete inactive app's record
    if(unitName)
    {
        ResidualProcessList::Delete(*unitName);
    }
}

void PLMCMDService::OnStopFailed(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMCMDService::OnStopFailed(%#x)", app));
}

void PLMCMDService::OnCrush(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, formatText("PLMCMDService::OnCrush(%#x)", task->GetAMPID()));

    // find app record
    AMPID proc = task->GetAMPID();
    auto serviceObj = GetCMDService(proc);

    // reboot app
    if(serviceObj)
    {
        // 1. make sure current app will not be auto reboot by systemd
        logInfo(PLC, "PLMCMDService::OnCrush(): stop service ", ToHEX(proc), ".");
        UnitManager::GetInstance()->StopUnit(proc);

        // 2. clean service's information if current service is starting
        OnStartFailed(task);

        if (serviceObj->IsOperating())
        {
            // notify class RebootFrequency, so that crush frequency is calculatable.
            RebootFrequency::GetInstance()->SetCrushed(proc);

            if(!RebootFrequency::GetInstance()->IsTooFrequent(proc))
            {
                // 3. reboot service if current service is a stable service
                logInfo(PLC, "PLMCMDService::OnCrush(): start specified service");
                auto plcTask = std::make_shared<PLCTask>();
                plcTask->SetType(PLC_CMD_START);
                plcTask->SetAMPID(proc);
                TaskDispatcher::GetInstance()->Dispatch(plcTask);
            }
        }
    }
}

shared_ptr<ActiveService> PLMCMDService::GetCMDService(AMPID ampid)
{
    DomainVerboseLog chatter(PLC, formatText("PLMCMDService::GetCMDService(%#x)", ampid));

    auto appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    auto procObj = appInfo->FindProc(ampid);
    appInfo->Unlock();

    return dynamic_pointer_cast<ActiveService>(procObj);
}