/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLMUIService.cpp
/// @brief contains the implementation of class PLMUIService
///
/// Created by zs on 2016/10/17.
/// this file contains the implementation of class PLMUIService
///

#include "UnitManager.h"
#include "ResidualProcessList.h"
#include "ProcessInfo.h"
#include "PLMUIService.h"
#include "ProcessFactory.h"
#include "ActiveUIService.h"
#include "HMICTask.h"
#include "TaskDispatcher.h"
#include "ShutdownManager.h"
#include "AppState.h"
#include "Process.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

PLMUIService::PLMUIService()
{}

PLMUIService::~PLMUIService()
{}

void PLMUIService::OnMessage(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLMUIService::OnMessage()");

    if(!task)
    {
        logWarn(PLC, "PLMUIService::OnMessage(): null task pointer!");
        return;
    }

    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    PLC_CMD cmd = (PLC_CMD)plcTask->GetType();
    logInfo(PLC,"PLMUIService::OnMessage(): taskType=", plcTask->GetTaskTypeString()," AMPID=", ToHEX(plcTask->GetAMPID()));
    switch (cmd)
    {
        case PLC_CMD_START:
            OnStart(plcTask);
            break;
        case PLC_CMD_STOP:
            OnStop(plcTask);
            break;
        case PLC_CMD_SHOW:
            OnShow(plcTask);
            break;
        case PLC_CMD_HIDE:
            OnHide(plcTask);
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
            logWarn(PLC, "PLMUIService::OnMessage(): invalid command: ", plcTask->GetTaskTypeString());
            break;
    }
}

void PLMUIService::OnStart(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMUIService::OnStart()");

    // block all the start commands if shutdown notify is received.
    bool isShutdown = ShutdownManager::GetInstance()->IsShuttingDown();
    if(isShutdown)
    {
        logWarn(PLC, "PLMUIService::OnStart(): no service is allowed to start after shutdown notify is received.");
        return;
    }

    AMPID proc = task->GetAMPID();
    E_SEAT seat = GET_SEAT(proc);
    logDebug(PLC,"PLMUIService::OnStart(): AMPID=", ToHEX(proc));

    if(fastBootManager && !fastBootManager->IsReady2Start(proc))
    {
        fastBootManager->DelayStartTask(task);
        logInfo(PLC, LOG_HEAD, "app ", ToHEX(proc), " is not in fast boot list. starting is delayed.");
        return;
    }

    logDebug(PLC, "PLMUIService::OnStart(): looking for specified Service...");
    ProcessInfo* appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    auto procObj = appInfo->FindUIProc(proc);
    auto UIServiceObj = dynamic_pointer_cast<ActiveUIService>(procObj);
    appInfo->Unlock();
    if (UIServiceObj)
    {
        logDebug(PLC,"PLMUIService::OnStart(): found");
    }
    else
    {
        logDebug(PLC,"PLMUIService::OnStart(): not found, creating...");
        auto serviceObj = GetUIServiceObject(proc);
        appInfo->Lock();
        appInfo->AddProc(seat, serviceObj);
        appInfo->Unlock();
        logDebug(PLC, "PLMUIService::OnStart(): done");

        UIServiceObj = serviceObj;
    }

    // send start request
    if(!UIServiceObj->GetState()->StartReq(UIServiceObj, task))
    {
        appInfo->Lock();
        appInfo->RemoveProc(proc);
        appInfo->Unlock();
        logWarn(PLC, LOG_HEAD, "specified app failed in booting. remove.");
    }
    else
    {
        // set service visible if current service is an auto-visible service
        if(UIServiceObj->IsAlwaysVisible())
        {
            auto plcTask = std::make_shared<PLCTask>();
            plcTask->SetType(PLC_CMD_SHOW);
            plcTask->SetAMPID(proc);

            TaskDispatcher::GetInstance()->Dispatch(plcTask);
        }
    }
}

void PLMUIService::OnStop(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMUIService::OnStop()");

    AMPID proc = task->GetAMPID();

    auto UIService = GetUIService(proc);
    if (UIService)
    {
        // stop app
        UIService->GetState()->StopReq(UIService, task);
    }
    else
    {
        logInfo(PLC, "PLMUIService::OnStop(): no service found!");
    }
}

void PLMUIService::OnShow(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMUIService::OnShow()");

    AMPID proc = task->GetAMPID();

    // find specified service
    auto serviceObj = GetUIService(proc);
    if (serviceObj)
    {
        // show service
        serviceObj->GetState()->ShowReq(serviceObj, task);
    }
    else
    {
        logInfo(PLC, "PLMUIService::ShowApp(): no active app found!");
    }
}

