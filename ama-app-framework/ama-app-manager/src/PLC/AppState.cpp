/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "PLCTask.h"
#include "AppState.h"
#include "ProcessInfo.h"
#include "HMICTask.h"
#include "TaskDispatcher.h"
#include "UnitManager.h"
#include "Process.h"
#include "AMGRLogContext.h"

#include <fstream>

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

void IAppState::ShowReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
        PRINT_WRONG_STATE_LOG;
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.
}
void IAppState::HideReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
        PRINT_WRONG_STATE_LOG;
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.
}
void IAppState::RemoveAppFromZOrderReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    if(IsStableState())
        PRINT_WRONG_STATE_LOG;
    else
        _DelayCurrentRequest(proc, task);// delay current task if current state is a unstable state, by default.
}

// -----------------------------------------------------
// InitialState implementation
// -----------------------------------------------------
bool InitialState::StartReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", proc->GetAMPID()));

    // validate argument
    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(!plcTask)
    {
        logError(PLC, LOG_HEAD, "null task pointer!");
        return false;
    }
    auto UIProc = dynamic_pointer_cast<UIProcess>(proc);
    if(!UIProc)
    {
        logError(PLC, LOG_HEAD, "null process pointer!");
        return false;
    }

    // tell if current start command's visibility
    bool isQuietStart = false;
    if(plcTask->GetType()==PLC_CMD_START)
    {
        isQuietStart = plcTask->GetUserData();
    }
    else
    {
        logError(PLC, LOG_HEAD, "invalid task type! type: ", plcTask->GetType());
    }

    // add invisible property to "HMIC_NOTIFY_APP_START" task
    if(isQuietStart)
    {
        AMPID app = proc->GetAMPID();

        auto weakUIProc = std::weak_ptr<UIProcess>(UIProc);
        std::function<bool(std::shared_ptr<Task>)> invisibleStartCtrl;
        invisibleStartCtrl = [app, isQuietStart, weakUIProc](std::shared_ptr<Task> task)->bool
        {
            // task check
            auto hmicTask = dynamic_pointer_cast<HMICTask>(task);
            if(!hmicTask)
                return false;
            if(hmicTask->GetAMPID() != app)
                return false;
            if(hmicTask->GetType() != HMIC_NOTIFY_APP_START)
                return false;

            // modify HMIC task
            hmicTask->SetUserData(isQuietStart);

            // recycle process's handle
            auto sharedUIProc = weakUIProc.lock();
            if(sharedUIProc)
                sharedUIProc->SetBootVisibilityCtrlHandle(0);

            return true;
        };
        unsigned long long handle = TaskDispatcher::GetInstance()->dispatchedTaskBroadcaster.Register(invisibleStartCtrl);

        UIProc->SetBootVisibilityCtrlHandle(handle);
    }
    // play start up animation
    else
        UIProc->PlayStartupAnimationNotify();

    // boot APP
    bool isOK = proc->Start();

    // change state
    proc->SetState(std::make_shared<StartingState>());

    return isOK;
}

void InitialState::IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "InitialState::IntentReq()");

    _DelayCurrentRequest(proc, task);
}

void ShowState::StopReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "ShowState::StopReq()");

    if(proc)
    {
        auto hmicTask = make_shared<HMICTask>();
        hmicTask->SetType(HMIC_NOTIFY_APP_STOP);
        hmicTask->SetAMPID(proc->GetAMPID());
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    }
}

void ShowState::HideReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "ShowState::HideReq()");

    if(proc)
    {
        // change state to VisibilityChangingState
        proc->SetState(std::make_shared<HidingState>() );
        logInfo(PLC, LOG_HEAD, "AMPID: ", ToHEX(proc->GetAMPID())
                , " old state: ", this->GetStateString()
                , " new state: ", proc->GetState()->GetStateString());

        auto hmicTask = make_shared<HMICTask>();
        hmicTask->SetType(HMIC_NOTIFY_APP_HIDE);
        hmicTask->SetAMPID(proc->GetAMPID());
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    }
}

void ShowState::IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "ShowState::IntentReq()");

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

void ShowState::RemoveAppFromZOrderReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    FUNCTION_SCOPE_LOG_C(PLC);

    logInfo(PLC, LOG_HEAD, "visible state! hide current app.");
    HideReq(proc, task);
}

bool HideState::StartReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    FUNCTION_SCOPE_LOG_C(PLC);

    auto UIProc = dynamic_pointer_cast<UIProcess>(proc);
    if(UIProc)
        ShowReq(UIProc, task);
    else
        return false;

    return true;
}

