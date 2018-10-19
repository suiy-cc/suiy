/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file UIProcess.h
/// @brief contains class UIProcess
///
/// Created by zs on 2016/10/13.
/// this file contains the definition of class UIProcess
///

#ifndef UIPROCESS_H
#define UIPROCESS_H

#include <list>
#include <signal.h>

#include "AppUI.h"
#include "Process.h"
#include "Broadcaster.h"
#include "AnimationPlayer.h"
#include "AppState.h"

class PLCTask;

class UIProcess
: virtual public Process
{
public:
    /// @name constructors & destructors
    /// @{
    UIProcess();
    UIProcess(AMPID app);
    virtual ~UIProcess();
    /// @}

    /// @name notifies
    /// @{
    virtual void ResumeNotify();
    virtual void PreShowNotify();
    virtual void ShowNotify();
    virtual void PreHideNotify();
    virtual void HideNotify();
    virtual void PlayStartupAnimationNotify();
    virtual void StopStartupAnimationNotify();
    /// @}

    /// @name operations
    /// @{
    void StartIME();
    void StopIME();
    /// @}

    /// @name getters & setters
    /// @{
    virtual void SetAMPID(AMPID proc)override;
    virtual AppStatus GetStatus() const override;
    shared_ptr<const IAppState> GetState()const;
    shared_ptr<AppUI> GetUI();
    void SetUI(const shared_ptr<AppUI> appUI);
    shared_ptr<const AppUI> GetUIConst()const;
    timer_t GetCreateHMISurfaceTimer() const;
    void SetCreateHMISurfaceTimer(timer_t createHMISurfaceTimer);
    virtual bool IsShowStartupScreen()const;
    void SetBootVisibilityCtrlHandle(unsigned long long handle);
    unsigned long long GetBootVisibilityCtrlHandle()const;
    /// @}

    /// @name broadcasters
    /// @{
    /// before pre-show broadcaster
    static Broadcaster<void(AMPID app)> beforePreShowBroadcaster;
    /// before pre-hide broadcaster
    static Broadcaster<void(AMPID app)> beforePreHideBroadcaster;
    /// @}

protected:
    void RegisterObserver();
    static void OnStarting(AMPID proc, E_PROC_STATE state);
    static unsigned int startingCallbackHandle;
    static void OnShow(AMPID proc, E_PROC_STATE state);
    static unsigned int showCallbackHandle;
    static void OnHide(AMPID proc, E_PROC_STATE state);
    static unsigned int hideCallbackHandle;
    static void OnBootTimeout(sigval_t sig_value);

private:
    shared_ptr<AppUI> UI;
    timer_t createHMISurfaceTimer; // timer
    pthread_attr_t attr; // attribute of timer's runtime
    AnimationPlayer animationPlayer;
    unsigned long long bootVisibilityCtrlHandle;
};


#endif // ACTIVEUIPROCESS_H
