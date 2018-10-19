/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "AudioStream.h"
#include "AudioStreamImpl.h"
#include "LogContext.h"
#include "UniqueID.h"
#include "NotifyWaiter.h"

#define NO_REQUEST_ID      (0)

/**********************************************AudioFocus class start****************************************/

AudioFocus::AudioFocus(ama_connectID_t connectID)
:mConnectID(connectID)
,mFocusSta(E_FOUCS_LOSE)
,mAudioDevice(GET_DEVICETYPE_FROM_CONNECTID(connectID))
,mpCallbackFunc(NULL)
,mFocusWaiterID(NO_REQUEST_ID)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",connectID);
}

AudioFocus::~AudioFocus()
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);
}

ama_focusSta_t AudioFocus::requestAudioFocus(audioFocusChangedCB pCallbackFunc)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);

    std::unique_lock<std::mutex> uLock(mMutexOfAction);

    mpCallbackFunc = pCallbackFunc;
    if(E_FOUCS_LOSE!=mFocusSta){
        logWarn(SDK_AC,"it's already had audio focus!");
    }else{
        AppAudioServiceImpl::GetInstance()->requestAudioDeviceFocus(mConnectID);

        mFocusWaiterID = GetUniqueID();
        RegisterWaiterID(mFocusWaiterID);
        Wait(mFocusWaiterID,3*1000);
    }

    logVerboseF(SDK_AC,"output args:focusState[%x]",mFocusSta);
    return mFocusSta;
}

ama_focusSta_t AudioFocus::abandonAudioFocus(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);

    std::unique_lock<std::mutex> uLock(mMutexOfAction);

    mpCallbackFunc = NULL;
    if(E_FOUCS_LOSE==mFocusSta){
        logWarn(SDK_AC,"mFocusSta already == E_FOUCS_LOSE!");
    }else{
        AppAudioServiceImpl::GetInstance()->releaseAudioDeviceFocus(mConnectID);

        mFocusWaiterID = GetUniqueID();
        RegisterWaiterID(mFocusWaiterID);
        Wait(mFocusWaiterID,3*1000);
    }

    logVerboseF(SDK_AC,"output args:focusState[%x]",mFocusSta);
    return mFocusSta;
}

ama_focusSta_t AudioFocus::getFocusSta(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);
    logVerboseF(SDK_AC,"output args:focusState[%x]",mFocusSta);
    return mFocusSta;
}

void AudioFocus::OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);
    logVerboseF(SDK_AC,"input args:connectID[%x],newFocusSta[%x],applyStreamID[%x]",connectID,newFocusSta,applyStreamID);

    if(mConnectID==connectID){
        if(applyStreamID==GET_STREAMID_FROM_CONNECTID(connectID)){
            mFocusSta = newFocusSta;
            if(NO_REQUEST_ID!=mFocusWaiterID){
                Notify(mFocusWaiterID);
                UnregisterWaiterID(mFocusWaiterID);
                mFocusWaiterID = NO_REQUEST_ID;
            }else{
                logError(SDK_AC,"there is no request or abandon action,but we received focus changed by applyStreamID==myself!");
            }
        }else{//focus changed for other stream request or abandon
            if(NULL != mpCallbackFunc){
                if(newFocusSta!=mFocusSta){
                    mFocusSta = newFocusSta;
                    mpCallbackFunc(mAudioDevice,newFocusSta,applyStreamID);
                }
            }else{
                //we still need to update focus state
                mFocusSta = newFocusSta;
                logError(SDK_AC,"Error status:audio focus changed but no callback function!");
            }
        }
    }
}

void AudioFocus::OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);
    logVerbose(SDK_AC,"do nothing in AudioFocus~");
}

/**********************************************AudioFocus class end******************************************/

/************************************AudioFocusOfVedioApp class start****************************************/

AudioFocusOfVedioApp::AudioFocusOfVedioApp(ama_connectID_t connectID)
:AudioFocus(connectID)
{
    mIsVideoAppShowing = false;
    mNotifyFocusSta = E_FOUCS_LOSE;
}

AudioFocusOfVedioApp::~AudioFocusOfVedioApp()
{

}

ama_focusSta_t AudioFocusOfVedioApp::requestAudioFocus(audioFocusChangedCB pCallbackFunc)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);

    std::unique_lock<std::mutex> uLock(mMutexOfAction);

    mpCallbackFunc = pCallbackFunc;
    if(E_FOUCS_LOSE!=mFocusSta){
        logWarn(SDK_AC,"it's already had audio focus!");
    }else{
        AppAudioServiceImpl::GetInstance()->requestAudioDeviceFocus(mConnectID);

        mFocusWaiterID = GetUniqueID();
        RegisterWaiterID(mFocusWaiterID);
        Wait(mFocusWaiterID,3*1000);
    }

    logVerboseF(SDK_AC,"output args:focusState[%x]",mNotifyFocusSta);
    return mNotifyFocusSta;
}

