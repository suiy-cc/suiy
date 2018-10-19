/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "AppManagerProxy.h"
#include "ProTask.h"

#include <memory>

#define E_POPUP_IG_NOT_COME     (11)
#define E_POPUP_TEMP_HIGH       (15)
#define E_POPUP_ID_NULL         (0xffffffff)
static PopupID popupIDOfIGNotCome = E_POPUP_ID_NULL;
static PopupID popupIDOfTempHigh = E_POPUP_ID_NULL;

AppManagerProxy::AppManagerProxy()
{
    SDKProperty property;
    property.appLogID = "COMS";
    property.appDescription = "common service.";

    AppSDKInit(property);
    RegisterContext(AMRP,"AMRP", "app manager proxy log context");
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");

    psInitPopup();
    powerSDKInit();

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

AppManagerProxy::~AppManagerProxy()
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");

    unregisterPowerStateChangedCB(powerStateChangedCallback);

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::startListeningEvents(void)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");
    ListenToAppStateNotify(appStatusChangedCallback);
    registerPowerStateChangedCB(powerStateChangedCallback);
    registerChangeOfPowerStaChangedCB(changeOfPowerStaChangedCallback);
    ListenTochangeOfAudioFocusEx(changeOfAudioFocusCallback);
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::appStatusChangedCallback(const AMPID ampid, const AppStatus appStatus)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_PRO_TASK_APPSTATE_CHANGED);
    pTask->setInt32Arg(ampid);
    pTask->setInt32Arg(appStatus);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::powerStateChangedCallback(uint8_t seatID,ama_PowerState_e powerState)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");
    logDebugF(AMRP,"-->>seatID:%d,powerState:%x \n",seatID,powerState);
    if(E_SEAT_IVI==seatID){
        auto pTask = std::make_shared<ProjectTask>();
        pTask->SetType(E_RPO_TASK_IVI_POWER_CHANGED);
        pTask->setInt32Arg(powerState);
        TaskDispatcher::GetInstance()->Dispatch(pTask);
    }
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::changeOfPowerStaChangedCallback(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");
    logDebugF(AMRP,"-->>seatID:%d,changeOfPowerSta:%x \n",seatID,changeOfPowerSta);

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_PRO_TASK_CHANGE_OF_POWER);
    pTask->setInt32Arg(seatID);
    pTask->setInt32Arg(changeOfPowerSta);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::changeOfAudioFocusCallback(
    ama_connectID_t connectID,ama_focusSta_t oldFocusSta,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_PRO_TASK_AUDIOFOCUS_CHANGED);
    pTask->setInt32Arg(connectID);
    pTask->setInt32Arg(oldFocusSta);
    pTask->setInt32Arg(newFocusSta);
    pTask->setInt32Arg(applyStreamID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::handleIGSingal(ama_PowerState_e powerState)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");
    logDebugF(AMRP,"-->>powerState:%x \n",powerState);

    if(E_PS_IG_NOT_COME_NOTIFY==powerState){
        logDebug(AMRP,"show IG not come popup");
        PopupInfo popupInfo;
        popupInfo.Pt = (PopupType)E_POPUP_IG_NOT_COME;
        popupInfo.ScreenIndex = 0;
        popupIDOfIGNotCome = psRequestPopup(popupInfo);
        psShowPopup(popupIDOfIGNotCome);
    }else if(E_PS_IG_COME_NOTIFY==powerState){
        if(E_POPUP_ID_NULL!=popupIDOfIGNotCome){
            logDebug(AMRP,"close IG not come popup");
            psClosePopup(popupIDOfIGNotCome);
            popupIDOfIGNotCome = E_POPUP_ID_NULL;
        }
    }else if(E_PS_ACCOFF==powerState){
        if(E_POPUP_ID_NULL!=popupIDOfIGNotCome){
            logDebug(AMRP,"close IG not come popup");
            psClosePopup(popupIDOfIGNotCome);
            popupIDOfIGNotCome = E_POPUP_ID_NULL;
        }
    }

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::handleAbnormalSignal(ama_PowerState_e powerState)
{
    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>IN");
    logDebugF(AMRP,"-->>powerState:%x \n",powerState);

    if(E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH==powerState){
        logDebug(AMRP,"show screen temp abnormal popup");
        //to make a popup to User
        PopupInfo popupInfo;
        popupInfo.Pt = (PopupType)E_POPUP_TEMP_HIGH;
        popupInfo.ScreenIndex = 0;
        popupIDOfTempHigh = psRequestPopup(popupInfo);
        psShowPopup(popupIDOfTempHigh);
    }else if(powerState&POWER_NORMAL_FLAG){
        if(E_POPUP_ID_NULL!=popupIDOfTempHigh){
            logDebug(AMRP,"close screen temp abnormal popup");
            psClosePopup(popupIDOfTempHigh);
            popupIDOfTempHigh = E_POPUP_ID_NULL;
        }
    }

    logVerbose(AMRP,"AppManagerProxy::",__FUNCTION__,"-->>OUT");
}

void AppManagerProxy::startFactoryApp(void)
{
    Intent("com.hs7.factory.scr1","startByLineTool","0");
}

void AppManagerProxy::startCarlifeApp(void)
{
    Intent("com.hs7.media-carlife.scr1","","",true);
}

void AppManagerProxy::startWelinkApp(void)
{
    Intent("com.hs7.welink.scr1","noAction","0");
}
