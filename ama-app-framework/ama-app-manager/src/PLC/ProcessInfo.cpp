/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveProcessInfo.cpp
/// @brief contains the implementation of class ActiveProcessInfo
///
/// Created by zs on 2016/6/13.
/// this file contains the implementation of class ActiveProcessInfo
///

#include <iostream>
#include <algorithm>

#include "AppState.h"
#include "ProcessInfo.h"

AMPID ProcessInfo::anonymousAMPIDCount = 0;

ProcessInfo* ProcessInfo::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static ProcessInfo onlyInstance;
    return &onlyInstance;
}

void ProcessInfo::Lock()
{
    mutexForAllDataMembers.lock();
}

void ProcessInfo::Unlock()
{
    mutexForAllDataMembers.unlock();
}

std::vector<shared_ptr<Process>> ProcessInfo::FindProc(std::function<bool(shared_ptr<Process>)> function)
{
    std::vector<shared_ptr<Process>> rtv;
    for(auto& proc: processes)
    {
        if(function(proc.second))
            rtv.push_back(proc.second);
    }

    return std::move(rtv);
}

shared_ptr<Process> ProcessInfo::FindProc(AMPID ampid)
{
    if(processes.find(ampid)!=processes.end())
        return processes[ampid];

    return nullptr;
}

shared_ptr<Process> ProcessInfo::FindProc(std::string unitName)
{
    for(auto& proc: processes)
        if(proc.second->GetBootCMD()==unitName)
            return proc.second;

    return nullptr;
}

shared_ptr<UIProcess> ProcessInfo::FindUIProc(AMPID ampid)
{
    auto proc = FindProc(ampid);
    return dynamic_pointer_cast<UIProcess>(proc);
}

shared_ptr<UIProcess> ProcessInfo::FindUIProc(std::string unitName)
{
    auto proc = FindProc(unitName);
    return dynamic_pointer_cast<UIProcess>(proc);
}

shared_ptr<ActiveApp> ProcessInfo::FindApp(AMPID ampid)
{
    auto proc = FindProc(ampid);
    return dynamic_pointer_cast<ActiveApp>(proc);
}

shared_ptr<ActiveApp> ProcessInfo::FindApp(std::string unitName)
{
    auto proc = FindProc(unitName);
    return dynamic_pointer_cast<ActiveApp>(proc);
}

/// @brief add a new app
/// @param seatId the seat ID of specified app
/// @param app the information of specified app
void ProcessInfo::AddProc(int seatId, shared_ptr<Process> app)
{
    logVerbose(PLC, "ProcessInfo::AddProc(", ToHEX(app->GetAMPID()), ")--->IN");

    processes[app->GetAMPID()] = app;

    logVerbose(PLC, "ProcessInfo::AddProc(", ToHEX(app->GetAMPID()), ")--->OUT");
}

AMPID ProcessInfo::GetAnonymousAMPID()
{
    DomainVerboseLog chatter(PLC, "ProcessInfo::GetAnonymousAMPID()");

    if(anonymousAMPIDCount>0xff)
        return INVALID_AMPID;

    return anonymousAMPIDCount++ + 0xff00;
}

/// @brief remove specified app
/// @param ampid the AMPID of specified process
void ProcessInfo::RemoveProc(AMPID ampid)
{
    logVerbose(PLC, "ProcessInfo::RemoveProc(", ToHEX(ampid), ")--->IN");

    processes.erase(ampid);

    logVerbose(PLC, "ProcessInfo::RemoveProc(", ToHEX(ampid), ")--->OUT");
}
