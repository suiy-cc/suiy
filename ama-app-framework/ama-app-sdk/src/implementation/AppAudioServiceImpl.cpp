/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppAudioServiceImpl.cpp
/// @brief contains the implementation of class AppAudioService
///
/// Created by wangqi on 2016/8/4.
/// this file contains the implementation of class AppAudioService
///

#include "AppAudioStream.h"
#include "AppAudioServiceImpl.h"
#include "AppManager.h"
#include "ama_audioTypes.h"
#include "ama_types.h"
#include "LogContext.h"
#include <mutex>

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

AppAudioServiceImpl* AppAudioServiceImpl::mpInstance = NULL;
static std::mutex mutexOfAppServiceImplInstance;

AppAudioServiceImpl::AppAudioServiceImpl()
:mpVideoStream(NULL)
,mVideoFocusSta(E_FOUCS_LOSE)
,mIsVisible(false)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::AppAudioServiceImpl-->>in");
    // pAppManager = AppManager::GetInstance();
    // if(pAppManager != NULL){
    //     mAMPID = pAppManager->GetAMPID();
    //     logInfo(SDK_AC,"AppAudioServiceImpl-->>mAMPID=", ToHEX(mAMPID));
    // }else{
    //     logError(SDK_AC,"AppAudioServiceImpl-->>pAppManager==NULL");
    // }

    mAMPID = AppManager::GetInstance()->GetAMPID();
    logInfo(SDK_AC,"AppAudioServiceImpl-->>mAMPID=", ToHEX(mAMPID));

    logVerbose(SDK_AC,"AppAudioServiceImpl::AppAudioServiceImpl-->>out");
}

AppAudioServiceImpl::~AppAudioServiceImpl()
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::~AppAudioServiceImpl-->>in");
    mutexOfAppServiceImplInstance.lock();
    if(AppAudioServiceImpl::mpInstance){
        delete AppAudioServiceImpl::mpInstance;
        AppAudioServiceImpl::mpInstance = NULL;
    }
    mutexOfAppServiceImplInstance.unlock();
    logVerbose(SDK_AC,"AppAudioServiceImpl::~AppAudioServiceImpl-->>out");
}

AppAudioServiceImpl* AppAudioServiceImpl::GetInstance()
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::GetInstance-->>in");
    if(NULL == AppAudioServiceImpl::mpInstance){
        mutexOfAppServiceImplInstance.lock();
        if(NULL == AppAudioServiceImpl::mpInstance){
            AppAudioServiceImpl::mpInstance = new AppAudioServiceImpl();
        }
        mutexOfAppServiceImplInstance.unlock();
    }
    return AppAudioServiceImpl::mpInstance;
    logVerbose(SDK_AC,"AppAudioServiceImpl::GetInstance-->>out");
}

ama_streamID_t AppAudioServiceImpl::createNewStream(ama_streamType_t streamType)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::createNewStream-->>in");

    ama_streamID_t streamIDWithNoIndex = CREATE_STREAMID_BY_AMPID_STREAMTYPE(mAMPID,streamType);

    ama_streamID_t newStreamID;
    if(generateNewStream(streamIDWithNoIndex,newStreamID)){
        logDebug(SDK_AC,"createNewStream-->>streamType=", ToHEX(streamType));
        logDebug(SDK_AC,"createNewStream-->>newStreamID=", ToHEX(newStreamID));
    }else{
        logError(SDK_AC,"system already generate 64 streamID! it can't generate new one!!!");
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::createNewStream-->>out");
    return newStreamID;
}

ama_streamID_t AppAudioServiceImpl::createNewStream(ama_streamType_t streamType,AppAudioStream*  pAppAudioStream)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::createNewStream-->>in");

    ama_streamID_t streamIDWithNoIndex = CREATE_STREAMID_BY_AMPID_STREAMTYPE(mAMPID,streamType);

    ama_streamID_t newStreamID;
    if(generateNewStream(streamIDWithNoIndex,newStreamID)){
        logDebug(SDK_AC,"createNewStream-->>streamType=", ToHEX(streamType));
        logDebug(SDK_AC,"createNewStream-->>newStreamID=", ToHEX(newStreamID));
    }else{
        logError(SDK_AC,"system already generate 64 streamID! it can't generate new one!!!");
    }

    mAppAudioStreamMap.insert(pair<ama_streamID_t,AppAudioStream*>(newStreamID,pAppAudioStream));
    if(E_STREAM_VIDEO_AUDIO==GET_STREAMTYPE_FROM_STREAMID(newStreamID)){
        logDebug(SDK_AC,"set mpVideoStream = pAppAudioStream");
        mpVideoStream = pAppAudioStream;
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::createNewStream-->>out");
    return newStreamID;
}

