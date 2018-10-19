/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "ProcessInfo.h"
#include "UIProcess.h"
#include "PLMApp.h"
#include "PLMUIService.h"
#include "PLMCMDService.h"
#include "PLCCtrl.h"
#include "wd.h"
#include "PLCTask.h"
#include "HMICTask.h"
#include "ResidualProcessList.h"
#include "AppList.h"
#include "UnitManager.h"
#include "AlwaysVisibleServices.h"
#include "StartService.h"
#include "Configuration.h"

using namespace std;

PLController::PLController()
: appTaskHandler(nullptr)
, UIServiceTaskHandler(nullptr)
, CMDServiceTaskHandler(nullptr)
, defTaskHandler(nullptr)
, appCrushCBHandle(0)
{
    //init those in task thread(OnModuleStartup)
    // : appTaskHandler(make_shared<PLMApp>())
    // , UIServiceTaskHandler(make_shared<PLMUIService>())
    // , CMDServiceTaskHandler(make_shared<PLMCMDService>())
    // , defTaskHandler(make_shared<ProcessLifecycleManager>())
    DomainVerboseLog chatter(PLC, "PLController::PLController()");
}

PLController::~PLController()
{
    DomainVerboseLog chatter(PLC, "PLController::~PLController()");

    if(appCrushCBHandle)
        UnitManager::GetInstance()->unitStateChangeBroadcaster.Unregister(appCrushCBHandle);
}

void PLController::doTask(shared_ptr<Task> task)
{
    if(!task)
        return;

    auto plcTask = dynamic_pointer_cast<PLCTask>(task);

    // feed the dog
    if (plcTask->IsWatchDotTask()) {
        wdSetState(E_TASK_ID_PLC, WATCHDOG_SUCCESS);
        return ;
    }


    DomainVerboseLog chatter(PLC, "PLController::doTask()");

    // print task type
    logInfo(PLC,"PLController::doTask(): taskType=", plcTask->GetTaskTypeString()," AMPID=", ToHEX(plcTask->GetAMPID()));

    // handle startup message
    if(plcTask->GetType()==PLC_NOTIFY_MODULE_STARTUP)
        return OnModuleStartup(plcTask);
    else if(plcTask->GetType()==PLC_CMD_START_AUTO_BOOT_PROCESSES)
        return OnStartAutoBootProcesses(plcTask);

    // handle the task
    shared_ptr<ProcessLifecycleManager> handler = GetHandler(task);
    if(handler)
        handler->OnMessage(plcTask);
}

shared_ptr<ProcessLifecycleManager> PLController::GetHandler(shared_ptr<Task> task)
{
    DomainVerboseLog chatter(PLC, "PLController::GetHandler()");

    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(plcTask)
    {
        AMPID proc = plcTask->GetAMPID();
        if (proc==INVALID_AMPID)
            return defTaskHandler;
        else if (IS_APP(proc))
            return appTaskHandler;
        else if (IS_SERVICE(proc) && GET_SEAT(proc)!=E_SEAT_NONE)
            return UIServiceTaskHandler;
        else if (IS_SERVICE(proc) && GET_SEAT(proc)==E_SEAT_NONE)
            return CMDServiceTaskHandler;
        else
        {
            logWarn(PLC, "PLController::GetHandler(): invalid AMPID: ", ToHEX(plcTask->GetAMPID()));
            return nullptr;
        }
    }
    else
    {
        logWarn(PLC, "PLController::GetHandler(): null task pointer.");
        return nullptr;
    }
}

void PLController::OnModuleStartup(std::shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, __STR_FUNCTIONP__);

    // add PLC start up job here.
    appTaskHandler = make_shared<PLMApp>();
    UIServiceTaskHandler = make_shared<PLMUIService>();
    CMDServiceTaskHandler = make_shared<PLMCMDService>();
    defTaskHandler = make_shared<ProcessLifecycleManager>();

    UnitManager::GetInstance()->init();

    if(IsFastBootEnabled())
        AppList::GetInstance()->fastInit();

    // 0. set app crush callback
    logInfo(PLC, LOG_HEAD, "set app crush callback");
    SetAppCrushCB();

    // 1. remove residual process
    logInfo(PLC, LOG_HEAD, "remove residual process");
    auto records = ResidualProcessList::ReadRecords();
    for(auto& record: records)
        UnitManager::GetInstance()->StopUnit(record);

    // fast boot
    if(IsFastBootEnabled())
    {
        // 2. proceed rapid start
        logInfo(PLC, LOG_HEAD, "proceed rapid start");
        RapidStart();

        // 2.5 enable fast-boot manager
        logInfo(PLC, LOG_HEAD, "enable fast-boot manager");
        ProcessLifecycleManager::fastBootManager = std::make_shared<FastBootManager>();
        ProcessLifecycleManager::fastBootManager->RegisterCallback();
        ProcessLifecycleManager::fastBootManager->StartTiming();
    }
    else
        logInfo(PLC, LOG_HEAD, "fast boot is disabled.");

    // 3. read always-visible-services
    logInfo(PLC, LOG_HEAD, "read always-visible-services");
    LoadAlwaysVisibleServices();

    // 4. fully initialize AppList
    logInfo(PLC, LOG_HEAD, "fully initialize AppList");
    AppList::GetInstance()->clear();
    AppList::GetInstance()->init();
}

void PLController::OnStartAutoBootProcesses(std::shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "PLController::OnStartAutoBootProcesses()");

    StartAutoStartProcesses();
}

void PLController::SetAppCrushCB()
{
    DomainVerboseLog chatter(PLC, "PLController::SetAppCrushCB()");

    appCrushCBHandle = UnitManager::GetInstance()->unitStateChangeBroadcaster.Register([](const std::string& unitName, bool isActive){
        if(!isActive)
        {
            // send app crush task
            auto procInfo = ProcessInfo::GetInstance();
            procInfo->Lock();
            auto procObj = procInfo->FindProc(unitName);
            if(procObj && !procObj->IsStopping())
            {
                DomainLog chatter(PLC, LOG_WARN, formatText("PLController::AppCrushCB(%s)", unitName.c_str()));
                SendCrushNotify(procObj);
            }
            procInfo->Unlock();
        }
    });
}

void PLController::RapidStart()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    auto apps = AppList::GetInstance()->fastBootList.GetFastBootApps();
    auto seats = GetAvailableScreens(true);
    std::set<E_SEAT> seatSet(seats.begin(), seats.end());
    for(auto& app: apps)
    {
        auto seat = GET_SEAT(app.getAMPID());
        if(seat!=E_SEAT_NONE) // seatless service is always bootable
            if(seatSet.find(seat)==seatSet.end())
                continue;

        auto task = make_shared<PLCTask>();
        task->SetAMPID(app.getAMPID());
        task->SetType(PLC_CMD_START);
        // TODO: redesign this
        if((0x2001==app.getAMPID())||(0x3001==app.getAMPID()))
            task->SetUserData(false); // set quiet start info
        else
            task->SetUserData(true); // set quiet start info
        doTask(task);
    }
}
