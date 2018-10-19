/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLCTask.cpp
/// @brief contains
///
/// Created by zs on 2017/03/09.
///

#include "PLCTask.h"
#include "TaskDispatcher.h"
#include "ama_audioTypes.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

std::shared_ptr<PLCTask> MakeIntentTask(const AMPID app, const IntentInfo &intentInfo)
{
    // prepare intent information.
    IntentInfo* tmpIntentInfo = nullptr;
    if(!intentInfo.action.empty() || !intentInfo.data.empty() || intentInfo.isQuiet==true)
    {
        tmpIntentInfo = new IntentInfo;
        tmpIntentInfo->action  = intentInfo.action;
        tmpIntentInfo->data    = intentInfo.data;
        tmpIntentInfo->isQuiet = intentInfo.isQuiet;
    }

    // prepare intent task
    auto pTask = std::make_shared<PLCTask>();
    pTask->SetType(PLC_CMD_INTENT);
    pTask->SetAMPID(app);
    pTask->SetUserData(reinterpret_cast<unsigned long long>(tmpIntentInfo));

    return std::move(pTask);
}

void DispatchIntentTask(const AMPID app, const IntentInfo &intentInfo)
{
    // prepare intent task.
    std::shared_ptr<PLCTask> pTask = MakeIntentTask(app, intentInfo);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

void DispatchStartServiceTask(const std::string& unitName)
{
    std::string tmp = unitName;
    DispatchStartServiceTask(std::move(tmp));
}

void DispatchStartServiceTask(std::string&& unitName)
{
    auto pUnitName = new std::string;
    pUnitName->swap(unitName);
    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetType(PLC_CMD_START_SERVICE);
    plcTask->SetUserData(reinterpret_cast<unsigned long long>(pUnitName), [](unsigned long long data){
        delete reinterpret_cast<std::string*>(data);
    });

    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}