static void SendShowTask(AMPID app, std::function<void(AMPID)> beforePreShowCallback = nullptr)
{
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_APP_SHOW);
    hmicTask->SetAMPID(app);
    if(beforePreShowCallback)
    {
        // create callback handle
        typedef unsigned int HandleType;
        HandleType* callbackHandle = new HandleType;
        *callbackHandle = 0;
        hmicTask->SetTaskDoneCBData(reinterpret_cast<unsigned long long>(callbackHandle)
                                   , [](unsigned long long data){delete reinterpret_cast<HandleType*>(data);});

        // register before pre-show notify right before
        // show-task is going to be done.
        hmicTask->SetPreTaskDoneCB([app, beforePreShowCallback, callbackHandle](std::shared_ptr<Task>){
            auto procInfo = ProcessInfo::GetInstance();
            procInfo->Lock();
            auto UIProc = procInfo->FindUIProc(app);
            if(UIProc)
                *callbackHandle = UIProc->beforePreShowBroadcaster.Register(beforePreShowCallback);
            procInfo->Unlock();
        });

        // unregister it right after show-task is done
        hmicTask->SetTaskDoneCB([app, callbackHandle](std::shared_ptr<Task>){
            auto procInfo = ProcessInfo::GetInstance();
            procInfo->Lock();
            auto UIProc = procInfo->FindUIProc(app);
            if(UIProc)
                UIProc->beforePreShowBroadcaster.Unregister(*callbackHandle);
            procInfo->Unlock();
        });
    }
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

}

void HideState::ShowReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "HideState::ShowReq()");

    if(proc)
    {
        // change state to VisibilityChangingState
        proc->SetState(std::make_shared<ShowingState>() );
        logInfo(PLC, LOG_HEAD, "AMPID: ", ToHEX(proc->GetAMPID())
                , " old state: ", this->GetStateString()
                , " new state: ", proc->GetState()->GetStateString());

        SendShowTask(proc->GetAMPID());
    }
}

void HideState::StopReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "HideState::StopReq()");

    if(proc)
    {
        auto hmicTask = make_shared<HMICTask>();
        hmicTask->SetType(HMIC_NOTIFY_APP_STOP);
        hmicTask->SetAMPID(proc->GetAMPID());
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    }
}

void HideState::IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "HideState::IntentReq()");

    shared_ptr<PLCTask> plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(plcTask)
    {
        IntentInfo intentInfoObj;
        IntentInfo* intentInfo = reinterpret_cast<IntentInfo*>(plcTask->GetUserData());
        if(intentInfo)
        {
            intentInfoObj = (*intentInfo);
            delete intentInfo;
        }

        // TODO: no intent sequence means new one can reach app before old one
        if(intentInfoObj.isQuiet && !intentInfoObj.action.empty())
            proc->IntentNotify(intentInfoObj);
        else if(!intentInfoObj.isQuiet)
        {
            // change state to VisibilityChangingState
            proc->SetState(std::make_shared<ShowingState>() );
            logInfo(PLC, LOG_HEAD, "AMPID: ", ToHEX(proc->GetAMPID())
                    , " old state: ", this->GetStateString()
                    , " new state: ", proc->GetState()->GetStateString());

            SendShowTask(proc->GetAMPID(), [intentInfoObj](AMPID app){
                // send intent notify right before pre-show notify
                auto procInfo = ProcessInfo::GetInstance();
                procInfo->Lock();
                auto UIProc = procInfo->FindUIProc(app);
                UIProc->IntentNotify(intentInfoObj);
                procInfo->Unlock();
            });
        }
    }
}

void HideState::RemoveAppFromZOrderReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const
{
    FUNCTION_SCOPE_LOG_C(PLC);

    logInfo(PLC, LOG_HEAD, "remove current app from zorder.");
    auto plcTask = dynamic_pointer_cast<PLCTask>(task);
    if(plcTask)
    {
        auto hmicTask = std::make_shared<HMICTask>();
        hmicTask->SetType(HMIC_CMD_REMOVE_APP_FROM_ZORDER);
        hmicTask->SetAMPID(plcTask->GetAMPID());
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    }
    else
        logError(PLC, LOG_HEAD, "invalid task type!");
}

void StoppingState::KillReq(shared_ptr<Process> proc, shared_ptr<Task> task)const
{
    DomainVerboseLog chatter(PLC, "StoppingState::KillReq()");

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
        proc->SetState(std::make_shared<KilledState>());
    }
}
