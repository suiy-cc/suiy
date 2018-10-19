/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppAudioStream.cpp
/// @brief contains the implementation of class AppAudioStream
///
/// Created by wangqi on 2016/8/4.
/// this file contains the implementation of class AppAudioStream
///

#include "AppAudioStream.h"
#include "AppAudioServiceImpl.h"
#include "LogContext.h"

AppAudioStream::AppAudioStream(ama_streamType_t streamType)
{
    logVerbose(SDK_AC,"AppAudioStream::AppAudioStream-->>in streamType=",streamType);
    pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        mStreamID = pAudioService->createNewStream(streamType,this);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }

    logVerbose(SDK_AC,"AppAudioStream::AppAudioStream-->>out ");
}

AppAudioStream::AppAudioStream(ama_seatID_t seatID,ama_appID_t appID,ama_streamType_t streamType)
{
    logVerbose(SDK_AC,"AppAudioStream::AppAudioStream-byService-->>in streamType=",streamType);
    pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        mStreamID = pAudioService->createNewStream(seatID,appID,streamType,this);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }

    logVerbose(SDK_AC,"AppAudioStream::AppAudioStream-byService-->>out ");
}

AppAudioStream::~AppAudioStream()
{
    logVerbose(SDK_AC,"AppAudioStream::~AppAudioStream-->>in");

    pAudioServiceImpl = AppAudioServiceImpl::GetInstance();

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        pAudioService->deleteStream(mStreamID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }

    logVerbose(SDK_AC,"AppAudioStream::~AppAudioStream-->>out");
}

ama_streamID_t AppAudioStream::streamID(void)
{
    logVerbose(SDK_AC,"AppAudioStream::streamID-->>in");
    logVerbose(SDK_AC,"AppAudioStream::streamID-->>out mStreamID=", ToHEX(mStreamID));
    return mStreamID;
}

string AppAudioStream::streamIDStr(void)
{
    logVerbose(SDK_AC,"AppAudioStream::streamIDStr-->>in");
    logVerbose(SDK_AC,"AppAudioStream::streamIDStr-->>out mStreamID=", ToHEX(mStreamID));
    return ToHEX(mStreamID);
}

ama_Error_e AppAudioStream::setStreamMuteState(bool isMute)
{
    logVerbose(SDK_AC,"AppAudioStream::setStreamMuteState-->>in isMute:",isMute);

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        pAudioService->setStreamMuteState(mStreamID,isMute);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
    }

    logVerbose(SDK_AC,"AppAudioStream::setStreamMuteState-->>out");
    return E_OK;
}

ama_Error_e AppAudioStream::requestAudioFocusForDefDev(void)
{
    logVerbose(SDK_AC,"AppAudioStream::requestAudioFocusForDefDev-->>IN");

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        ama_connectID_t defConnectID;
        if(GET_FOCUSTYPE_FROM_STREAMID(mStreamID) >= E_FOCUS_TYPE_MIC){       //for mic device
            defConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,E_IVI_MIC);
            logInfo(SDK_AC,"requestAudioFocusForDefDev-->>defConnectID=", ToHEX(defConnectID));
        }else{      //for Speaker LRSEHeadset RRSEHeadset audioDevice
            if(E_RSE_R_HEADSET<GET_DEF_DEVICETYPE_FROM_STREAMID(mStreamID)){//is a service with ampid 0xfxxx
                defConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,E_IVI_SPEAKER);
            }else{
                defConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,GET_DEF_DEVICETYPE_FROM_STREAMID(mStreamID));
            }
            logInfo(SDK_AC,"requestAudioFocusForDefDev-->>defConnectID=", ToHEX(defConnectID));
        }

        pAudioService->requestAudioDeviceFocus(defConnectID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
        return E_NULL_PTR;
    }

    logVerbose(SDK_AC,"AppAudioStream::requestAudioFocusForDefDev-->>OUT");
    return E_OK;
}

ama_Error_e AppAudioStream::requestAudioFocusForDev(ama_audioDeviceType_t audioDevice)
{
    logVerbose(SDK_AC,"AppAudioStream::requestAudioFocusForDev-->>IN audioDevice=", ToHEX(audioDevice));

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        if((GET_FOCUSTYPE_FROM_STREAMID(mStreamID)>=E_FOCUS_TYPE_MAX)&&(audioDevice!=E_IVI_MIC)){
            logError(SDK_AC,"mic stream can't request speaker or headset");
            return E_NULL_PTR;
        }
        ama_connectID_t connectID;
        connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,audioDevice);
        pAudioService->requestAudioDeviceFocus(connectID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
        return E_NULL_PTR;
    }

    logVerbose(SDK_AC,"AppAudioStream::requestAudioFocusForDev-->>OUT");
    return E_OK;
}

