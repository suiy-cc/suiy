/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file CAPINotifyEnum.h
/// @brief contains common API notify's enumeration and its declearation
///
/// Created by zs on 2017/10/17.
///

#ifndef CAPINOTIFYENUM_H
#define CAPINOTIFYENUM_H

#include <stdint.h>
#include <string>

enum E_CAPI_NOTIFY{
    E_CAPI_NOTIFY_EARLYINITIALIZE,
    E_CAPI_NOTIFY_INITIALIZE,
    E_CAPI_NOTIFY_INTENT,
    E_CAPI_NOTIFY_RESUME,
    E_CAPI_NOTIFY_PRESHOW,
    E_CAPI_NOTIFY_SHOW,
    E_CAPI_NOTIFY_PREHIDE,
    E_CAPI_NOTIFY_HIDE,
    E_CAPI_NOTIFY_STOP,
    E_CAPI_NOTIFY_SHUTDOWN,
    E_CAPI_NOTIFY_INSTALLEDPACKAGESCHANGED,
    E_CAPI_NOTIFY_PLAYSTARTUPANIMATION,
    E_CAPI_NOTIFY_STOPSTARTUPANIMATION,
    E_CAPI_NOTIFY_ANIMATIONOVER,
    E_CAPI_NOTIFY_POWERSTATECHANGEEX,
    E_CAPI_NOTIFY_SM_SETREQUEST,
    E_CAPI_NOTIFY_SM_SETDONE,
    E_CAPI_NOTIFY_AMGR_SET,
    E_CAPI_NOTIFY_POPUP,
    E_CAPI_NOTIFY_IME_FOCUSCHANGED,
    E_CAPI_NOTIFY_AUDIO_FOCUSGAIN,
    E_CAPI_NOTIFY_AUDIO_FOCUSLOSS,
    E_CAPI_NOTIFY_AUDIO_FOCUSCHANGED,
    E_CAPI_NOTIFY_TITLEBARVISIBILITYSWITCH,
    E_CAPI_NOTIFY_TITLEBARDROPDOWNSTATESWITCH,
    E_CAPI_NOTIFY_APPSHARESTATECHANGE,
    E_CAPI_NOTIFY_APPSTATECHANGE,
    E_CAPI_NOTIFY_APPS_IME_CLOSED,
    E_CAPI_NOTIFY_NSM_RESTART_OK,

    E_CAPI_NOTIFY_COUNT,
};

// declearation of notifies

// PLC
void EarlyInitializeNotify(uint32_t app);
void InitializeNotify(uint32_t app, uint32_t startArg, uint32_t isVisible, uint64_t id);
void IntentNotify(uint32_t app, const std::string& action, const std::string& data);
void ResumeNotify(uint32_t app, uint64_t id);
void PreShowNotify(uint32_t app, uint32_t startArg, uint64_t id);
void ShowNotify(uint32_t app);
void PreHideNotify(uint32_t app, uint64_t id);
void HideNotify(uint32_t app);
void StopNotify(uint32_t app, const std::string& bootCMD);

// home
void InstalledPKGsChangedNotify(uint32_t home);

// animation-player
void PlayStartupAnimationNotify(uint32_t app);
void StopStartupAnimationNotify(uint32_t app);
void AnimationOverNotify(uint32_t app);

// PM
void BroadcastShutdownNotify(uint32_t type);
void PowerStateChangeNotifyEx(uint32_t seat, uint32_t state);

// SM
void BroadcastSMSetRequestNotify(uint32_t requester, uint32_t replyer, const std::string& key, const std::string& value);
void BroadcastSMSetDoneNotify(uint32_t requester, uint32_t replyer, const std::string& key, const std::string& value);
void BroadcastAMGRSetNotify(uint32_t key, const std::string& value);

// popup
void ShowPopupNotify(uint32_t popupId, uint32_t reason);
void ShowPopupExNotify(uint32_t popupId, uint32_t reason,const std::string& message);


// IME
void IMEFocusChangedNotify(uint32_t app);
void IMEClosedNotify(uint32_t arg);

// audio
void AudioFocusChangedNotify(uint32_t connectID,uint32_t oldFocusSta,uint32_t newFocusSta,uint32_t applyStreamID);

// titlebar
void SwitchTitlebarVisibilityNotify(uint32_t titlebar, uint32_t visibility, uint32_t style = 0);
void SwitchTitlebarDropDownStateNotify(uint32_t titlebar, uint32_t visibility, uint32_t style = 0);

// app share
void AppShareStateChangeNotify(uint32_t app, uint32_t seat, uint32_t state);

// app state change
void AppStatusChangeNotify(uint32_t app, uint32_t state);

// node state manager restart notify
void BroadcastNSMRebootNotify();

#endif // CAPINOTIFYENUM_H
