/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "InformingSoundImpl.h"

#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/org/neusoft/InformingSoundProxy.hpp>

#include <string>
#include <memory>


LOG_DECLARE_CONTEXT(SDK_IS);

// Common API
std::shared_ptr<v0_1::org::neusoft::InformingSoundProxy<>> sInformingSoundProxy;

//Ampid will be replaced in Informing sound service,so we just use 0x0000 is ok!
const ama_AMPID_t IS_SDK_AMPID = 0x0000;

InformingSoundImpl::InformingSoundImpl()
:mIsInited(false)
{
}

InformingSoundImpl::~InformingSoundImpl()
{
}

void InformingSoundImpl::init(int tryTimeMs)
{
    RegisterContext(SDK_IS,"SIS");
    if(!mIsInited){
        logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");
        CommonAPI::Runtime::setProperty("LogContext", "SIS");
        CommonAPI::Runtime::setProperty("LibraryBase", "InformingSound");

        std::string domain = "";
        std::string instance = "";
        std::string connection = "";

        // initialize CAPI
        std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        sInformingSoundProxy = runtime->buildProxy<::v0_1::org::neusoft::InformingSoundProxy>(domain,instance, connection);

        int tryCnt = 0;
        int maxTryCnt = tryTimeMs*10;
        while (!sInformingSoundProxy->isAvailable()){
            usleep(100);
            tryCnt++;
            if(tryCnt>maxTryCnt){
                logError(SDK_IS,"IS commonAPI connection failed!!!");
                return;
            }
        }
        logInfoF(SDK_IS,"InformingSound CommonAPI service Available! spend %d us \n",(tryCnt*100));

        mIsInited = true;
        logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
    }else{
        logDebug(SDK_IS,"InformingSoundImpl already init!");
    }
}

void InformingSoundImpl::playSoftKeySound(ama_audioDeviceType_t deviceType)
{
    if(!mIsInited){
        init(300);
        logDebug(SDK_IS,"init() before request action");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");

    if(sInformingSoundProxy){
        ama_streamID_t streamID = CREATE_STREAMID_BY_AMPID_STREAMTYPE(IS_SDK_AMPID,E_STREAM_SOFTKEY);
        ama_connectID_t connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,deviceType);
        CommonAPI::CallStatus callStatus;
        sInformingSoundProxy->reqToPlayInformingSound(connectID,true, callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS){
            logError(SDK_IS, "failed calling sInformingSoundProxy::reqToPlayInformingSound()!");
        }
    }else{
        logError(SDK_IS,"sInformingSoundProxy is NULL!!");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundImpl::playPromptSound(ama_audioDeviceType_t deviceType)
{
    if(!mIsInited){
        init(300);
        logDebug(SDK_IS,"init() before request action");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");

    if(sInformingSoundProxy){
        ama_streamID_t streamID = CREATE_STREAMID_BY_AMPID_STREAMTYPE(IS_SDK_AMPID,E_STREAM_NOTIFICATION);
        ama_connectID_t connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,deviceType);
        CommonAPI::CallStatus callStatus;
        sInformingSoundProxy->reqToPlayInformingSound(connectID, true,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS){
            logError(SDK_IS, "failed calling sInformingSoundProxy::reqToPlayInformingSound()!");
        }
    }else{
        logError(SDK_IS,"sInformingSoundProxy is NULL!!");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundImpl::playBackdoorPromptSound()
{
    if(!mIsInited){
        init(300);
        logDebug(SDK_IS,"init() before request action");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");

    if(sInformingSoundProxy){
        ama_streamID_t streamID = CREATE_STREAMID_BY_AMPID_STREAMTYPE(IS_SDK_AMPID,E_STREAM_BACKDOOR_NOTI);
        //backdoor prompt sound only play in ivi speaker
        ama_connectID_t connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_SPEAKER);
        CommonAPI::CallStatus callStatus;
        sInformingSoundProxy->reqToPlayInformingSound(connectID, true,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS){
            logError(SDK_IS, "failed calling sInformingSoundProxy::reqToPlayInformingSound()!");
        }
    }else{
        logError(SDK_IS,"sInformingSoundProxy is NULL!!");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundImpl::playFeedbackSound(int volumeType,int volume)
{
    if(!mIsInited){
        init(300);
        logDebug(SDK_IS,"init() before request action");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");

    if(sInformingSoundProxy){
        CommonAPI::CallStatus callStatus;
        sInformingSoundProxy->reqToPlayFeedbackSound(volumeType, volume, callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS){
            logError(SDK_IS, "failed calling sInformingSoundProxy::playFeedbackSound()!");
        }
    }else{
        logError(SDK_IS,"sInformingSoundProxy is NULL!!");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundImpl::playRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel)
{
    if(!mIsInited){
        init(300);
        logDebug(SDK_IS,"init() before request action");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");

    if(sInformingSoundProxy){
        CommonAPI::CallStatus callStatus;
        sInformingSoundProxy->reqToPlaytRadarSound(soundType,highestLevelChannel,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS){
            logError(SDK_IS, "failed calling sInformingSoundProxy::reqToPlaytRadarSound()!");
        }
    }else{
        logError(SDK_IS,"sInformingSoundProxy is NULL!!");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
}

void InformingSoundImpl::playAVMAlertSound(bool isPlay)
{
    if(!mIsInited){
        init(300);
        logDebug(SDK_IS,"init() before request action");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>IN");

    if(sInformingSoundProxy){
        ama_streamID_t streamID = CREATE_STREAMID_BY_AMPID_STREAMTYPE(IS_SDK_AMPID,E_STREAM_AVM_ALERT);
        ama_connectID_t connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_SPEAKER);
        CommonAPI::CallStatus callStatus;
        sInformingSoundProxy->reqToPlayInformingSound(connectID,isPlay, callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS){
            logError(SDK_IS, "failed calling sInformingSoundProxy::reqToPlayInformingSound()!");
        }
    }else{
        logError(SDK_IS,"sInformingSoundProxy is NULL!!");
    }

    logVerbose(SDK_IS,"InformingSoundImpl::",__FUNCTION__,"-->>OUT");
}
