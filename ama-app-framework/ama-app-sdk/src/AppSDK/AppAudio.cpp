/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppAudio.cpp
/// @brief contains the implementation of informingSound API
///
/// Created by wangqi on 2016/11/21.
/// this file contains the implementation of informingSound API
///

#include "ama_audioTypes.h"
#include "AppAudio.h"
#include "AppAudioServiceImpl.h"
#include "LogContext.h"
#include "AppIS.h"

void reqSoftKeySound(void)
{
    logVerbose(SDK_AC,"reqSoftKeySound-->>IN");
    void* pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        ama_seatID_t seatID = GET_SEAT(pAudioService->getAMpid());
        //defined in AppIS.h
        playSoftKeySound(seatID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }
    logVerbose(SDK_AC,"reqSoftKeySound-->>OUT");
}

void reqPromptSound(void)
{
    logVerbose(SDK_AC,"reqPromptSound-->>IN");
    void* pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        ama_seatID_t seatID = GET_SEAT(pAudioService->getAMpid());
        //defined in AppIS.h
        playPromptSound(seatID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }
    logVerbose(SDK_AC,"reqPromptSound-->>OUT");
}

void reqBackdoorPromptSound(void)
{
    logVerbose(SDK_AC,"reqBackdoorPromptSound-->>IN");

    //defined in AppIS.h
    playBackdoorPromptSound();

    logVerbose(SDK_AC,"reqBackdoorPromptSound-->>OUT");
}

void reqToPlayRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel)
{
    logVerbose(SDK_AC,"reqToPlayRadarSound-->>IN soundType:",ToHEX(soundType)," highestLevelChannel:",ToHEX(highestLevelChannel));

    //defined in AppIS.h
    playRadarSound(soundType,highestLevelChannel);

    logVerbose(SDK_AC,"reqToPlayRadarSound-->>OUT");
}

void requestToUseSpeaker(ama_seatID_t seatID)
{
    logVerbose(SDK_AC,"requestToUseSpeaker-->>IN");
    void* pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        pAudioService->requestToUseSpeaker(seatID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }
    logVerbose(SDK_AC,"requestToUseSpeaker-->>OUT");
}

void abandonToUseSpeaker(ama_seatID_t seatID)
{
    logVerbose(SDK_AC,"abandonToUseSpeaker-->>IN");
    void* pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        pAudioService->abandonToUseSpeaker(seatID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }
    logVerbose(SDK_AC,"abandonToUseSpeaker-->>OUT");
}

ama_AMPID_t getLastConnectMediaApp(ama_audioDeviceType_t deviceID)
{
    logVerbose(SDK_AC,"getLastConnectMediaApp-->>IN");

    AppManager* pAppManager = AppManager::GetInstance();
    if(pAppManager != NULL){
        ama_AMPID_t ampid = pAppManager->reqToGetLastApp(deviceID);
        logDebug(SDK_AC,"lastMediaApp:",ampid);
        return ampid;
    }else{
        logError(SDK_SET,"getLastConnectMediaApp-->>pAppManager==NULL");
        return 0x0;
    }

    logVerbose(SDK_AC,"getLastConnectMediaApp-->>OUT");
}

void ListenTochangeOfAudioFocus(changeOfAudioFocusCB f)
{
    AppManager::GetInstance()->SetAudioFocusChangeNotifyCB(f);
}

void ListenTochangeOfAudioFocusEx(changeOfAudioFocusCBEx f)
{
    AppManager::GetInstance()->SetAudioFocusChangeNotifyCBEx(f);
}

bool isVolumeTypeActive(int volumeType)
{
    logVerbose(SDK_AC,"isVolumeTypeActive-->>IN");

    AppManager* pAppManager = AppManager::GetInstance();
    if(pAppManager != NULL){
        bool isActive = pAppManager->checkIsVolumeTypeActive(volumeType);
        logDebug(SDK_AC,"isActive:",isActive);
        return isActive;
    }else{
        logError(SDK_SET,"isVolumeTypeActive-->>pAppManager==NULL");
        return true;
    }

    logVerbose(SDK_AC,"isVolumeTypeActive-->>OUT");
}

void duckAudio(int durationsms,int volume)
{
    logVerbose(SDK_AC,"duckAudio-->>IN");

    AppManager::GetInstance()->requestToDuckAudio(0x0,true,durationsms,volume);

    logVerbose(SDK_AC,"duckAudio-->>OUT");
}

void unduckAudio(int durationsms)
{
    logVerbose(SDK_AC,"unduckAudio-->>IN");

    AppManager::GetInstance()->requestToDuckAudio(0x0,false,durationsms,0x0);

    logVerbose(SDK_AC,"unduckAudio-->>OUT");
}
