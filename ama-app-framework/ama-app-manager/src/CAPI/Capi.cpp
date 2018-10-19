/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <memory>
#include <vector>
#include <COMMON/UniqueID.h>

#include "Capi.h"
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "NotifyWaiter.h"
#include "HMICTask.h"

std::shared_ptr<NeusoftAppMgrStubImpl> AppMgr_Service;

void InitializeCAPI()
{
    DomainVerboseLog chatter(CAPI, "InitializeCAPI()");

    CommonAPI::Runtime::setProperty("LogContext", "ct1");
    CommonAPI::Runtime::setProperty("LibraryBase", "AppMgr");
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    AppMgr_Service = std::make_shared<NeusoftAppMgrStubImpl>();
    while (true)
    {
        logInfo(CAPI, "Initializing CAPI server...");
        std::string domain = "";
        std::string instance = "";
        std::string connection = "";
        bool successfullyRegistered = runtime->registerService(domain, instance, AppMgr_Service, connection);

        if(successfullyRegistered)
            break;

        const int retryTime = 10;
        logWarn(CAPI,"Initialize CAPI server failed, trying again in ", retryTime, " ms.");
        std::this_thread::sleep_for(std::chrono::milliseconds(retryTime));
    }
    logInfo(CAPI,"Successfully Registered CAPI server!");
}

void ResumeNotify(uint32_t _ampid, uint64_t id)
{
    logInfo(CAPI,"ResumeNotify()->AMPID:", ToHEX(_ampid));
    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_RESUME, std::vector<uint64_t>{_ampid, id}, std::vector<std::string>());
}

void IntentNotify(uint32_t _ampid, const std::string& action, const std::string& data)
{
    logInfo(CAPI,"IntentNotify()->AMPID:", ToHEX(_ampid));
    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_INTENT, std::vector<uint64_t>{_ampid}, std::vector<std::string>{action, data});
}

void PreShowNotify(uint32_t _ampid, uint32_t startArg, uint64_t id)
{
    logInfo(CAPI,"PreShowNotify()->AMPID:", ToHEX(_ampid));
    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_PRESHOW, std::vector<uint64_t>{_ampid, startArg, id}, std::vector<std::string>());
}

void ShowNotify(uint32_t _ampid)
{
    logInfo(CAPI,"ShowNotify()->AMPID:", ToHEX(_ampid));
    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_SHOW, std::vector<uint64_t>{_ampid}, std::vector<std::string>());
}

void PreHideNotify(uint32_t _ampid, uint64_t id)
{
    logInfo(CAPI, "PreHideNotify()->AMPID:", ToHEX(_ampid));
    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_PREHIDE, std::vector<uint64_t>{_ampid, id}, std::vector<std::string>());
}

void HideNotify(uint32_t _ampid)
{
    logInfo(CAPI,"HideNotify()->AMPID:", ToHEX(_ampid));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_HIDE, std::vector<uint64_t>{_ampid}, std::vector<std::string>());

}

void EarlyInitializeNotify(uint32_t _ampid)
{
    logInfo(CAPI,"EarlyInitializeNotify()->AMPID:", ToHEX(_ampid));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_EARLYINITIALIZE, std::vector<uint64_t>{_ampid}, std::vector<std::string>());

}

void InitializeNotify(uint32_t _ampid, uint32_t startArg, uint32_t isVisible, uint64_t id)
{
    logInfo(CAPI,"InitializeNotify()->AMPID:", ToHEX(_ampid));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_INITIALIZE, std::vector<uint64_t>{_ampid, startArg, isVisible, id}, std::vector<std::string>());

}

void StopNotify(uint32_t _ampid, const std::string& bootCMD)
{
    logInfo(CAPI,"StopNotify()->AMPID:", ToHEX(_ampid), " boot command:", bootCMD);

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_STOP, std::vector<uint64_t>{_ampid}, std::vector<std::string>{bootCMD});

}

void InstalledPKGsChangedNotify(uint32_t _ampid)
{
    AMPID relatedHome = HOME_AMPID(GET_SEAT(_ampid));
    logInfo(CAPI,"InstalledPKGsChangedNotify()->related home: ", ToHEX(relatedHome));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_INSTALLEDPACKAGESCHANGED, std::vector<uint64_t>{relatedHome}, std::vector<std::string>());

}

void PlayStartupAnimationNotify(uint32_t _ampid)
{
    logInfo(CAPI,"PlayStartupAnimationNotify()->AMPID: ", ToHEX(_ampid));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_PLAYSTARTUPANIMATION, std::vector<uint64_t>{_ampid}, std::vector<std::string>());

}

