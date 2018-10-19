/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Process.cpp
/// @brief contains the implementation of class Process
///
/// Created by zs on 2016/10/13.
/// this file contains the implementation of class Process
///

#include <COMMON/UniqueID.h>
#include "HMICTask.h"
#include "NotifyWaiter.h"
#include "ProcessInfo.h"
#include "PLCTask.h"
#include "Process.h"
#include "TaskDispatcher.h"
#include "Capi.h"
#include "AppList.h"
#include "UnitManager.h"

#ifdef ENABLE_DEBUG_MODE
#include "AppProfileManager.h"
#endif // ENABLE_DEBUG_MODE

Broadcaster<void(AMPID app, E_PROC_STATE state)> Process::appStateChangeBroadcaster;

Process::Process()
    :ampid(INVALID_AMPID)
    ,pid(0)
    ,isHaveFocus(false)
    ,isAliveApp(false)
    ,state(std::make_shared<ProcDeadState>())
{
}

Process::Process(AMPID app)
    :ampid(app)
    ,pid(0)
    ,isHaveFocus(false)
    ,isAliveApp(false)
    ,state(std::make_shared<ProcDeadState>())
{
}

Process::Process(AMPID app, std::shared_ptr<IProcState> procState)
    :ampid(app)
    ,pid(0)
    ,isHaveFocus(false)
    ,isAliveApp(false)
    ,state(procState)
{
}

bool Process::operator==(const Process &rhs) const
{
    return ampid == rhs.ampid;
}

bool Process::operator!=(const Process &rhs) const
{
    return !(rhs == *this);
}

bool Process::operator<(const Process &rhs) const
{
    return ampid < rhs.ampid;
}

bool Process::operator>(const Process &rhs) const
{
    return rhs < *this;
}

bool Process::operator<=(const Process &rhs) const
{
    return !(rhs < *this);
}

bool Process::operator>=(const Process &rhs) const
{
    return !(*this < rhs);
}

AMPID Process::GetAMPID()const
{
    return ampid;
}

void Process::SetAMPID(AMPID ID)
{
    ampid = ID;
}

std::string Process::GetBootCMD()
{
    return bootCMD;
}

void Process::SetBootCMD(std::string cmd)
{
    bootCMD = cmd;
}

bool Process::IsTheSame(shared_ptr<Process> app) const
{
    if(app)
    {
        return app->ampid == this->ampid;
    }
    else
        return false;
}

E_APPID Process::GetAppID()const
{
    return GET_APPID(ampid);
}

E_SEAT Process::GetSeat()const
{
    return GET_SEAT(ampid);
}

bool Process::IsAutoStart()const
{
    return AppList::GetInstance()->IsAutoStart(GetAMPID());
}

unsigned int Process::GetPID()const
{
    return pid;
}

void Process::SetPID(unsigned int pid)
{
    this->pid = pid;
}

bool Process::IsStarting()const
{
    return state.GetValue()->GetState() == E_PROC_STATE_STARTING;
}

bool Process::IsStopping()const
{
    return state.GetValue()->GetState() == E_PROC_STATE_STOPPING;
}

bool Process::IsOperating()const
{
    return IS_ACTIVE_STATE(state.GetValue()->GetState());
}

AppStatus Process::GetStatus()const
{
    AppStatus status = 0;
    SET_VISIBLE(status, false);
    SET_AVAILABLE(status, IsOperating());

    return status;
}

shared_ptr<const IProcState> Process::GetState()const
{
    return state.GetValue();
}

void Process::SetState(const shared_ptr<IProcState> newState)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", GetAMPID()));

    state.Operate([this, newState](shared_ptr<IProcState>& AppState){
        const type_info &newStateType = typeid(*(newState.get()));

        logInfo(PLC, LOG_HEAD, "old state: ", AppState->GetStateString(), " new state: ", newState->GetStateString());

        if(typeid(*(AppState.get())) != newStateType)
        {
            AppState = newState;
            if(AppState->IsStableState())
                AppState->OnEnterStableState(shared_from_this());

            // broadcast the change of app state
            appStateChangeBroadcaster.NotifyAll([AppState, this](AppShareStateChangeNotifyCB notify){
                DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"Broadcaster(%#x, %s)", GetAMPID(), AppState->GetStateString().c_str()));

                if(AppState)
                    notify(GetAMPID(), AppState->GetState());
                else
                    logWarn(PLC,__STR_FUNCTION__+"Broadcaster(): null state pointer!");
            });
        }
    });
}

