/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLMApp.cpp
/// @brief contains the implementation of class PLMApp
///
/// Created by zs on 2016/10/17.
/// this file contains the implementation of class PLMApp
///

#include "PLMApp.h"
#include "HMICTask.h"
#include "AppState.h"
#include "UnitManager.h"
#include "LastUserCtx.h"
#include "ResidualProcessList.h"
#include "ProcessFactory.h"
#include "ProcessInfo.h"
#include "TaskDispatcher.h"
#include "ShutdownManager.h"
#include "AppAwakeMgr.h"
#include "Process.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

PLMApp::PLMApp()
{}

PLMApp::~PLMApp()
{}

void PLMApp::OnMessage(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLMApp::OnMessage()");

    if(!task)
    {
        logWarn(PLC, "PLMApp::OnMessage(): null task pointer!");
        return;
    }

    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    PLC_CMD cmd = (PLC_CMD)plcTask->GetType();
    logInfo(PLC,"PLMApp::OnMessage(): taskType=", plcTask->GetTaskTypeString()," AMPID=", ToHEX(plcTask->GetAMPID()));
    switch (cmd)
    {
    case PLC_CMD_START:
        StartApp(plcTask);
        break;
    case PLC_CMD_STOP:
        StopApp(plcTask);
        break;
    case PLC_CMD_SHOW:
        ShowApp(plcTask);
        break;
    case PLC_CMD_HIDE:
        HideApp(plcTask);
        break;
    case PLC_CMD_KILL:
        KillApp(plcTask);
        break;
    case PLC_CMD_INTENT:
        DriveApp(plcTask);
        break;
    case PLC_CMD_NESTED_INTENT:
        OnNestedIntent(plcTask);
        break;
    case PLC_CMD_REMOVE_APP_FROM_ZORDER:
        OnRemoveAppFromZOrder(plcTask);
        break;
    case PLC_NOTIFY_START_OK:
        OnAppStartSucceeded(plcTask);
        break;
    case PLC_NOTIFY_START_NG:
        OnAppStartFailed(plcTask);
        break;
    case PLC_NOTIFY_STOP_OK:
        OnAppStopSucceeded(plcTask);
        break;
    case PLC_NOTIFY_STOP_NG:
        OnAppStopFailed(plcTask);
        break;
    case PLC_NOTIFY_APPCRUSH:
        OnAppCrushed(plcTask);
        break;
    case PLC_NOTIFY_IVI_SHUTDOWN:
        OnIVIShutdown(plcTask);
        break;
    case PLC_NOTIFY_APP_SHUTDOWN_OK:
        OnAppShutdownOK(plcTask);
        break;
    case PLC_NOTIFY_APP_BOOT_TIMEOUT:
        OnAppBootTimeout(plcTask);
        break;
    case PLC_NOTIFY_FAST_BOOT_APP_STARTED:
        OnFastBootAppStarted(task);
        break;
    case PLC_SET_LAST_MODE:
        SetLastMode(plcTask);
        break;
    default:
        logWarn(PLC, "PLMApp::OnMessage(): invalid command: ", static_cast<int>(cmd));
        break;
    }
}

void PLMApp::SetLastMode(shared_ptr<PLCTask> task)
{
    AMPID appid = task->GetAMPID();
    LastUserCtx::GetInstance()->setForeground(appid);
}
void PLMApp::StartApp(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC,"PLMApp::StartApp()");

    // block all the start commands if shutdown notify is received.
    bool isShutdown = ShutdownManager::GetInstance()->IsShuttingDown();
    if(isShutdown)
    {
        logWarn(PLC, "PLMApp::StartApp(): no app is allowed to start after shutdown notify is received.");
        return;
    }

    AMPID app = task->GetAMPID();
    E_SEAT seat = GET_SEAT(app);
    logDebug(PLC,"PLMApp::StartApp()AMPID=", ToHEX(app));

    if(fastBootManager && !fastBootManager->IsReady2Start(app))
    {
        fastBootManager->DelayStartTask(task);
        logInfo(PLC, LOG_HEAD, "app ", ToHEX(app), " is not in fast boot list. starting is delayed.");
        return;
    }

    // intent an icon
    if(AppList::GetInstance()->IsIntent(app))
    {
        logInfo(PLC, "PLMApp::StartApp(): an icon is found. intent it.");
        DispatchIntentTask(app, IntentInfo("","",task->GetUserData()));
        return;
    }

    logDebug(PLC, "PLMApp::StartApp(): looking for specified APP...");
    ProcessInfo* appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    shared_ptr<ActiveApp> appObj = appInfo->FindApp(app);
    appInfo->Unlock();
    if (appObj)
    {
        logDebug(PLC,"PLMApp::StartApp(): found");
    }
    else
    {
        logDebug(PLC,"PLMApp::StartApp(): not found, creating...");
        auto newAppObj = GetAppObject(app);
        appInfo->Lock();
        appInfo->AddProc(seat, newAppObj);
        appInfo->Unlock();
        logDebug(PLC, "PLMApp::StartApp(): done");

        appObj = newAppObj;
    }

    // send start request
    if(!appObj->GetState()->StartReq(appObj, task))
    {
        // start failed, delete object
        appInfo->Lock();
        appInfo->RemoveProc(app);
        appInfo->Unlock();
        logWarn(PLC, LOG_HEAD, "specified app failed in booting. remove.");
    }
}