ama_focusSta_t AudioFocusOfVedioApp::abandonAudioFocus(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);

    std::unique_lock<std::mutex> uLock(mMutexOfAction);

    mpCallbackFunc = NULL;
    if(E_FOUCS_LOSE==mFocusSta){
        logWarn(SDK_AC,"mFocusSta already == E_FOUCS_LOSE!");
    }else{
        AppAudioServiceImpl::GetInstance()->releaseAudioDeviceFocus(mConnectID);

        mFocusWaiterID = GetUniqueID();
        RegisterWaiterID(mFocusWaiterID);
        Wait(mFocusWaiterID,3*1000);
    }

    logVerboseF(SDK_AC,"output args:focusState[%x]",mNotifyFocusSta);
    return mNotifyFocusSta;
}

ama_focusSta_t AudioFocusOfVedioApp::getFocusSta(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);
    logVerboseF(SDK_AC,"output args:focusState[%x]",mNotifyFocusSta);
    return mNotifyFocusSta;
}

void AudioFocusOfVedioApp::OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocus::mConnectID[%x]",mConnectID);
    logVerboseF(SDK_AC,"input args:connectID[%x],newFocusSta[%x],applyStreamID[%x]",connectID,newFocusSta,applyStreamID);

    if(mConnectID==connectID){
        if(applyStreamID==GET_STREAMID_FROM_CONNECTID(connectID)){
            mFocusSta = newFocusSta;
            mNotifyFocusSta = getNotifyFocusStaByCurSta();
            if(NO_REQUEST_ID!=mFocusWaiterID){
                Notify(mFocusWaiterID);
                UnregisterWaiterID(mFocusWaiterID);
                mFocusWaiterID = NO_REQUEST_ID;
            }else{
                logError(SDK_AC,"there is no request or abandon action,but we received focus changed by applyStreamID==myself!");
            }
        }else{//focus changed for other stream request or abandon action
            mFocusSta = newFocusSta;
            if(getNotifyFocusStaByCurSta()!=mNotifyFocusSta){
                mNotifyFocusSta = getNotifyFocusStaByCurSta();
                if(NULL != mpCallbackFunc){
                    mpCallbackFunc(mAudioDevice,mNotifyFocusSta,applyStreamID);
                }else{
                    logError(SDK_AC,"Error status:audio focus changed but no callback function!");
                }
            }
        }
    }
}

void AudioFocusOfVedioApp::OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioFocusOfVedioApp::mConnectID[%x]",mConnectID);
    logVerboseF(SDK_AC,"input args:ampid[%x],isVisible[%d]",ampid,isVisible);

    string itemID = GetItemID(ampid);
    if(   (("com.hs7.media-video.scr1"==itemID)&&(mAudioDevice==E_IVI_SPEAKER))
        ||(("com.hs7.media-video.scr2"==itemID)&&(mAudioDevice==E_RSE_L_HEADSET))
        ||(("com.hs7.media-video.scr3"==itemID)&&(mAudioDevice==E_RSE_R_HEADSET))   ){
        if(mIsVideoAppShowing!=isVisible){
            logInfo(SDK_AC,"video app visible state changed");
            mIsVideoAppShowing = isVisible;
            if(getNotifyFocusStaByCurSta()!=mNotifyFocusSta){
                mNotifyFocusSta = getNotifyFocusStaByCurSta();
                if(NULL != mpCallbackFunc){
                    mpCallbackFunc(mAudioDevice,mNotifyFocusSta,VIDEO_SHOW_STATE);
                }
            }
        }
    }
}

//call this function when visible or focus state changed
ama_focusSta_t AudioFocusOfVedioApp::getNotifyFocusStaByCurSta(void)
{
    if(E_FOUCS_GAIN==mFocusSta){
        if(mIsVideoAppShowing){
            return E_FOUCS_GAIN;
        }else{
            return E_FOUCS_LOSE_TRANSIENT;
        }
    }else if(E_FOUCS_LOSE_TRANSIENT==mFocusSta){
        return E_FOUCS_LOSE_TRANSIENT;
    }else{
        return E_FOUCS_LOSE;
    }
}

/***************************************AudioFocusOfVedioApp class end***************************************/

/*****************************************AudioStreamImpl class start****************************************/

AudioStreamImpl::AudioStreamImpl(ama_streamID_t streamID)
:mStreamID(streamID)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);

    if(E_STREAM_VIDEO_AUDIO==GET_STREAMTYPE_FROM_STREAMID(streamID)){
        mpSpeakerFocus = new AudioFocusOfVedioApp(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_SPEAKER));
        mpLHeadsetFocus = new AudioFocusOfVedioApp(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_RSE_L_HEADSET));
        mpRHeadsetFocus = new AudioFocusOfVedioApp(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_RSE_R_HEADSET));
        mpMicFocus = new AudioFocusOfVedioApp(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_MIC));
    }else{
        mpSpeakerFocus = new AudioFocus(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_SPEAKER));
        mpLHeadsetFocus = new AudioFocus(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_RSE_L_HEADSET));
        mpRHeadsetFocus = new AudioFocus(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_RSE_R_HEADSET));
        mpMicFocus = new AudioFocus(CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_MIC));
    }
}

