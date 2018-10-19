/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ProcState.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/07/13.
///
/// this file contains the implementation of class ProcState
///

#include "UnitManager.h"
#include "HMICTask.h"
#include "ProcessInfo.h"
#include "AppState.h"
#include "PLCTask.h"
#include "ProcState.h"

void IProcState::_DelayCurrentRequest(shared_ptr<Process> proc, shared_ptr<Task> task)
{
    FUNCTION_SCOPE_LOG_C(PLC);

    shared_ptr<PLCTask> plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(plcTask)
        proc->AddDeferredTask(plcTask);
}

bool IProcState::StartReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
        PRINT_WRONG_STATE_LOG;
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.

    return true;
}

void IProcState::IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
    {
        PRINT_WRONG_STATE_LOG;
        shared_ptr<PLCTask> plcTask = dynamic_pointer_cast<PLCTask>(task);
        if(plcTask)
        {
            IntentInfo* intentInfo = reinterpret_cast<IntentInfo*>(plcTask->GetUserData());
            if(intentInfo)
                delete intentInfo;
        }
    }
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.
}

void IProcState::OnEnterStableState(shared_ptr<Process> proc)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", proc->GetAMPID()));

    logDebug(PLC, LOG_HEAD, "deferredTaskCount:", proc->getDeferredTaskCount());
    auto taskList = proc->PopAllDeferredTasks();
    for(auto& plcTask: taskList)
    {
        TaskDispatcher::GetInstance()->Dispatch(plcTask);
        logInfo(PLC, LOG_HEAD, "deferred task ", plcTask->GetTaskTypeString(), " is sent.");
    }
}

void IProcState::KillReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
        PRINT_WRONG_STATE_LOG;
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.
}

void IProcState::StopReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
        PRINT_WRONG_STATE_LOG;
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.
}

bool ProcDeadState::StartReq(shared_ptr<Process> proc, shared_ptr<Task> task) const
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", proc->GetAMPID()));

    // validate argument
    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(!plcTask)
    {
        logError(PLC, LOG_HEAD, "null task pointer!");
        return false;
    }
    if(!proc)
    {
        logError(PLC, LOG_HEAD, "null process pointer!");
        return false;
    }

    // validate task type
    if(plcTask->GetType()!=PLC_CMD_START)
    {
        logError(PLC, LOG_HEAD, "invalid task type! type: ", plcTask->GetType());
        return false;
    }

    // boot APP
    bool isOK = proc->Start();

    proc->SetState(std::make_shared<ProcStartingState>());

    return isOK;
}

void ProcActiveState::IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task) const
{
    FUNCTION_SCOPE_LOG_C(PLC);

    shared_ptr<PLCTask> plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(plcTask)
    {
        IntentInfo* intentInfo = reinterpret_cast<IntentInfo*>(plcTask->GetUserData());
        if(intentInfo)
        {
            proc->IntentNotify(*intentInfo);
            delete intentInfo;
        }
    }
}

void ProcActiveState::StopReq(shared_ptr<Process> proc, shared_ptr<Task> task) const
{
    FUNCTION_SCOPE_LOG_C(PLC);

    proc->SetState(std::make_shared<ProcStoppingState>());

    proc->StopNotify();
}

void ProcStoppingState::KillReq(shared_ptr<Process> proc, shared_ptr<Task> task) const
{
    FUNCTION_SCOPE_LOG_C(PLC);

    if(proc)
    {
        // kill the process
        if(UnitManager::GetInstance()->GetUnitState(proc->GetAMPID()))
            UnitManager::GetInstance()->StopUnit(proc->GetAMPID());

        // delete process record
        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        procInfo->RemoveProc(proc->GetAMPID());
        procInfo->Unlock();

        // change state to killed state
        proc->SetState(std::make_shared<ProcDeadState>());
    }
}
