/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveUIProcess.cpp
/// @brief contains the implementation of class ActiveUIProcess
///
/// Created by zs on 2016/10/13.
/// this file contains the implementation of class ActiveUIProcess
///

#include <signal.h>
#include <fstream>

#include "TaskDispatcher.h"
#include "ProcessInfo.h"
#include "Capi.h"
#include "UIProcess.h"
#include "NotifyWaiter.h"
#include "HMICTask.h"
#include "AppUIHMI.h"
#include "AppState.h"
#include "cs_task.h"
#include "Configuration.h"
#include "AppList.h"
#include "BPMTask.h"
#include "COMMON/UniqueID.h"

#ifdef ENABLE_DEBUG_MODE
#include "AppProfileManager.h"
#endif // ENABLE_DEBUG_MODE

Broadcaster<void(AMPID app)> UIProcess::beforePreShowBroadcaster;
Broadcaster<void(AMPID app)> UIProcess::beforePreHideBroadcaster;
unsigned int UIProcess::startingCallbackHandle = 0;
unsigned int UIProcess::showCallbackHandle = 0;
unsigned int UIProcess::hideCallbackHandle = 0;

UIProcess::UIProcess()
    : Process(INVALID_AMPID, std::make_shared<InitialState>())
    , UI(nullptr)
    , createHMISurfaceTimer(0)
    , animationPlayer(0)
    , bootVisibilityCtrlHandle(0)
{
    memset(&attr, 0, sizeof(attr));
    RegisterObserver();
};

UIProcess::UIProcess(AMPID app)
    : Process(app, std::make_shared<InitialState>())
    , UI(nullptr)
    , createHMISurfaceTimer(0)
    , animationPlayer(app)
    , bootVisibilityCtrlHandle(0)
{
    memset(&attr, 0, sizeof(attr));
    RegisterObserver();
};

UIProcess::~UIProcess()
{
    if(createHMISurfaceTimer)
        timer_delete(createHMISurfaceTimer);

    if(bootVisibilityCtrlHandle)
    {
        logInfo(PLC, LOG_HEAD, "unregister handle = ", bootVisibilityCtrlHandle);
        TaskDispatcher::GetInstance()->dispatchedTaskBroadcaster.Unregister(bootVisibilityCtrlHandle);
    }
}

void Respond2ResumeNotify(unsigned int ampid, unsigned long long id)
{
    Notify(id);
    logInfo(CAPI, "respond to resume notify. id: ", ToHEX(id));
}

void UIProcess::ResumeNotify()
{
    DomainVerboseLog chatter(PLC, "UIProcess::ResumeNotify()");

    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);
    ::ResumeNotify(GetAMPID(), id);
#ifndef CODE_FOR_AIO
#ifndef CODE_FOR_HS7
    if(!Wait(id))
        logWarn(PLC, "UIProcess::ResumeNotify(): wait timeout!");
#endif // CODE_FOR_HS7
#endif // CODE_FOR_AIO
    UnregisterWaiterID(id);
}

void Respond2PreShowNotify(unsigned int ampid, unsigned long long id)
{
    Notify(id);
    logInfo(CAPI, "respond to pre-show notify. id: ", ToHEX(id));
}

void UIProcess::PreShowNotify()
{
    DomainVerboseLog chatter(PLC, "UIProcess::PreShowNotify()");

    beforePreShowBroadcaster.NotifyAll([this](std::function<void(AMPID)> function){
        DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", this->GetAMPID()));

        function(this->GetAMPID());
    });

    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);
    ::PreShowNotify(GetAMPID(), -1, id);
#ifndef CODE_FOR_AIO
#ifndef CODE_FOR_HS7
    if(!Wait(id)) // note: this timeout includes both pre-show and intent.
        logWarn(PLC, "UIProcess::PreShowNotify(): wait timeout!");
#endif // CODE_FOR_HS7
#endif // CODE_FOR_AIO
    UnregisterWaiterID(id);
}

void UIProcess::ShowNotify()
{
    ::ShowNotify(GetAMPID());
}