AudioStreamImpl::~AudioStreamImpl()
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);

    delete(mpSpeakerFocus);
    delete(mpLHeadsetFocus);
    delete(mpRHeadsetFocus);
    delete(mpMicFocus);
}

ama_streamID_t AudioStreamImpl::streamID(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"output args:mStreamID[%x]",mStreamID);

    return mStreamID;
}

string AudioStreamImpl::streamIDStr(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"output args:mStreamID[%x]",mStreamID);

    return ToHEX(mStreamID);
}

ama_focusSta_t AudioStreamImpl::requestAudioFocus(ama_audioDeviceType_t audioDevice,audioFocusChangedCB func)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"input args:audioDevice[%x]",audioDevice);

    switch(audioDevice){
    case E_IVI_SPEAKER:  return mpSpeakerFocus->requestAudioFocus(func);
    case E_RSE_L_HEADSET:  return mpLHeadsetFocus->requestAudioFocus(func);
    case E_RSE_R_HEADSET:  return mpRHeadsetFocus->requestAudioFocus(func);
    case E_IVI_MIC:  return mpMicFocus->requestAudioFocus(func);
    default:    logError(SDK_AC,"audioDevice is not defined!"); return E_FOUCS_LOSE;
    }
}

ama_focusSta_t AudioStreamImpl::abandonAudioFocus(ama_audioDeviceType_t audioDevice)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"input args:audioDevice[%x]",audioDevice);

    switch(audioDevice){
    case E_IVI_SPEAKER:  return mpSpeakerFocus->abandonAudioFocus();
    case E_RSE_L_HEADSET:  return mpLHeadsetFocus->abandonAudioFocus();
    case E_RSE_R_HEADSET:  return mpRHeadsetFocus->abandonAudioFocus();
    case E_IVI_MIC:  return mpMicFocus->abandonAudioFocus();
    default:    logError(SDK_AC,"audioDevice is not defined!"); return E_FOUCS_LOSE;
    }
}

ama_focusSta_t AudioStreamImpl::getFocusState(ama_audioDeviceType_t audioDevice)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"input args:audioDevice[%x]",audioDevice);

    switch(audioDevice){
    case E_IVI_SPEAKER:  return mpSpeakerFocus->getFocusSta();
    case E_RSE_L_HEADSET:  return mpLHeadsetFocus->getFocusSta();
    case E_RSE_R_HEADSET:  return mpRHeadsetFocus->getFocusSta();
    case E_IVI_MIC:  return mpMicFocus->getFocusSta();
    default:    logError(SDK_AC,"audioDevice is not defined!"); return E_FOUCS_LOSE;
    }
}

ama_Error_e AudioStreamImpl::setStreamMuteStateSync(bool isMute)//for all device
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    AppAudioServiceImpl::GetInstance()->setStreamMuteState(mStreamID,isMute);

    return E_OK;
}

void AudioStreamImpl::OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"input args:connectID[%x],newFocusSta[%x],applyStreamID[%x]",connectID,newFocusSta,applyStreamID);
    if(mStreamID==GET_STREAMID_FROM_CONNECTID(connectID)){
        mpSpeakerFocus->OnAudioFocusChanged(connectID,newFocusSta,applyStreamID);
        mpLHeadsetFocus->OnAudioFocusChanged(connectID,newFocusSta,applyStreamID);
        mpRHeadsetFocus->OnAudioFocusChanged(connectID,newFocusSta,applyStreamID);
        mpMicFocus->OnAudioFocusChanged(connectID,newFocusSta,applyStreamID);
    }
}

void AudioStreamImpl::OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    logVerboseF(SDK_AC,"AudioStreamImpl::mStreamID[%x]",mStreamID);
    logVerboseF(SDK_AC,"input args:ampid[%x],isVisible[%d]",ampid,isVisible);
    //audio focus only care vedio app visible state
    if("com.hs7.media-video"==(GetItemID(ampid).substr(0,19))){
        mpSpeakerFocus->OnAppVisibleStaChanged(ampid,isVisible);
        mpLHeadsetFocus->OnAppVisibleStaChanged(ampid,isVisible);
        mpRHeadsetFocus->OnAppVisibleStaChanged(ampid,isVisible);
        mpMicFocus->OnAppVisibleStaChanged(ampid,isVisible);
    }
}

/*****************************************AudioStreamImpl class end****************************************/