void StopSuccessReq(unsigned int ampid, unsigned long long id)
{
    logInfo(CAPI, "StopSuccessReq(", ToHEX(ampid), ")--->IN");
    Notify(id);
    logInfo(CAPI, "StopSuccessReq(", ToHEX(ampid), ")--->OUT");
}
void StopStartupAnimationNotify(uint32_t _ampid)
{
    logInfo(CAPI,"StopStartupAnimationNotify()->AMPID: ", ToHEX(_ampid));
    unsigned long long id = GetUniqueID();
    RegisterWaiterID(id);
    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_STOPSTARTUPANIMATION, std::vector<uint64_t>{_ampid, id}, std::vector<std::string>());
    if(!Wait(id))
        logWarn(CAPI, "StopStartupAnimationNotify(): wait timeout!");
    UnregisterWaiterID(id);
}

void AnimationOverNotify(uint32_t _ampid)
{
    logInfo(CAPI,"AnimationOverNotify()->AMPID: ", ToHEX(_ampid));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_ANIMATIONOVER, std::vector<uint64_t>{_ampid}, std::vector<std::string>());

}

void BroadcastShutdownNotify(uint32_t type)
{
    logInfo(CAPI, "BroadcastShutdownNotify()->type: ", type);

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_ANIMATIONOVER, std::vector<uint64_t>{type}, std::vector<std::string>());

}

void PowerStateChangeNotifyEx(uint32_t seat, uint32_t state)
{
    logInfo(CAPI, "BroadcastPowerStateChangeNotifyEx()->state: ", state, " seat: ", seat);

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_POWERSTATECHANGEEX, std::vector<uint64_t>{seat, state}, std::vector<std::string>());

}

void AppShareStateChangeNotify(uint32_t app, uint32_t seat, uint32_t state)
{
    logInfo(CAPI, "AppShareStateChangeNotify()->app: ", ToHEX(app), " seat: ", seat, " state: ", state);

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_APPSHARESTATECHANGE, std::vector<uint64_t>{app, seat, state}, std::vector<std::string>());

}

// app state change
void AppStatusChangeNotify(uint32_t app, uint32_t state)
{
    logInfo(CAPI, "AppStateChangeNotify()->app: ", ToHEX(app), " state: ", ToHEX(state));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_APPSTATECHANGE, std::vector<uint64_t>{app, state}, std::vector<std::string>());
}

// node state manager restart notify
void BroadcastNSMRebootNotify()
{
    FUNCTION_SCOPE_LOG_C(CAPI);

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_NSM_RESTART_OK, std::vector<uint64_t>(), std::vector<std::string>());
}

void AudioFocusChangedNotify(uint32_t connectID,uint32_t oldFocusSta,uint32_t newFocusSta,uint32_t applyStreamID)
{
    logVerbose(CAPI,"audioDeviceFocusChangedNotify-->in connectID=", ToHEX(connectID));
    logVerbose(CAPI,"oldFocusSta:",ToHEX(oldFocusSta)," newFocusSta:",ToHEX(newFocusSta)," applyStreamID:",ToHEX(applyStreamID));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_AUDIO_FOCUSCHANGED, std::vector<uint64_t>{connectID,oldFocusSta,newFocusSta,applyStreamID}, std::vector<std::string>());

    logVerbose(CAPI,"audioDeviceFocusChangedNotify-->out");
}

// SM
void BroadcastSMSetRequestNotify(uint32_t requester, uint32_t replyer, const std::string& key, const std::string& value)
{
    DomainVerboseLog chatter(CAPI, formatText("BroadcastSMSetRequestNotify(%#x, %#x, %s, %s)", requester, replyer, key.c_str(), value.c_str()));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_SM_SETREQUEST, std::vector<uint64_t>{requester, replyer}, std::vector<std::string>{key, value});
}

void BroadcastSMSetDoneNotify(uint32_t requester, uint32_t replyer, const std::string& key, const std::string& value)
{
    DomainVerboseLog chatter(CAPI, formatText("BroadcastSMSetDoneNotify(%#x, %#x, %s, %s)", requester, replyer, key.c_str(), value.c_str()));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_SM_SETDONE, std::vector<uint64_t>{requester, replyer}, std::vector<std::string>{key, value});
}

