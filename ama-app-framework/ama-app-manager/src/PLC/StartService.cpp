/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file StartService.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/05/10.
///

#include "StartService.h"
#include "log.h"
#include "AMGRLogContext.h"
#include "PLCTask.h"
#include "TaskDispatcher.h"
#include "CMDOption.h"
#include "UnitManager.h"
#include "HMICTask.h"
#include "StringOperation.h"
#include "Configuration.h"
#include "ItemID.h"

#include <fstream>

void StartService(AMPID service)
{
    DomainVerboseLog chatter(PLC, formatText("StartService(%#x)", service));

    auto pTask = std::make_shared<PLCTask>();
    pTask->SetType(PLC_CMD_START);
    pTask->SetAMPID(service);
    pTask->SetUserData(1);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

void StartService()
{
    DomainVerboseLog chatter(PLC, formatText("StartService()"));

    CMDOption serviceAMPID("AMPID");
    static const std::string path("/etc/ama.d/BootSequence.txt");
    std::ifstream bootSequenceFile(path.c_str());

    if(bootSequenceFile.is_open())
    {
        while(bootSequenceFile.peek()!=EOF)
        {
            std::string serviceDescriptor;
            bootSequenceFile >> serviceDescriptor;
            logInfo(PLC, "StartService(): start service \"", serviceDescriptor, "\"");
            if(serviceAMPID.FromString(serviceDescriptor))
            {
                AMPID service = static_cast<unsigned int>(std::stoul(serviceAMPID.GetValue(), 0, 0));
                StartService(service);
            }
            else
            {
                UnitManager::GetInstance()->StartUnit(serviceDescriptor);
            }
        }

        bootSequenceFile.close();
    }
    else
        logWarn(PLC, "StartService(): can't open \"", path, "\".");
}

void StartAutoStartProcesses()
{
    DomainVerboseLog chatter(PLC, "StartAutoStartProcesses()");

    std::vector<std::string> bootSequence;

    // get screen count
    auto seats = GetAvailableScreens(true);
    seats.push_back(E_SEAT_NONE);

    // read boot sequence from configure file
    CMDOption serviceAMPID("AMPID");
    static const std::string path("/etc/ama.d/BootSequence.txt");
    std::ifstream bootSequenceFile(path.c_str());
    if(bootSequenceFile.is_open())
    {
        while(bootSequenceFile.peek()!=EOF)
        {
            std::string serviceDescriptor;
            bootSequenceFile >> serviceDescriptor;
            if(IsComment(serviceDescriptor))
                continue;
            else if(IsValidItemID(serviceDescriptor))
            {
                AMPID ampid = AppList::GetInstance()->GetAMPID(serviceDescriptor);

                // validate AMPID
                if(ampid==INVALID_AMPID)
                {
                    logWarn(PLC, "StartAutoStartProcesses(): service \"", serviceDescriptor, "\"'s AMPID is not found. ignore.");
                    continue;
                }

                // check seat
                auto seat = GET_SEAT(ampid);
                if(std::find(seats.begin(), seats.end(), seat)==seats.end())
                {
                    logWarn(PLC, "StartAutoStartProcesses(): service \"", serviceDescriptor, "\"'s seat is not found. ignore.");
                    continue;
                }

                bootSequence.push_back(serviceDescriptor);
            }
            else
            {
                bootSequence.push_back(serviceDescriptor);
            }
        }

        bootSequenceFile.close();
    }

    // add auto-start-app to boot-sequence
    auto autoStartApps = AppList::GetInstance()->GetAutoStartAppList();
    if(IsAutoStartDisabled())
        autoStartApps.clear();
    for(auto& proc: autoStartApps)
    {
        // check seat
        auto seat = GET_SEAT(proc);
        if(std::find(seats.begin(), seats.end(), seat)==seats.end())
            continue;

        // check duplicated process
        auto itemID = AppList::GetInstance()->GetItemID(proc);
        if(std::find(bootSequence.begin(), bootSequence.end(), itemID)!=bootSequence.end())
            continue;

        bootSequence.push_back(itemID);
    }

    // start auto-boot
    for(auto &serviceDescriptor: bootSequence)
    {
        logInfo(PLC, "StartService(): start service \"", serviceDescriptor, "\"");
        if(IsValidItemID(serviceDescriptor))
        {
            AMPID proc = AppList::GetInstance()->GetAMPID(serviceDescriptor);
            StartService(proc);
        }
        else
        {
            DispatchStartServiceTask(std::move(serviceDescriptor));
        }
    }
}

void SendAutoStartTask()
{
    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetType(PLC_CMD_START_AUTO_BOOT_PROCESSES);

    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}