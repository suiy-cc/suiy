/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ProcessInfo.h
/// @brief contains class ProcessInfo
///
/// Created by zs on 2016/6/13.
/// this file contains the definition of class ProcessInfo
///

#ifndef ACTIVEAPPINFO_H
#define ACTIVEAPPINFO_H

#include <mutex>

#include "ActiveApp.h"

/// @class ProcessInfo
/// @brief this class stores information of all the active APPs
///
/// Do lock the object before you perform any operation.
/// usage:
/// ProcessInfo* info = GetInstance();
/// info->Lock();
/// ...
/// shared_ptr<ProcessInfo> appInfo = info->FindApp(seatId, surfaceId, found);
/// ...
/// info->Unlock();
class ProcessInfo
{
public:
    ~ProcessInfo(){};

    static ProcessInfo* GetInstance();

    /// @{
    void Lock();
    void Unlock();
    /// @}

    /// @name operations
    /// @{
    void RemoveProc(AMPID app);
    std::vector<shared_ptr<Process>> FindProc(std::function<bool(shared_ptr<Process>)> function);
    shared_ptr<Process> FindProc(AMPID ampid);
    shared_ptr<Process> FindProc(std::string unitName);
    shared_ptr<UIProcess> FindUIProc(AMPID ampid);
    shared_ptr<UIProcess> FindUIProc(std::string unitName);
    shared_ptr<ActiveApp> FindApp(AMPID ampid);
    shared_ptr<ActiveApp> FindApp(std::string unitName);
    void AddProc(int seatId, shared_ptr<Process> app);
    AMPID GetAnonymousAMPID();
    /// @}
private:
    ProcessInfo(){}; // for singleton pattern

    // data members
    std::unordered_map<AMPID, shared_ptr<Process>> processes;

    std::mutex mutexForAllDataMembers;

    static AMPID anonymousAMPIDCount;
};


#endif // ACTIVEAPPINFO_H