void PLMApp::StopApp(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();

    shared_ptr<ActiveApp> theApp = GetApp(app);
    if (theApp)
    {
        theApp->GetState()->StopReq(theApp, task);
    }
    else
    {
        logWarn(PLC, "PLMApp::StopApp(): no active app found!");
    }
}

void PLMApp::ShowApp(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();

    logVerbose(PLC, "PLMApp::ShowApp() app:",app," --->IN");
    shared_ptr<ActiveApp> theApp = GetApp(app);
    if (theApp)
    {
        theApp->GetState()->ShowReq(theApp, task);
    }
    else
    {
        logWarn(PLC, "PLMApp::ShowApp(): no active app found!");
    }
    //set last user context ~cj 20160922
    //LastUserCtx::GetInstance()->setForeground(app);

    logVerbose(PLC, "PLMApp::ShowApp() app:",app," --->OUT");
}

void PLMApp::HideApp(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();

    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", app));
    shared_ptr<ActiveApp> theApp = GetApp(app);
    if (theApp)
    {
        theApp->GetState()->HideReq(theApp, task);
    }
    else
    {
        logWarn(PLC, LOG_HEAD, "no active app found!");
    }
}

void PLMApp::KillApp(shared_ptr<PLCTask> task)
{
    logVerbose(PLC, "PLMApp::KillApp(", ToHEX(task->GetAMPID()), ")--->IN");

    AMPID app = task->GetAMPID();

    auto theApp = GetApp(app);
    if (theApp)
    {
        theApp->GetState()->KillReq(theApp, task);
    }
    else
    {
        logWarn(PLC, "PLMApp::KillApp(): no active app found!");
    }

    logVerbose(PLC, "PLMApp::KillApp(", ToHEX(task->GetAMPID()), ")--->OUT");
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

void PLMApp::DriveApp(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMApp::DriveApp(%#x)", app));

    // intent an icon
    if(AppList::GetInstance()->IsIntent(app))
    {
        logInfo(PLC, "PLMApp::DriveApp(): nested intent is recognised! re-dispatch.");
        auto nestedIntent = task;
        nestedIntent->SetType(PLC_CMD_NESTED_INTENT);
        TaskDispatcher::GetInstance()->Dispatch(nestedIntent);
        return;
    }

    // try to find the app
    auto appObj = GetApp(task->GetAMPID());

    // get intent information
    IntentInfo *intentInfo = reinterpret_cast<IntentInfo *>(task->GetUserData());
    IntentInfo intent;
    if(intentInfo)
        intent = *intentInfo;
    logInfo(PLC, "PLMApp::DriveApp(): AMPID: ", ToHEX(app), " action: ", intent.action, " data: ", intent.data, " quiet mode: ", intent.isQuiet);

    if(appObj)
    {
        appObj->GetState()->IntentReq(appObj, task);
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
        infectTaskFunction = [app, intent](std::shared_ptr<Task> task)->bool{
            // task check
            auto hmicTask = dynamic_pointer_cast<HMICTask>(task);
            if(!hmicTask)
                return false;
            if(hmicTask->GetAMPID()!=app)
                return false;
            if(hmicTask->GetType()!=HMIC_NOTIFY_APP_START)
                return false;

            // handles of before-preshow/before-prehide task's task-done callback
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
            std::function<void(std::shared_ptr<Task>)> infect = [app, intentNotify, callbackHandles](std::shared_ptr<Task>){
                auto procInfo = ProcessInfo::GetInstance();
                procInfo->Lock();
                auto UIProcObj = procInfo->FindUIProc(app);
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
            std::function<void(std::shared_ptr<Task>)> cure = [app, callbackHandles](std::shared_ptr<Task>){
                auto procInfo = ProcessInfo::GetInstance();
                procInfo->Lock();
                auto UIProcObj = procInfo->FindUIProc(app);
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

void PLMApp::OnNestedIntent(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText("PLMApp::OnNestedIntent(%#x)", app));

    // get intent information
    IntentInfo *intentInfo = reinterpret_cast<IntentInfo *>(task->GetUserData());
    IntentInfo intent;
    if(intentInfo)
        intent = *intentInfo;

    // save intent information
    std::vector<std::shared_ptr<PLCTask>> intentStack;
    if(intentInfo)
    {
        auto task = MakeIntentTask(INVALID_AMPID, intent);
        intentStack.push_back(task);
        delete intentInfo;
    }

    // find root app
    AMPID mockingApp = app;
    while(mockingApp!=INVALID_AMPID && AppList::GetInstance()->IsIntent(mockingApp))
    {
        std::string itemID = AppList::GetInstance()->GetIntentItemID(mockingApp);
        std::string action = AppList::GetInstance()->GetIntentAction(mockingApp);
        std::string data = AppList::GetInstance()->GetIntentData(mockingApp);
        IntentInfo mockingIntent = {action, data, false};
        auto task = MakeIntentTask(INVALID_AMPID, mockingIntent);
        intentStack.push_back(task);
        mockingApp = AppList::GetInstance()->GetAMPID(itemID);
    }

    // quit if root app == INVALID_AMPID
    if(mockingApp==INVALID_AMPID)
    {
        logWarn(PLC, "PLMApp::OnNestedIntent(): root app is not found! intent is canceled.");
        return;
    }

    logInfo(PLC, "PLMApp::OnNestedIntent(): total intent layers count = ", intentStack.size());

    // set tasks' AMPID to root app
    AMPID realApp = mockingApp;
    for(auto& info: intentStack)
        info->SetAMPID(realApp);

    // send tasks
    while(!intentStack.empty())
    {
        TaskDispatcher::GetInstance()->Dispatch(intentStack.back());
        intentStack.pop_back();
    }
}

void PLMApp::OnRemoveAppFromZOrder(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();

    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", app));
    shared_ptr<ActiveApp> theApp = GetApp(app);
    if (theApp)
    {
        theApp->GetState()->RemoveAppFromZOrderReq(theApp, task);
    }
    else
    {
        logWarn(PLC, LOG_HEAD, "no active app found!");
    }
}

void PLMApp::OnAppStartSucceeded(shared_ptr<PLCTask> task)
{
    logVerbose(PLC, "PLMApp::OnAppStartSucceeded()--->IN");

    logInfo(PLC, "PLMApp::OnAppStartSucceeded(): app ", ToHEX(task->GetAMPID()), " is successfully boot.");

    // store boot app
    auto appObj = GetProc(task->GetAMPID());
    if(appObj)
    {
        ResidualProcessList::Save(appObj->GetBootCMD());

        AppStatusChangeNotify(task->GetAMPID(), appObj->GetStatus());
    }

    logVerbose(PLC, "PLMApp::OnAppStartSucceeded()--->OUT");
}

void PLMApp::OnAppStartFailed(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLMApp::OnAppStartFailed()");

    AMPID ampid = task->GetAMPID();
    if(ampid!=INVALID_AMPID)
    {
        logError(PLC, "PLMApp::OnAppStartFailed(): app ", ToHEX(task->GetAMPID()), " is failed in booting.");

        // home start failed
        if(GET_APPID(ampid)==E_APPID_HOME)
        {
            logWarn(PLC, "PLMApp::OnAppStartFailed(): home start failed. in case it's timeout, we don't delete home's record. instead, we just wait.");
            return;
        }

        AMPID app = task->GetAMPID();
        ProcessInfo* appInfo = ProcessInfo::GetInstance();
        appInfo->Lock();
        shared_ptr<ActiveApp> theApp = appInfo->FindApp(app);
        appInfo->Unlock();
        if (theApp)
        {
            theApp->StopStartupAnimationNotify();
            appInfo->Lock();
            appInfo->RemoveProc(app);
            appInfo->Unlock();
            logInfo(PLC, "PLMApp::OnAppStartFailed(): app record is deleted!");
        }
        else
        {
            logWarn(PLC, "PLMApp::OnAppStartFailed(): app record is not found!");
        }
    }
}

void PLMApp::OnAppStopSucceeded(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    logVerbose(PLC, "PLMApp::OnAppStopSucceeded(", ToHEX(app), ")--->IN");

    std::string* unitName = reinterpret_cast<std::string*>(task->GetUserData());

    // delete inactive app's record
    if(unitName)
    {
        ResidualProcessList::Delete(*unitName);

        AppStatus status;
        SET_VISIBLE(status, false);
        SET_AVAILABLE(status, false);
        AppStatusChangeNotify(task->GetAMPID(), status);
    }

    // crushed apps must be stopped at first.
    if(std::find(crushedApps.begin(), crushedApps.end(), app) != crushedApps.end())
    {
        // continue handling app crush
        OnAppCrushed(task);
    }

    logVerbose(PLC, "PLMApp::OnAppStopSucceeded(", ToHEX(app), ")--->OUT");
}

void PLMApp::OnAppStopFailed(shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    logWarn(PLC, "PLMApp::OnAppStopFailed(", ToHEX(app), ")--->IN");

    // crushed apps must be stopped at first.
    if(std::find(crushedApps.begin(), crushedApps.end(), app) != crushedApps.end())
    {
        // continue handling app crush
        OnAppCrushed(task);
    }

    logWarn(PLC, "PLMApp::OnAppStopFailed(", ToHEX(app), ")--->OUT");
}

void PLMApp::OnAppCrushed(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, formatText("PLMApp::OnAppCrushed(%#x)", task->GetAMPID()));

    // find app record
    AMPID app = task->GetAMPID();
    shared_ptr<ActiveApp> appObj = GetApp(app);

    // reboot app
    if(appObj)
    {
        if(task->GetType()==PLC_NOTIFY_APPCRUSH)
        {
            // -1. ignore duplicate crush report
            if(std::find(crushedApps.begin(), crushedApps.end(), app)!=crushedApps.end())
            {
                logWarn(PLC, "PLMApp::OnAppCrushed(): ignore duplicate crush report.");
                return;
            }

            // 0. record crushing apps
            logWarn(PLC, "PLMApp::OnAppCrushed(): record crushed app ", ToHEX(app), ".");
            crushedApps.push_back(app);

            // 1. make sure current app will not be auto reboot by systemd
            logWarn(PLC, "PLMApp::OnAppCrushed(): stop app ", ToHEX(app), ".");
            UnitManager::GetInstance()->StopUnit(app);
        }

        if(task->GetType()==PLC_NOTIFY_STOP_OK || task->GetType()==PLC_NOTIFY_STOP_NG)
        {
            // 2. clean app's information if current app is starting
            if (appObj->IsStarting()
                && GET_APPID(app)!=E_APPID_HOME // home always restart
                )
            {
                logWarn(PLC, "PLMApp::OnAppCrushed(): invoke PLMApp::OnAppBootFailed() to handle starting-time crush");
                OnAppStartFailed(task);
            }
            else
            {
                if(GET_APPID(app)==E_APPID_HOME)
                    logWarnF(PLC, "PLMApp::OnAppCrushed(): home[%#x] crushed!", app);
                else
                    logWarn(PLC, "PLMApp::OnAppCrushed(): send HMIC_NOTIFY_APP_CRUSH task to handle stable-state crush");
                auto hmicTask = std::make_shared<HMICTask>();
                hmicTask->SetType(HMIC_NOTIFY_APP_CRUSH);
                hmicTask->SetAMPID(app);
                TaskDispatcher::GetInstance()->Dispatch(hmicTask);
            }

            // 3. reboot app
            // HMIC is on it

            // 4. delete crush record
            crushedApps.erase(std::remove(crushedApps.begin(), crushedApps.end(), app));
        }
    }
}

void PLMApp::OnAppBootTimeout(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, formatText("PLMApp::OnAppBootTimeout(%#x)", task->GetAMPID()));

    logWarn(PLC, "PLMApp::OnAppBootTimeout(): timeout. handle the app as it's crushed.");
    SendCrushNotify(task->GetAMPID());
}

shared_ptr<ActiveApp> PLMApp::GetApp(AMPID app) const
{
    shared_ptr<ActiveApp> appObj;
    ProcessInfo* appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    appObj = appInfo->FindApp(app);
    appInfo->Unlock();

    return appObj;
}

shared_ptr<UIProcess> PLMApp::GetProc(AMPID proc) const
{
    shared_ptr<UIProcess> procObj;
    ProcessInfo* appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    procObj = appInfo->FindApp(proc);
    appInfo->Unlock();

    return procObj;
}