void Respond2PreHideNotify(unsigned int ampid, unsigned long long id)
{
    Notify(id);
    logInfo(CAPI, "respond to pre-hide notify. id: ", ToHEX(id));
}

void UIProcess::PreHideNotify()
{
    logVerbose(PLC, "UIProcess::PreHideNotify()--->IN");

    beforePreHideBroadcaster.NotifyAll([this](std::function<void(AMPID)> function){
        DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%#x)", this->GetAMPID()));

        function(this->GetAMPID());
    });

//    unsigned long long id = GetUniqueID();
//    RegisterWaiterID(id);
    ::PreHideNotify(GetAMPID(), 0);
//    if(!Wait(id))
//        logWarn(PLC, "UIProcess::PreHideNotify(): wait timeout!");
//    UnregisterWaiterID(id);

    // Send CS Task
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_IME_HIDE);
    csTask->setAmpid(GetAMPID());
    TaskDispatcher::GetInstance()->Dispatch(csTask);
    logVerbose(PLC, "UIProcess::PreHideNotify()--->OUT");
}

void UIProcess::HideNotify()
{
    logVerbose(PLC, "UIProcess::HideNotify()--->IN");

    ::HideNotify(GetAMPID());

    logVerbose(PLC, "UIProcess::HideNotify()--->OUT");
}

void UIProcess::PlayStartupAnimationNotify()
{
    logVerbose(PLC, "UIProcess::PlayStartupAnimationNotify()--->IN");

    if(animationPlayer.IsReady() && IsShowStartupScreen())
        animationPlayer.Show();

    logVerbose(PLC, "UIProcess::PlayStartupAnimationNotify()--->OUT");
}

void UIProcess::StopStartupAnimationNotify()
{
    logVerbose(PLC, "UIProcess::StopStartupAnimationNotify()--->IN");

    if(animationPlayer.IsReady())
        animationPlayer.Hide();

    logVerbose(PLC, "UIProcess::StopStartupAnimationNotify()--->OUT");
}

void UIProcess::StartIME()
{
    DomainVerboseLog chatter(PLC, "UIProcess::StartIME()");

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_IME_SHOW);
    hmicTask->SetAMPID(GetAMPID());
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void UIProcess::StopIME()
{
    DomainVerboseLog chatter(PLC, "UIProcess::StopIME()");

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_IME_HIDE);
    hmicTask->SetAMPID(GetAMPID());
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void UIProcess::SetAMPID(AMPID proc)
{
    Process::SetAMPID(proc);
    animationPlayer.SetAMPID(proc);
}

AppStatus UIProcess::GetStatus() const
{
    AppStatus status = Process::GetStatus();
    auto ui = dynamic_pointer_cast<const AppUIHMI>(GetUIConst());
    if(ui)
        SET_VISIBLE(status, GetState()->IsVisibleState());

    return status;
}

shared_ptr<const IAppState> UIProcess::GetState()const
{
    return dynamic_pointer_cast<const IAppState>(Process::GetState());
}

shared_ptr<AppUI> UIProcess::GetUI()
{
    return UI;
}

void UIProcess::SetUI(const shared_ptr<AppUI> appUI)
{
    UI = appUI;
}

shared_ptr<const AppUI> UIProcess::GetUIConst()const
{
    return const_pointer_cast<AppUI>(UI);
}

timer_t UIProcess::GetCreateHMISurfaceTimer() const
{
    return createHMISurfaceTimer;
}

void UIProcess::SetCreateHMISurfaceTimer(timer_t createHMISurfaceTimer)
{
    UIProcess::createHMISurfaceTimer = createHMISurfaceTimer;
}

bool UIProcess::IsShowStartupScreen()const
{
    return AppList::GetInstance()->IsPlayStartupAnimation(GetAMPID());
}

void UIProcess::SetBootVisibilityCtrlHandle(unsigned long long handle)
{
    bootVisibilityCtrlHandle = handle;
    logInfo(PLC, LOG_HEAD, "hanle  = ", bootVisibilityCtrlHandle);
}

unsigned long long UIProcess::GetBootVisibilityCtrlHandle()const
{
    return bootVisibilityCtrlHandle;
}