ama_streamID_t AppAudioServiceImpl::createNewStream(ama_seatID_t seatID,ama_appID_t appID,ama_streamType_t streamType,AppAudioStream* pAppAudioStream)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::createNewStream-->>in");

    ama_streamID_t streamIDWithNoIndex = CREATE_STREAMID_BY_SEAT_APP_STREAMTYPE(seatID,appID,streamType);

    ama_streamID_t newStreamID;
    if(generateNewStream(streamIDWithNoIndex,newStreamID)){
        logDebug(SDK_AC,"createNewStream-->>streamType=", ToHEX(streamType));
        logDebug(SDK_AC,"createNewStream-->>newStreamID=", ToHEX(newStreamID));
    }else{
        logError(SDK_AC,"system already generate 64 streamID! it can't generate new one!!!");
    }

    mAppAudioStreamMap.insert(pair<ama_streamID_t,AppAudioStream*>(newStreamID,pAppAudioStream));
    if(E_STREAM_VIDEO_AUDIO==GET_STREAMTYPE_FROM_STREAMID(newStreamID)){
        logDebug(SDK_AC,"set mpVideoStream = pAppAudioStream");
        mpVideoStream = pAppAudioStream;
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::createNewStream-->>out");
    return newStreamID;
}

bool AppAudioServiceImpl::generateNewStream(ama_streamID_t streamIDWithNoIndex,ama_streamID_t& newStreamID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::generateNewStream-->>in");

    uint8_t streamIndex = 0;
    newStreamID = SET_STREAMINDEX_FOR_STREAMID(streamIndex,streamIDWithNoIndex);
    for(uint8_t streamIndex=0;streamIndex<=MAX_STREAM_INDEX;streamIndex++){
        newStreamID = SET_STREAMINDEX_FOR_STREAMID(streamIndex,streamIDWithNoIndex);
        std::map<ama_streamID_t,AppAudioStream*>::iterator it = mAppAudioStreamMap.find(newStreamID);
        if(it!=mAppAudioStreamMap.end()){
            logDebug(SDK_AC,"streamID already exist! streamID:",newStreamID);
            continue;
        }else{
            logDebug(SDK_AC,"generateNewStream successed! streamID:",newStreamID);
            logVerbose(SDK_AC,"AppAudioServiceImpl::generateNewStream-->>out");
            return true;
        }
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::generateNewStream-->>out");
    newStreamID = 0x0;
    return false;
}

void AppAudioServiceImpl::deleteStream(ama_streamID_t streamID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::deleteStream()-->>IN streamID:",ToHEX(streamID));

    std::map<ama_streamID_t,AppAudioStream*>::iterator it;
    it = mAppAudioStreamMap.find(streamID);
    if(it!=mAppAudioStreamMap.end()){
        mAppAudioStreamMap.erase(it);
    }else{
        logWarn(SDK_AC,"can't find AppAudioStream");
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::deleteStream()-->>OUT");
}

ama_Error_e AppAudioServiceImpl::requestAudioDeviceFocus(const ama_connectID_t connectID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::requestAudioDeviceFocus-->>in");
    logInfo(SDK_AC,"requestAudioDeviceFocus-->>connectID=", ToHEX(connectID));
    logInfoF(SDK_ADBG,"connectID[0x%x]-->>requestAudioFocus()",connectID);

    AppManager::GetInstance()->requestAudioDeviceFocus(connectID);

    logVerbose(SDK_AC,"AppAudioServiceImpl::requestAudioDeviceFocus-->>out");
    return E_OK;
}

ama_Error_e AppAudioServiceImpl::releaseAudioDeviceFocus(const ama_connectID_t connectID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::releaseAudioDeviceFocus-->>in");
    logInfo(SDK_AC,"releaseAudioDeviceFocus-->>connectID=", ToHEX(connectID));
    logInfoF(SDK_ADBG,"connectID[0x%x]-->>abandonAudioFocus()",connectID);

    AppManager::GetInstance()->releaseAudioDeviceFocus(connectID);

    logVerbose(SDK_AC,"AppAudioServiceImpl::releaseAudioDeviceFocus-->>out");
    return E_OK;
}

ama_Error_e AppAudioServiceImpl::setStreamMuteState(const ama_streamID_t streamID,bool isMute)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::setStreamMuteState-->>in");
    logInfo(SDK_AC,"setStreamMuteState-->>isMute=",isMute);
    logInfoF(SDK_ADBG,"streamID[0x%x]-->>setStreamMuteState(isMute[%d])",streamID,isMute);

    AppManager::GetInstance()->setStreamMuteState(streamID,isMute);

    logVerbose(SDK_AC,"AppAudioServiceImpl::setStreamMuteState-->>out");
    return E_OK;
}

void AppAudioServiceImpl::OnAudioDeviceFocusChangedNotify(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::OnAudioDeviceFocusChangedNotify-->>in");
    logInfo(SDK_AC,"-->>connectID=", ToHEX(connectID),"newFocusSta=",ToHEX(newFocusSta),"applyStreamID=",ToHEX(applyStreamID));

    notifyFocusStaToObserver(connectID,newFocusSta,applyStreamID);

    std::map<ama_streamID_t,AppAudioStream*>::iterator it;
    ama_streamID_t streamID = GET_STREAMID_FROM_CONNECTID(connectID);
    it = mAppAudioStreamMap.find(streamID);
    if(it!=mAppAudioStreamMap.end()){
        logInfoF(SDK_ADBG,"received notify: connectID[0x%x] newFocusSta[0x%x] applyStreamID[0x%x]",connectID,newFocusSta,applyStreamID);
        auto pAppStream = mAppAudioStreamMap[streamID];
        if(NULL != pAppStream){
            if(E_STREAM_VIDEO_AUDIO==GET_STREAMTYPE_FROM_STREAMID(streamID)){
                logInfo(SDK_AC,"record mVideoFocusSta = ",ToHEX(newFocusSta));
                mVideoFocusSta = newFocusSta;
                notifyFocusStaChangedByCurSta(applyStreamID);
            }else{
                logInfo(SDK_AC,"pAppStream->OnAudioDeviceFocusChangedNotify()-->>audioDeviceID:",ToHEX(GET_DEVICETYPE_FROM_CONNECTID(connectID)));
                logInfo(SDK_ADBG,"call OnAudioFocusChanged()");
                logInfo(SDK_ADBG,"call OnAudioFocusChangedEx()");
                pAppStream->OnAudioFocusChanged(GET_DEVICETYPE_FROM_CONNECTID(connectID),newFocusSta);
                pAppStream->OnAudioFocusChangedEx(GET_DEVICETYPE_FROM_CONNECTID(connectID),newFocusSta,applyStreamID);
                if((GET_DEF_DEVICETYPE_FROM_STREAMID(connectID)) == (GET_DEVICETYPE_FROM_CONNECTID(connectID))){
                    logDebug(SDK_AC,"pAppStream->OnAudioFocusChangedForDefDev()-->>");
                    logInfo(SDK_ADBG,"call OnAudioFocusChangedForDefDev()");
                    pAppStream->OnAudioFocusChangedForDefDev(newFocusSta);
                }else if(E_IVI_MIC == (GET_DEVICETYPE_FROM_CONNECTID(connectID))){//if this stream request mic device
                    logDebug(SDK_AC,"pAppStream->OnAudioFocusChangedForDefDev()-->>");
                    logInfo(SDK_ADBG,"call OnAudioFocusChangedForDefDev()");
                    pAppStream->OnAudioFocusChangedForDefDev(newFocusSta);
                }
            }
        }else{
            logError(SDK_AC,"NULL == pAppStream");
        }
    }else{
        logDebug(SDK_AC,"can't find AppAudioStream");
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::OnAudioDeviceFocusChangedNotify-->>out");
}

void AppAudioServiceImpl::requestToUseSpeaker(const ama_seatID_t seatID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::requestToUseSpeaker-->>IN seatID=", ToHEX(seatID));

    AppManager::GetInstance()->requestToUseSpeaker(seatID);

    logVerbose(SDK_AC,"AppAudioServiceImpl::requestToUseSpeaker-->>OUT");
}

void AppAudioServiceImpl::abandonToUseSpeaker(const ama_seatID_t seatID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::abandonToUseSpeaker-->>IN seatID=", ToHEX(seatID));

    AppManager::GetInstance()->abandonToUseSpeaker(seatID);

    logVerbose(SDK_AC,"AppAudioServiceImpl::abandonToUseSpeaker-->>OUT");
}

ama_AMPID_t AppAudioServiceImpl::getAMpid(void)
{
    return mAMPID;
}

void AppAudioServiceImpl::printAppAudioStreamMap(void)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::printAppAudioStreamMap-->>in");

    for(map<ama_streamID_t,AppAudioStream*>::iterator it=mAppAudioStreamMap.begin();
            it!=mAppAudioStreamMap.end(); ++it){
        logVerboseF(SDK_AC,"streamID=%d ,&pAppStream=%d",it->first,&(it->second));
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::printAppAudioStreamMap-->>out");
}

void AppAudioServiceImpl::updateVisibleSta(ama_AMPID_t ampid,bool isVisible)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::updateVisibleSta-->>in isVisible:",isVisible);

    notifyAppVisibleStaToObserver(ampid,isVisible);

    if(mpVideoStream!=NULL){
        logInfo(SDK_AC,"visible state changed of itemID:",GetItemID(ampid));
        if("com.hs7.media-video.scr1"==GetItemID(ampid)){
            if(isVisible!=mIsVisible){
                logInfo(SDK_AC,"isVisible changed");
                mIsVisible = isVisible;
                logInfo(SDK_AC,"mVideoFocusSta:",ToHEX(mVideoFocusSta));
                logInfoF(SDK_AC,"mpVideoStream:%d:",mpVideoStream);
                if((mpVideoStream!=NULL)&&(mVideoFocusSta==E_FOUCS_GAIN)){
                    //make sure 1:mpVideoStream!=NULL 2:visible changed 3:mVideoFocusSta==E_FOUCS_GAIN
                    notifyFocusStaChangedByCurSta(VIDEO_SHOW_STATE);
                }
            }
        }
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::updateVisibleSta-->>out");
}

void AppAudioServiceImpl::notifyFocusStaChangedByCurSta(ama_streamID_t applyStreamID)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::notifyFocusStaChangedByCurSta-->>in");

    ama_focusSta_t notifyFocus;

    if(mpVideoStream!=NULL){
        logDebug(SDK_AC,"mpVideoStream!=NULL");
        if(mVideoFocusSta==E_FOUCS_GAIN){
            if(mIsVisible){
                notifyFocus = E_FOUCS_GAIN;
            }else{
                notifyFocus = E_FOUCS_LOSE_TRANSIENT;
            }
        }else{
            notifyFocus = mVideoFocusSta;
        }

        //notify stream focus changed
        mpVideoStream->OnAudioFocusChangedForDefDev(notifyFocus);
        mpVideoStream->OnAudioFocusChanged(E_IVI_SPEAKER,notifyFocus);
        mpVideoStream->OnAudioFocusChangedEx(E_IVI_SPEAKER,notifyFocus,applyStreamID);
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::notifyFocusStaChangedByCurSta-->>out");
}

void AppAudioServiceImpl::attach(AudioFocusObserver *pObserver)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::attach-->>in");

    mObserverList.push_back(pObserver);

    logVerbose(SDK_AC,"AppAudioServiceImpl::attach-->>out");
}

void AppAudioServiceImpl::detach(AudioFocusObserver *pObserver)
{
    logVerbose(SDK_AC,"AppAudioServiceImpl::detach-->>in");

    for(std::list<AudioFocusObserver *>::iterator it = mObserverList.begin(); it != mObserverList.end(); ++it){
        if(*it == pObserver){
            logVerbose(SDK_AC,"remove pSettingObserver from mObserverList");
            mObserverList.erase(it);
            logVerbose(SDK_AC,"AudioSetting::detach()-->OUT");
            return;
        }
    }

    logVerbose(SDK_AC,"AppAudioServiceImpl::detach-->>out");
}

void AppAudioServiceImpl::notifyFocusStaToObserver(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    list<AudioFocusObserver *>::iterator it;
    for(it=mObserverList.begin(); it!=mObserverList.end(); ++it){
        (*it)->OnAudioFocusChanged(connectID,newFocusSta,applyStreamID);
    }
}

void AppAudioServiceImpl::notifyAppVisibleStaToObserver(ama_AMPID_t ampid,bool isVisible)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    list<AudioFocusObserver *>::iterator it;
    for(it=mObserverList.begin(); it!=mObserverList.end(); ++it){
        (*it)->OnAppVisibleStaChanged(ampid,isVisible);
    }
}