void BroadcastAMGRSetNotify(uint32_t state_type, const std::string& value)
{
    logVerbose(CAPI, "Capi.cpp::AMGRSetNotify(fireAMGR_SetNotifyEvent)::",state_type," =",value,"--->IN");

    if(AppMgr_Service){
        AppMgr_Service->updateState(state_type,value);
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_AMGR_SET, std::vector<uint64_t>{state_type}, std::vector<std::string>{value});
    }

    logVerbose(CAPI, "Capi.cpp::AMGRSetNotify(fireAMGR_SetNotifyEvent)::",state_type," =",value,"--->OUT");
}

void update_orgDate(const ::v0_1::org::neusoft::AppMgr::key_value& key_value)
{
    logVerbose(CAPI,"Capi.cpp::update_orgDate(fireKey_value_mapEvent)--->IN",key_value.size());

    //::v0_1::org::neusoft::AppMgr::key_value __key_value = (std::unordered_map<std::__cxx11::basic_string<char>, v0_1::org::neusoft::AppMgr::value_ampid>)key_value;
	if(AppMgr_Service)
        AppMgr_Service->fireKey_value_mapEvent(key_value);

    logVerbose(CAPI,"Capi.cpp::update_orgDate(fireKey_value_mapEvent)--->OUT");
}

// popup
void ShowPopupNotify(uint32_t popupId, uint32_t reason)
{
    logVerbose(CAPI,"ShowPopupNotify-->IN");
    if(AppMgr_Service)
        AppMgr_Service->fireNotifyPopupEvent(popupId, reason);
//        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_POPUP, std::vector<uint64_t>{popupId, reason}, std::vector<std::string>());
    logVerbose(CAPI,"ShowPopupNotify-->out");
}

void ShowPopupExNotify(uint32_t popupId, uint32_t reason,const std::string& message)
{
    logVerbose(CAPI,"ShowPopupExNotify-->IN");
    if(AppMgr_Service)
        AppMgr_Service->fireNotifyPopupExEvent(popupId, reason,message);
//        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_POPUP, std::vector<uint64_t>{popupId, reason}, std::vector<std::string>());
    logVerbose(CAPI,"ShowPopupExNotify-->out");
}

void popupUpdateInfoList(const std::vector<std::pair<uint64_t, PopInfo>> & vp)
{
    std::vector<::v0_1::org::neusoft::AppMgr::PopupInfo> _popupList;
    for (auto & pi : vp) {
        _popupList.emplace_back(pi.first, pi.second.Pt, pi.second.Value, pi.second.Ampid, pi.second.ScreenIndex);
    }
    logVerbose(CAPI,"popupUpdateInfoList-->IN");
    if(AppMgr_Service)
        AppMgr_Service->fireUpdatePopupListEvent(_popupList);
    logVerbose(CAPI,"popupUpdateInfoList-->OUT");
}

// IME
void IMEFocusChangedNotify(uint32_t app)
{
    DomainVerboseLog chatter(CAPI, formatText("IMEFocusChangedNotify(%#x)", app));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_IME_FOCUSCHANGED, std::vector<uint64_t>{app}, std::vector<std::string>());
}

void SwitchTitlebarVisibilityNotify(uint32_t titlebarID, uint32_t visibility, uint32_t style)
{
    DomainVerboseLog chatter(CAPI, formatText("SwitchTitlebarVisibilityNotify(%#x, %s, %d)", titlebarID, visibility?"true":"false", style));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_TITLEBARVISIBILITYSWITCH, std::vector<uint64_t>{titlebarID, visibility, style}, std::vector<std::string>());
}

void SwitchTitlebarDropDownStateNotify(uint32_t titlebarID, uint32_t isDropDown, uint32_t style)
{
    DomainVerboseLog chatter(CAPI, formatText("SwitchTitlebarDropDownStateNotify(%#x, %s, %d)", titlebarID, isDropDown?"true":"false", style));

    if(AppMgr_Service)
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_TITLEBARDROPDOWNSTATESWITCH, std::vector<uint64_t>{titlebarID, isDropDown, style}, std::vector<std::string>());
}

void IMEClosedNotify(uint32_t arg)
{
    logVerboseF(CAPI, "Capi.cpp::NotifyImeClosedEvent (arg: %d) ---> IN",arg);
    if(AppMgr_Service){
        AppMgr_Service->fireGeneralNotifyEvent(E_CAPI_NOTIFY_APPS_IME_CLOSED,
                                               std::vector<uint64_t>{arg, },
                                               std::vector<std::string>{});
    }
    logVerbose(CAPI, "Capi.cpp::NotifyImeClosedEvent--->OUT");
}
