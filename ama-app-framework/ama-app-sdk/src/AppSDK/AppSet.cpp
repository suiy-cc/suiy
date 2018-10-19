/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppSet.cpp
/// @brief contains the implementation of Framework set API
///
/// Created by wangqi on 2017/05/11.
/// this file contains the implementation of Framework set API
///

#include "AppSet.h"
#include "AppManager.h"
#include "LogContext.h"
#include "AppIS.h"

void setByAppManager(ama_setType_e key,int value)
{
    logVerbose(SDK_SET,"setByAppManager-->>IN key:",key," value:",value);

    AppManager* pAppManager = AppManager::GetInstance();
    if(pAppManager != NULL){
        pAppManager->requestToSet(pAppManager->GetAMPID(),key,value);
    }else{
        logError(SDK_SET,"setByAppManager-->>pAppManager==NULL");
    }

    logVerbose(SDK_SET,"setByAppManager-->>OUT");
}

void setVolumeByAppManager(ama_setType_e volumeType,int volume,bool isEnableFeedBack)
{
    logVerbose(SDK_SET,"setByAppManager-->>IN volumeType:",volumeType," volume:",volume);

    AppManager* pAppManager;

    switch(volumeType){
        case E_MediaVolMixWithNavi:
        case E_MeidaVolMixWithRadar:
        case E_NaviVolMixWithPhone:
        case E_FrontRadarVol:
        case E_RearRadarVol:
        case E_IVINotificationVol:
        case E_IVISoftKeyVol:
        case E_IVIMediaVol:
        case E_SMSTTSVol:
        case E_VRTTSVol:
        case E_NaviTTSVol:
        case E_PhoneVol:
        case E_ECallVol:
        case E_LRseNotificationVol:
        case E_LRseSoftKeyVol:
        case E_LRseMediaVol:
        case E_RRseNotificationVol:
        case E_RRseSoftKeyVol:
        case E_RRseMediaVol:
            pAppManager = AppManager::GetInstance();
            if(pAppManager != NULL){
                pAppManager->requestToSet(pAppManager->GetAMPID(),volumeType,volume);
            }else{
                logError(SDK_SET,"setByAppManager-->>pAppManager==NULL");
            }

            if(isEnableFeedBack){
                playFeedbackSound(volumeType,volume);
            }
            break;
        default:
            logError(SDK_SET,"we don't handle this volumeType here");
            break;
    }

    logVerbose(SDK_SET,"setByAppManager-->>OUT");
}

std::string AMGR_getState(ama_stateType_e state_type)
{
	logVerbose(SDK_SET,"AMGR_getState-->>IN state_type:",state_type);

    AppManager* pAppManager = AppManager::GetInstance();
    if(pAppManager != NULL){
        return pAppManager->AMGRGetState(pAppManager->GetAMPID(),state_type);
    }else{
        logError(SDK_SET,"AMGR_getState-->>pAppManager==NULL");
		return "unknown";
    }

    logVerbose(SDK_SET,"AMGR_getState-->>OUT");
}

void AMGR_regCb4StateChanged(AMGR_handleStateChange callback)
{
	logVerbose(SDK_SET,"AppSM.cpp::AMGR_regCb4StateChanged-->>IN");

	if(callback != NULL)
	{
		AppManager::GetInstance()->AMGR_changedNotify(callback);
	}
	else
	{
		logError(SDK_SET,"AppSM.cpp::AMGR_regCb4StateChanged::callback is NULL!");
	}

	logVerbose(SDK_SET,"AppSM.cpp::AMGR_regCb4StateChanged-->>OUT");
}