ama_Error_e AppAudioStream::abandonAudioFocusForDefDev(void)
{
    logVerbose(SDK_AC,"AppAudioStream::abandonAudioFocusForDefDev-->>IN");
    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        ama_connectID_t defConnectID;
        if(GET_FOCUSTYPE_FROM_STREAMID(mStreamID) >= E_FOCUS_TYPE_MIC){       //for mic device
            defConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,E_IVI_MIC);
            logInfo(SDK_AC,"abandonAudioFocusForDefDev-->>defConnectID=", ToHEX(defConnectID));
        }else{      //for Speaker LRSEHeadset RRSEHeadset audioDevice
            if(E_RSE_R_HEADSET<GET_DEF_DEVICETYPE_FROM_STREAMID(mStreamID)){//is a service with ampid 0xfxxx
                defConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,E_IVI_SPEAKER);
            }else{
                defConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,GET_DEF_DEVICETYPE_FROM_STREAMID(mStreamID));
            }
            logInfo(SDK_AC,"abandonAudioFocusForDefDev-->>defConnectID=", ToHEX(defConnectID));
        }

        pAudioService->releaseAudioDeviceFocus(defConnectID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
        return E_NULL_PTR;
    }
    logVerbose(SDK_AC,"AppAudioStream::abandonAudioFocusForDefDev-->>OUT");
    return E_OK;
}

ama_Error_e AppAudioStream::abandonAudioFocusForDev(ama_audioDeviceType_t audioDevice)
{
    logVerbose(SDK_AC,"AppAudioStream::abandonAudioFocusForDev-->>IN audioDevice=", ToHEX(audioDevice));

    auto pAudioService = static_cast<AppAudioServiceImpl*>(pAudioServiceImpl);
    if(NULL != pAudioService){
        if((GET_FOCUSTYPE_FROM_STREAMID(mStreamID)>=E_FOCUS_TYPE_MAX)&&(audioDevice!=E_IVI_MIC)){
            logError(SDK_AC,"mic stream can't request speaker or headset");
            return E_NULL_PTR;
        }
        ama_connectID_t connectID;
        connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(mStreamID,audioDevice);
        pAudioService->releaseAudioDeviceFocus(connectID);
    }else{
        logError(SDK_AC,"pAudioServiceImpl == NULL");
        return E_NULL_PTR;
    }

    logVerbose(SDK_AC,"AppAudioStream::abandonAudioFocusForDev-->>OUT");
    return E_OK;
}

void AppAudioStream::OnAudioFocusChangedForDefDev(ama_focusSta_t newFocusSta)
{
    logVerbose(SDK_AC,"AppAudioStream::OnAudioFocusChangedForDefDev()-->>IN");
    logVerbose(SDK_AC,"newFocusSta=",ToHEX(newFocusSta));

    logVerbose(SDK_AC,"AppAudioStream::OnAudioFocusChangedForDefDev()-->>OUT");
}

void AppAudioStream::OnAudioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta)
{
    logVerbose(SDK_AC,"AppAudioStream::OnAudioFocusChanged()-->>IN");
    logVerbose(SDK_AC,"audioDevice=",ToHEX(audioDevice));
    logVerbose(SDK_AC,"newFocusSta=",ToHEX(newFocusSta));

    logVerbose(SDK_AC,"AppAudioStream::OnAudioFocusChanged()-->>OUT");
}

void AppAudioStream::OnAudioFocusChangedEx(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    logVerbose(SDK_AC,"AppAudioStream::OnAudioFocusChanged()-->>IN");
    logVerbose(SDK_AC,"audioDevice=",ToHEX(audioDevice));
    logVerbose(SDK_AC,"newFocusSta=",ToHEX(newFocusSta));
    logVerbose(SDK_AC,"applyStreamID=",ToHEX(applyStreamID));

    logVerbose(SDK_AC,"AppAudioStream::OnAudioFocusChanged()-->>OUT");
}

ama_Error_e AppAudioStream::duckAudio(int durationsms,int volume)
{
    logWarn(SDK_AC,"AppAudioStream::duckAudio-->>this function will be delete!");
    return E_OK;
}

ama_Error_e AppAudioStream::unduckAudio(int durationsms)
{
    logWarn(SDK_AC,"AppAudioStream::unduckAudio-->>this function will be delete!");
    return E_OK;
}