void PLMUIService::OnHide(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMUIService::OnHide()");

    AMPID proc = task->GetAMPID();

    // find specified service
    auto serviceObj  = GetUIService(proc);
    if (serviceObj)
    {
        // hide service
        serviceObj->GetState()->HideReq(serviceObj, task);
    }
    else
    {
        logInfo(PLC, "PLMApp::HideApp(): no active app found!");
    }
}

void PLMUIService::OnKill(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLMUIService::OnKill()");

    AMPID proc = task->GetAMPID();

    auto serviceObj = GetUIService(proc);
    if (serviceObj)
    {
        serviceObj->GetState()->KillReq(serviceObj, task);
    }
    else
    {
        logInfo(PLC, "PLMUIService::OnKill(): no active app found!");
    }
}

static void IntentNotify(AMPID app, const IntentInfo& intent)
{
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto UIProcObj = procInfo->FindUIProc(app);
    if(UIProcObj)
        UIProcObj->IntentNotify(intent);
    procInfo->Unlock();
};

void PLMUIService::OnIntent(shared_ptr<PLCTask> task)
{
    AMPID proc = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMUIService::OnIntent(%#x)", proc));

    if(task)
    {
        auto serviceObj = GetUIService(task->GetAMPID());

        // get intent information
        IntentInfo *intentInfo = reinterpret_cast<IntentInfo *>(task->GetUserData());
        IntentInfo intent;
        if(intentInfo)
            intent = *intentInfo;
        logInfo(PLC, "PLMUIService::OnIntent(): AMPID: ", ToHEX(proc), " action: ", intent.action, " data: ", intent.data, " quiet mode: ", intent.isQuiet);

        if(serviceObj)
        {
            serviceObj->GetState()->IntentReq(serviceObj, task);
        }
        else
        {
            delete intentInfo;

            // send start task
            auto plcTask = make_shared<PLCTask>();
            plcTask->SetAMPID(task->GetAMPID());
            plcTask->SetType(PLC_CMD_START);
            plcTask->SetUserData(intent.isQuiet); // set quiet start info
            TaskDispatcher::GetInstance()->Dispatch(plcTask);

            // send intent notify when app is ready to show/hide
            std::function<bool(std::shared_ptr<Task>)> infectTaskFunction;
            infectTaskFunction = [proc, intent](std::shared_ptr<Task> task)->bool{
                // task check
                auto hmicTask = dynamic_pointer_cast<HMICTask>(task);
                if(!hmicTask)
                    return false;
                if(hmicTask->GetAMPID()!=proc)
                    return false;
                if(hmicTask->GetType()!=HMIC_NOTIFY_APP_START)
                    return false;

                // callback handles
                unsigned int* callbackHandles = new unsigned int[2];
                callbackHandles[0] = 0;
                callbackHandles[1] = 0;
                hmicTask->SetTaskDoneCBData(reinterpret_cast<unsigned long long>(callbackHandles)
                                            ,[](unsigned long long data){delete []reinterpret_cast<unsigned int*>(data);});

                // prepare an intent function object
                std::function<void(AMPID)> intentNotify = [intent](AMPID app){
                    IntentNotify(app, intent);
                };

                // infection is here
                // add intent notify into app started task
                std::function<void(std::shared_ptr<Task>)> infect = [proc, intentNotify, callbackHandles](std::shared_ptr<Task>){
                    auto procInfo = ProcessInfo::GetInstance();
                    procInfo->Lock();
                    auto UIProcObj = procInfo->FindUIProc(proc);
                    if(UIProcObj)
                    {
                        callbackHandles[0] = UIProcObj->beforePreShowBroadcaster.Register(intentNotify);
                        callbackHandles[1] = UIProcObj->beforePreHideBroadcaster.Register(intentNotify);
                    }
                    procInfo->Unlock();
                };
                // cure is here
                // delete intent notify when start task is done.
                // so that no extra intent notify is sent to app.
                std::function<void(std::shared_ptr<Task>)> cure = [proc, callbackHandles](std::shared_ptr<Task>){
                    auto procInfo = ProcessInfo::GetInstance();
                    procInfo->Lock();
                    auto UIProcObj = procInfo->FindUIProc(proc);
                    if(UIProcObj)
                    {
                        UIProcObj->beforePreShowBroadcaster.Unregister(callbackHandles[0]);
                        UIProcObj->beforePreHideBroadcaster.Unregister(callbackHandles[1]);
                    }
                    procInfo->Unlock();
                };
                hmicTask->SetPreTaskDoneCB(infect);
                hmicTask->SetTaskDoneCB(cure);

                return true;
            };
            TaskDispatcher::GetInstance()->dispatchedTaskBroadcaster.Register(infectTaskFunction);

            // incase app-started-task never come,
            // we unregister it when app is stop
            // TODO: write a good flow of app stop, and
            // implement this
        }
    }
}

void PLMUIService::OnStartSucceeded(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMUIService::OnStartSucceeded()");

    logInfo(PLC, "PLMUIService::OnAppStartSucceeded(): app ", ToHEX(task->GetAMPID()), " is successfully boot.");

    auto serviceObj = GetUIService(task->GetAMPID());
    if(serviceObj)
    {
        // store boot app
        ResidualProcessList::Save(serviceObj->GetBootCMD());
    }
}

void PLMUIService::OnStartFailed(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMUIService::OnStartFailed()");

    AMPID ampid = task->GetAMPID();
    if(ampid!=INVALID_AMPID)
    {
        logError(PLC, "PLMUIService::OnAppStartFailed(): app ", ToHEX(task->GetAMPID()), " is failed in booting.");

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
            logInfo(PLC, "PLMUIService::OnAppStartFailed(): service record is deleted!");
        }
        else
        {
            logInfo(PLC, "PLMUIService::OnAppStartFailed(): service record is not found!");
        }
    }
}