void UIProcess::RegisterObserver()
{
    if(!appStateChangeBroadcaster.IsRegistered(startingCallbackHandle))
        startingCallbackHandle = appStateChangeBroadcaster.Register(&UIProcess::OnStarting);
    if(!appStateChangeBroadcaster.IsRegistered(showCallbackHandle))
        showCallbackHandle = appStateChangeBroadcaster.Register(&UIProcess::OnShow);
    if(!appStateChangeBroadcaster.IsRegistered(hideCallbackHandle))
        hideCallbackHandle = appStateChangeBroadcaster.Register(&UIProcess::OnHide);
}

void UIProcess::OnStarting(AMPID proc, E_PROC_STATE state)
{
    if(state!=E_PROC_STATE_STARTING)
        return;

    DomainVerboseLog chatter(PLC, formatText("UIProcess::OnStarting(%#x, %d)", proc, state));

    // find UI process object
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto UIProcObj = procInfo->FindUIProc(proc);
    procInfo->Unlock();
    if(!UIProcObj)
        return;

    static const char *const path = "/media/ivi-data/ama.d/noAppBootTimeout";
    std::ifstream noAppBootTimeout(path);
    bool isAppBootTimeout = !noAppBootTimeout.is_open();
    noAppBootTimeout.close();

    if(isAppBootTimeout)
    {
        // create boot timer in case the app is timeout
        pthread_attr_init(&UIProcObj->attr);
        pthread_attr_setdetachstate(&UIProcObj->attr, PTHREAD_CREATE_DETACHED);
        struct sigevent event;
        event.sigev_notify            = SIGEV_THREAD;
        event.sigev_value.sival_int   = proc;
        event.sigev_notify_function   = &UIProcess::OnBootTimeout;
        event.sigev_notify_attributes = &UIProcObj->attr;
        timer_t timerID;
        timer_create(CLOCK_MONOTONIC, &event, &timerID);

        // start timer
        UIProcObj->SetCreateHMISurfaceTimer(timerID);
        itimerspec itsp;
        itsp.it_interval.tv_sec  = 0;
        itsp.it_interval.tv_nsec = 0;
        itsp.it_value.tv_sec     = GetAppBootTimeoutInSec(); // app boot timeout
        itsp.it_value.tv_nsec    = 0;
        timer_settime(timerID, 0, &itsp, nullptr);
    }
    else
        logInfo(PLC, "UIProcess::OnStarting(): no app boot timeout due to config file ", std::string(path));

}

void UIProcess::OnShow(AMPID proc, E_PROC_STATE state)
{
    if(state!=E_APP_STATE_SHOW)
        return;

    // find UI process object
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto UIProcObj = procInfo->FindUIProc(proc);
    procInfo->Unlock();
    if(!UIProcObj)
        return;

    // delete boot timer
    auto timer = UIProcObj->GetCreateHMISurfaceTimer();
    if(timer!=0)
    {
        timer_delete(UIProcObj->GetCreateHMISurfaceTimer());
        UIProcObj->SetCreateHMISurfaceTimer(0);
    }

}

void UIProcess::OnHide(AMPID proc, E_PROC_STATE state)
{
    if(state!=E_APP_STATE_HIDE)
        return;

    // find UI process object
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto UIProcObj = procInfo->FindUIProc(proc);
    procInfo->Unlock();
    if(!UIProcObj)
        return;

    // delete boot timer
    auto timer = UIProcObj->GetCreateHMISurfaceTimer();
    if(timer!=0)
    {
        timer_delete(UIProcObj->GetCreateHMISurfaceTimer());
        UIProcObj->SetCreateHMISurfaceTimer(0);
    }

}

void UIProcess::OnBootTimeout(sigval_t sig_value)
{
    DomainVerboseLog chatter(PLC, formatText("UIProcess::OnBootTimeout(%#x)", sig_value.sival_int));

    AMPID proc = sig_value.sival_int;

    auto plcTask = make_shared<PLCTask>();
    plcTask->SetType(PLC_NOTIFY_APP_BOOT_TIMEOUT);
    plcTask->SetAMPID(proc);

    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}
