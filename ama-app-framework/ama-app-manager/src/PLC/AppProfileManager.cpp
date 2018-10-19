/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppProfileManager.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/09/18.
///

/// this file contains the implementation of class AppProfileManager
///

#include <mutex>

#include "log.h"
#include "UIProcess.h"
#include "UnitManager.h"
#include "ProcessInfo.h"
#include "AppProfileManager.h"
#include "COMMON/TimeStamp.h"

const std::string AppProfileManager::dir = "/tmp/";

AppProfileManager::AppProfileManager()
: startProcCBHandle(0)
, startedCBHandle(0)
, showCBHandle(0)
{
    RegisterCB();
}

AppProfileManager::~AppProfileManager()
{
    UnregisterCB();
}

AppProfileManager* AppProfileManager::GetInstance()
{
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static AppProfileManager manager;
    return &manager;
}

void AppProfileManager::RegisterCB()
{
    logInfo(PLC, LOG_HEAD, "enter");
    logInfo(PLC, LOG_HEAD, "Process::appStateChangeBroadcaster.Register()...");
    startProcCBHandle = Process::appStateChangeBroadcaster.Register([this](AMPID app, E_PROC_STATE state){
        if(state==E_PROC_STATE_STARTING)
            OnStartCMDSent(app);
    });
    logInfo(PLC, LOG_HEAD, "Process::appStateChangeBroadcaster.Register() OK");
    logInfo(PLC, LOG_HEAD, "UnitManager::GetInstance()->unitStateChangeBroadcaster.Register()...");
    startedCBHandle = UnitManager::GetInstance()->unitStateChangeBroadcaster.Register([this](const std::string& unitName, bool isActive){
        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto UIProc = procInfo->FindUIProc(unitName);
        procInfo->Unlock();
        if(!UIProc)
            return;
        else if(isActive && UIProc)
            OnStarted(UIProc->GetAMPID());
    });
    logInfo(PLC, LOG_HEAD, "UnitManager::GetInstance()->unitStateChangeBroadcaster.Register() OK");
    logInfo(PLC, LOG_HEAD, "Process::appStateChangeBroadcaster.Register()...");
    showCBHandle = Process::appStateChangeBroadcaster.Register([this](AMPID app, E_PROC_STATE state){
        if(state==E_APP_STATE_SHOW)
            OnSurfaceShow(app);
        if(state==E_APP_STATE_HIDE)
            OnSurfaceHide(app);
    });
    logInfo(PLC, LOG_HEAD, "Process::appStateChangeBroadcaster.Register() OK");
    logInfo(PLC, LOG_HEAD, "exit");
}

void AppProfileManager::UnregisterCB()
{
    Process::appStateChangeBroadcaster.Unregister(startProcCBHandle);
    UnitManager::GetInstance()->unitStateChangeBroadcaster.Unregister(startedCBHandle);
    Process::appStateChangeBroadcaster.Unregister(showCBHandle);
}

void AppProfileManager::OnAppStart(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)!=profileMap.end())
            return;

        profileMap[app].SetItemID(AppList::GetInstance()->GetItemID(app));
        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x start command is received.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnStartCMDSent(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        profileMap[app].SetItemID(AppList::GetInstance()->GetItemID(app));
        auto timeStamp = TimeStamp::GetString();
        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x start command is sent to systemd.", timeStamp.c_str(), app));
    });
}

void AppProfileManager::OnStarted(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x is started by systemd.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnAppSDKBootLogReceived(AMPID app, const std::vector<std::string>& bootLog)
{
    profiles.Operate([app, &bootLog](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        for(auto& log: bootLog)
            profileMap[app].AppendLog(log);
    });
}

void AppProfileManager::OnSurfaceCreated(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x 's surface is created.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnSurfaceConfigured(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x 's surface is configured.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnPreshowIn(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x is handling preshow/prehide notify.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnPreshowOut(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x 's preshow/prehide notify is handled.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnSurfaceShow(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x 's surface is visible. app is started, but loading-screen is still on.", TimeStamp::GetString().c_str(), app));
    });
}

void AppProfileManager::OnSurfaceHide(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x 's surface is hide. app is started.", TimeStamp::GetString().c_str(), app));

        profileMap[app].Serialize(dir);

        profileMap.erase(app);
    });
}

void AppProfileManager::OnLoadingScreenEnd(AMPID app)
{
    profiles.Operate([app](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x 's loading-screen is hide. app is visible for user.", TimeStamp::GetString().c_str(), app));

        profileMap[app].Serialize(dir);

        profileMap.erase(app);
    });
}

void AppProfileManager::AppendLog(AMPID app, const std::string& log)
{
    profiles.Operate([app, &log](ProfileMapType& profileMap){
        if(profileMap.find(app)==profileMap.end())
            return;

        profileMap[app].AppendLog(formatText("time stamp: %s s app: %#x [%s]", TimeStamp::GetString().c_str(), app, log.c_str()));
    });
}