void PLMUIService::OnStopSucceeded(shared_ptr<PLCTask> task)
{
    AMPID service = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMUIService::OnStopSucceeded(%#x)", service));

    std::string* unitName = reinterpret_cast<std::string*>(task->GetUserData());

    // delete inactive app's record
    if(unitName)
    {
        ResidualProcessList::Delete(*unitName);
    }

    // crushed apps must be stopped at first.
    if(std::find(crushedServices.begin(), crushedServices.end(), service) != crushedServices.end())
    {
        // continue handling app crush
        OnCrush(task);
    }
}

void PLMUIService::OnStopFailed(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMUIService::OnStopFailed(%#x)", app));

    // crushed apps must be stopped at first.
    if(std::find(crushedServices.begin(), crushedServices.end(), app) != crushedServices.end())
    {
        // continue handling app crush
        OnCrush(task);
    }
}

void PLMUIService::OnCrush(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, formatText("PLMUIService::OnCrush(%#x)", task->GetAMPID()));

    // find app record
    AMPID proc = task->GetAMPID();
    auto serviceObj = GetUIService(proc);

    // reboot app
    if(serviceObj)
    {
        if(task->GetType()==PLC_NOTIFY_APPCRUSH)
        {
            // -1. ignore duplicate crush report
            if(std::find(crushedServices.begin(), crushedServices.end(), proc)!=crushedServices.end())
            {
                logWarn(PLC, "PLMApp::OnAppCrushed(): ignore duplicate crush report.");
                return;
            }

            // 0. record crushing apps
            logInfo(PLC, "PLMUIService::OnCrush(): record crushed app ", ToHEX(proc), ".");
            crushedServices.push_back(proc);

            // 1. make sure current app will not be auto reboot by systemd
            logInfo(PLC, "PLMUIService::OnCrush(): stop service ", ToHEX(proc), ".");
            UnitManager::GetInstance()->StopUnit(proc);
        }

        if(task->GetType()==PLC_NOTIFY_STOP_OK || task->GetType()==PLC_NOTIFY_STOP_NG)
        {
            // 2. clean service's information if current service is starting
            if (serviceObj->IsStarting())
            {
                logInfo(PLC, "PLMUIService::OnCrush(): invoke PLMUIService::OnStartFailed() to handle starting-time crush");
                OnStartFailed(task);
            }
            else
            {
                // 2. reboot service if current service is a stable service
                logInfo(PLC, "PLMUIService::OnCrush(): send HMIC_NOTIFY_APP_CRUSH task to handle stable-state crush");
                auto hmicTask = std::make_shared<HMICTask>();
                hmicTask->SetType(HMIC_NOTIFY_APP_CRUSH);
                hmicTask->SetAMPID(proc);
                TaskDispatcher::GetInstance()->Dispatch(hmicTask);
            }

            // 3. reboot service
            // HMIC is on it

            // 4. delete crush record
            crushedServices.erase(std::remove(crushedServices.begin(), crushedServices.end(), proc));
        }
    }
}

shared_ptr<ActiveUIService> PLMUIService::GetUIService(AMPID ampid)
{
    DomainVerboseLog chatter(PLC, formatText("PLMUIService::GetUIService(%#x)", ampid));

    auto appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    auto procObj = appInfo->FindProc(ampid);
    appInfo->Unlock();

    return dynamic_pointer_cast<ActiveUIService>(procObj);
}