bool Process::Start()
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", ampid));

    // boot APP
    string bootCMD = UnitManager::GetInstance()->StartUnit(ampid, NULL);
    if(bootCMD.empty())
        return false;

    // save boot command
    SetBootCMD(bootCMD);

    return true;
}

void Process::Stop()
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", ampid));

    UnitManager::GetInstance()->StopUnit(ampid);
}

std::list<shared_ptr<Task>> Process::PopAllDeferredTasks()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    std::list<shared_ptr<Task>> taskList;
    deferredTasks.Operate([&taskList](std::list<std::shared_ptr<Task>>& DeferredTasks){
        taskList.swap(DeferredTasks);
    });

    return std::move(taskList);
}

void Process::AddDeferredTask(shared_ptr<Task> plcTask)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%s, %#x)", plcTask?plcTask->GetTaskTypeString().c_str():"null", GetAMPID()));

    deferredTasks.Operate([plcTask](std::list<std::shared_ptr<Task>>& DeferredTasks){
        DeferredTasks.push_back(plcTask);
    });
}

int Process::getDeferredTaskCount()
{
    DomainVerboseLog chatter(PLC, formatText("Process::getDeferredTaskCount()"));
    logDebug(PLC,"DeferredTaskCount:",deferredTasks.GetValue().size());
    return deferredTasks.GetValue().size();
}

void Respond2InitializeNotify(unsigned int ampid, unsigned long long id)
{
    Notify(id);
    logInfo(CAPI, "respond to initialize notify. id: ", ToHEX(id));
}

void Process::InitializeNotify(int startArg, bool isVisible)
{
    DomainVerboseLog chatter(PLC, formatText("Process::InitializeNotify(%#x, %d, %s)", GetAMPID(), startArg, isVisible?"true":"false"));
#ifdef ENABLE_DEBUG_MODE
    AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "intialize notify in");
#endif // ENABLE_DEBUG_MODE
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);
    ::InitializeNotify(GetAMPID(), startArg, isVisible, id);
    if(!Wait(id))
        logWarn(PLC, "Process::InitializeNotify(): wait timeout!");
    UnregisterWaiterID(id);
#ifdef ENABLE_DEBUG_MODE
    AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "intialize notify out");
#endif // ENABLE_DEBUG_MODE
}

void Process::IntentNotify(const IntentInfo &intent)
{
    DomainVerboseLog chatter(PLC, "Process::IntentNotify()");

    if(!intent.action.empty())
        ::IntentNotify(GetAMPID(), intent.action, intent.data);
}

void Process::StopNotify()
{
    DomainVerboseLog chatter(PLC, "Process::StopNotify()");

    ::StopNotify(GetAMPID(), GetBootCMD());
}

bool Process::isHaveAudioFocus(void)
{
    return isHaveFocus;
}

bool Process::isAlwaysAliveApp(void)
{
    return isAliveApp;
}

void Process::updateFocusStaByAC(ama_streamID_t stream,ama_focusSta_t focusSta)
{
    mStreamFocusMap[stream] = focusSta;
    std::map<ama_streamID_t,ama_focusSta_t>::iterator it;
    for(it=mStreamFocusMap.begin();it!=mStreamFocusMap.end();++it){
        if(E_FOUCS_LOSE != (it->second)){
            isHaveFocus = true;
            return;
        }
    }

    isHaveFocus = false;
}

void Process::updateAlwaysAliveAppBPM(ama_streamID_t stream,bool status)
{
    isAliveApp = status;
}


void SendCrushNotify(shared_ptr<Process> process)
{
    SendCrushNotify(process->GetAMPID());
}

void SendCrushNotify(AMPID proc)
{
    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetAMPID(proc);
    plcTask->SetType(PLC_NOTIFY_APPCRUSH);
    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}

void ShutdownNotify(unsigned int type)
{
    BroadcastShutdownNotify(type);
}
