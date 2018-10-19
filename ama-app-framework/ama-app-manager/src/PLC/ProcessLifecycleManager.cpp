/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ProcessLifecycleManager.cpp
/// @brief contains the implementation of class ProcessLifecycleManager
///
/// Created by zs on 2016/10/14.
/// this file contains the implementation of class ProcessLifecycleManager
///

#include <fstream>

#include "ama_powerTypes.h"
#include "AppState.h"
#include "UnitManager.h"
#include "ProcessInfo.h"
#include "ProcessLifecycleManager.h"
#include "ShutdownManager.h"
#include "log.h"
#include "PLCTask.h"
#include "HMICTask.h"
#include "ResidualProcessList.h"
#include "RecoverAppMgr.h"
#include "LCDManager.h"
#include "COMMON/TimeStamp.h"

std::shared_ptr<FastBootManager> ProcessLifecycleManager::fastBootManager;

ProcessLifecycleManager::ProcessLifecycleManager()
{
    // initialize shutdown manager
    ShutdownManager::GetInstance()->IsShutdown();
}

ProcessLifecycleManager::~ProcessLifecycleManager()
{

}

void ProcessLifecycleManager::OnMessage(std::shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "ProcessLifecycleManager::OnMessage()");

    if(!task)
    {
        logWarn(PLC, "ProcessLifecycleManager::OnMessage(): null task pointer!");
        return;
    }

    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    PLC_CMD cmd = (PLC_CMD)plcTask->GetType();
    logInfo(PLC,"ProcessLifecycleManager::OnMessage(): taskType=", plcTask->GetTaskTypeString()," AMPID=", ToHEX(plcTask->GetAMPID()));
    switch (cmd)
    {
        case PLC_NOTIFY_IVI_SHUTDOWN:
            OnIVIShutdown(task);
            break;
        case PLC_NOTIFY_APP_SHUTDOWN_OK:
            OnAppShutdownOK(task);
            break;
        case PLC_CMD_RELEASE_FAST_BOOT_MANAGER:
            OnReleaseFastBootManager(task);
            break;
        case PLC_NOTIFY_FAST_BOOT_APP_STARTED:
            OnFastBootAppStarted(task);
            break;
        case PLC_CMD_START_SERVICE:
            OnStartService(task);
            break;
        case PLC_QUERY_APP_INFO_LIST:
            OnQueryAppInfoList(task);
            break;
        case PLC_CMD_UPDATE_APPLIST:
            OnUpdateAppList(task);
            break;
        case PLC_QUERY_IS_MODULE_INITIALIZED:
            logInfo(PLC, LOG_HEAD, "PLC is initialized.");
            break;
        default:
            logWarn(PLC, "ProcessLifecycleManager::OnMessage(): invalid command: ", static_cast<int>(cmd));
            break;
    }
}

void ProcessLifecycleManager::OnIVIShutdown(shared_ptr<PLCTask> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);

    // handle shutdown jobs
    ShutdownManager::GetInstance()->HandleIVIShutdown(static_cast<E_SHUTDOWN_TYPE>(task->GetUserData()));

    // stop booting apps
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto processes = procInfo->FindProc([](shared_ptr<Process> procObj)->bool{
        auto appObj = dynamic_pointer_cast<ActiveApp>(procObj);
        if(appObj)
        {
            return typeid(*appObj->GetState().get()) == typeid(StartingState);
        }
        return false;
    });
    for(auto& process: processes)
    {
        auto appObj = dynamic_pointer_cast<ActiveApp>(process);
        UnitManager::GetInstance()->StopUnit(appObj->GetBootCMD());
        procInfo->RemoveProc(appObj->GetAMPID());
    }
    procInfo->Unlock();

}

void ProcessLifecycleManager::OnAppShutdownOK(shared_ptr<PLCTask> task)
{
    DomainVerboseLog chatter(PLC, "ProcessLifecycleManager::OnAppShutdownOK()");

    ShutdownManager::GetInstance()->ShutdownComplete(task->GetAMPID());
}

void ProcessLifecycleManager::OnReleaseFastBootManager(shared_ptr<PLCTask> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);

    if(fastBootManager)
    {
        fastBootManager->ResendStartTasks();
        fastBootManager = nullptr;
        logInfo(PLC, LOG_HEAD, "fast boot manager is released!");
    }
    else
        logInfo(PLC, LOG_HEAD, "fast boot manager is already released!");

    // start screen monitor
    logInfo(PLC, LOG_HEAD, "start screen monitor.");
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetType(HMIC_CMD_START_SCREEN_MONITOR);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    system("echo \"center touch screen is operational\" > /dev/kmsg");
}

void ProcessLifecycleManager::OnFastBootAppStarted(std::shared_ptr<PLCTask> task)
{
    AMPID app = task->GetAMPID();
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(#%x)", app));

    if(fastBootManager)
        fastBootManager->SetAppStarted(app);
    else
        logWarn(PLC, LOG_HEAD, "fastBootManager is already released!");
}

void ProcessLifecycleManager::OnStartService(std::shared_ptr<PLCTask> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);

    auto unitName = reinterpret_cast<std::string*>(task->GetUserData());

    if(fastBootManager && !fastBootManager->IsFastBootCompleted())
    {
        fastBootManager->DelayStartTask(task);
        logWarn(PLC, LOG_HEAD, "starting service ", *unitName, " is delayed.");
        return;
    }

    logInfo(PLC, LOG_HEAD, "start: ", *unitName);
    UnitManager::GetInstance()->StartUnit(*unitName);
}

void ProcessLifecycleManager::OnUpdateAppList(std::shared_ptr<PLCTask> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);

    std::ofstream oFile("/media/ivi-data/ama.d/AppListFetchDiag.log", std::ios::app);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "ProcessLifecycleManager::OnUpdateAppList()" << std::endl;
        oFile.close();
    }
    AppList::GetInstance()->UpdateAppList();
}

void ProcessLifecycleManager::OnQueryAppInfoList(std::shared_ptr<PLCTask> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);
    typedef std::tuple<unsigned int, std::vector<AppPKGInfo>>* TuplePTR;

    auto homeampid = REF_ARG(task, TuplePTR, 0);

    auto ampidList = AppList::GetInstance()->GetInstalledAppList(homeampid);

    std::vector<AppPKGInfo>& apppkgInfos = REF_ARG(task, TuplePTR, 1);
    for(auto& ampid : ampidList)
    {
        AppPKGInfo apppkgInfo;
        apppkgInfo.ampid = ampid;
        apppkgInfo.appName = AppList::GetInstance()->GetAppName(ampid);
        apppkgInfo.itemID = AppList::GetInstance()->GetItemID(ampid);
        apppkgInfo.bigIconPath = AppList::GetInstance()->GetBigIconPath(ampid);
        apppkgInfo.smallIconPath = AppList::GetInstance()->GetSmallIconPath(ampid);
        apppkgInfo.appType = AppList::GetInstance()->GetAppType(ampid);
        apppkgInfos.push_back(apppkgInfo);
    }
}
