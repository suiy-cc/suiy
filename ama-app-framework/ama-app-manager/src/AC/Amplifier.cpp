/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioCtrl.cpp
/// @brief contains the implementation of class AudioController
///
/// Created by wangqi on 2016/6/16.
/// this file contains the implementation of class AudioController
///

#include "Amplifier.h"
#include "AMClient.h"
#include "AudioSetting.h"
#include "rpc_proxy.h"

extern RpcProxy rp;
#ifdef CODE_FOR_EV
#define STEREO_ALL	1
#define TELEPHONE	5
#define MUTE		8
#elif CODE_FOR_HS5
#define STEREO_ALL		2
#define STEREO_DRIVER	3
#define STEREO_REAR		4
#define CENTER_POINT	5
#define TELEPHONE		7
#define MUTE			9
#elif CODE_FOR_HS7
#define STEREO_ALL		1
#define STEREO_DRIVER	2
#define STEREO_REAR		3
#define CENTER_POINT	5
#define TELEPHONE		7
#define MUTE			8
#else
#define STEREO_ALL		1
#define STEREO_DRIVER	2
#define STEREO_REAR		3
#define CENTER_POINT	5
#define TELEPHONE		7
#define MUTE			8
#endif

Amplifier::Amplifier()
:mIsMute(false)
,mCurAmpSrcSta(E_MEDIA_STATE)
{
    logVerbose(AC,"Amplifier::Amplifier()-->IN");

    mCurBalanceSet = UNSETTED_DATA;
    mCurFaderSet = UNSETTED_DATA;
    mCurAMPMode = UNSETTED_DATA;

    logVerbose(AC,"Amplifier::Amplifier()-->OUT");
}

Amplifier::~Amplifier()
{
    logVerbose(AC,"Amplifier::~Amplifier()-->IN");
    logVerbose(AC,"Amplifier::~Amplifier()-->OUT");
}

void Amplifier::initDefaultMode(void)
{
    logVerbose(AC,"Amplifier::initDefaultMode()-->IN");

    mCurAMPMode = 1;
    //set AMP-Mode
    rp.setAmpMode(mCurAMPMode);

    logVerbose(AC,"Amplifier::initDefaultMode()-->OUT");
}

void Amplifier::setAMPByCurSourceState()
{
    logVerbose(AC,"Amplifier::setAMPByCurSourceState()-->IN");

    if(!mIsMute){
        switch(mCurAmpSrcSta){
        case E_MEDIA_STATE: setAMPForMediaSrcSta(); break;
        case E_TUNER_STATE: setAMPForTunerSrcSta(); break;
        case E_PHONE_STATE: setAMPForPhoneSrcSta(); break;
        default:    logError(AC,"mCurAmpSrcSta is not defined!");   break;
        }
    }else{
        logInfo(AC,"Amplifier is mute state,can't set amp");
    }

    logVerbose(AC,"Amplifier::setAMPByCurSourceState()-->OUT");
}

void Amplifier::setAMPForPhoneSrcSta()
{
    logVerbose(AC,"Amplifier::setAMPForPhoneSrcSta()-->IN");

    #ifdef CODE_FOR_EV
    logDebug("coding for EV here");
    setCallMode_HQEV();
    #else
    logDebug("coding for HS7 and HS5 here");
    setCallMode();
    #endif

    logVerbose(AC,"Amplifier::setAMPForPhoneSrcSta()-->OUT");
}

void Amplifier::setAMPForTunerSrcSta()
{
    logVerbose(AC,"Amplifier::setAMPForTunerSrcSta()-->IN");

    #ifdef CODE_FOR_EV
    logDebug("coding for EV here");
    setNormalMode_HQEV();
    #else
    logDebug("coding for HS7 and HS5 here");
    int soundEnvironmental = AudioSetting::GetInstance()->getSoundEffectSet(E_SEAT_IVI);
    switch(soundEnvironmental){
    case E_Stereo:
        setStereoMode();
        break;
    case E_Stereo_driver:
        setStereoDriverMode();
        break;
    case E_Stereo_rear:
        setStereoRearMode();
        break;
    case E_CenterPoint:
        setStereoMode();
        break;
    default:
        logError(AC,"soundEnvironmental is not defined");
        break;
    }
    #endif

    logVerbose(AC,"Amplifier::setAMPForTunerSrcSta()-->OUT");
}

void Amplifier::setAMPForMediaSrcSta()
{
    logVerbose(AC,"Amplifier::setAMPForMediaSrcSta()-->IN");

    #ifdef CODE_FOR_EV
    logDebug("coding for EV here");
    setNormalMode_HQEV();
    #else
    logDebug("coding for HS7 and HS5 here");
    int soundEnvironmental = AudioSetting::GetInstance()->getSoundEffectSet(E_SEAT_IVI);
    switch(soundEnvironmental){
    case E_Stereo:
        setStereoMode();
        break;
    case E_Stereo_driver:
        setStereoDriverMode();
        break;
    case E_Stereo_rear:
        setStereoRearMode();
        break;
    case E_CenterPoint:
        setCenterPointMode();
        break;
    default:
        logError(AC,"soundEnvironmental is not defined");
        break;
    }
    #endif

    logVerbose(AC,"Amplifier::setAMPForMediaSrcSta()-->OUT");
}

void Amplifier::OnConnectStaChanged(ama_connectID_t connectID,bool isConnect)
{
    logVerbose(AC,"Amplifier::OnConnectStaChanged()-->IN");

    ama_focusType_t focusType = GET_FOCUSTYPE_FROM_CONNECTID(connectID);
    if((GET_DEVICETYPE_FROM_CONNECTID(connectID)==E_IVI_SPEAKER)
    &&(focusType > E_FOCUS_TYPE_SOFTKEY)
    &&(focusType < E_FOCUS_TYPE_MAX)){
        if(isConnect){
            ama_streamType_t streamType;

            streamType = GET_STREAMTYPE_FROM_CONNECTID(connectID);
            if((E_FOCUS_TYPE_PHONE==focusType)||(E_FOCUS_TYPE_BICALL==focusType)||(E_FOCUS_TYPE_ECALL==focusType)){
                logInfo(AC,"update mCurAmpSrcSta = E_PHONE_STATE");
                mCurAmpSrcSta = E_PHONE_STATE;
            }else if(E_FOCUS_TYPE_MEDIA==focusType){
                if((E_STREAM_FM==streamType)||(E_STREAM_AM==streamType)){
                    logInfo(AC,"update mCurAmpSrcSta = E_TUNER_STATE");
                    mCurAmpSrcSta = E_TUNER_STATE;
                }else{
                    logInfo(AC,"update mCurAmpSrcSta = E_MEDIA_STATE");
                    mCurAmpSrcSta = E_MEDIA_STATE;
                }
            }
            //we only need set Amp mode when route connect
            setAMPByCurSourceState();
        }
    }

    logVerbose(AC,"Amplifier::OnConnectStaChanged()-->OUT");
}

void Amplifier::setAMPWhenSettingChanged()
{
    logVerbose(AC,"Amplifier::setAMPWhenSettingChanged()-->IN");

    setAMPByCurSourceState();

    logVerbose(AC,"Amplifier::setAMPWhenSettingChanged()-->OUT");
}

void Amplifier::setCallMode_HQEV()//0,0,5
{
    logVerbose(AC,"Amplifier::setCallMode_HQEV()-->IN");

    if(0 != mCurBalanceSet){
        mCurBalanceSet = 0;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    if(0 != mCurFaderSet){
        mCurFaderSet = 0;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(TELEPHONE != mCurAMPMode){
        mCurAMPMode = TELEPHONE;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setCallMode_HQEV()-->OUT");
}

void Amplifier::setNormalMode_HQEV()//set,set,1
{
    logVerbose(AC,"Amplifier::setNormalMode_HQEV()-->IN");

    int balanceSet = AudioSetting::GetInstance()->getBalanceSet();
    if(balanceSet != mCurBalanceSet){
        mCurBalanceSet = balanceSet;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    int faderSet = AudioSetting::GetInstance()->getFaderSet();
    if(faderSet != mCurFaderSet){
        mCurFaderSet = faderSet;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(STEREO_ALL != mCurAMPMode){
        mCurAMPMode = STEREO_ALL;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setNormalMode_HQEV()-->OUT");
}

#ifndef CODE_FOR_EV
void Amplifier::setCallMode()//0,0,7
{
    logVerbose(AC,"Amplifier::setCallMode_HS7()-->IN");

    if(0 != mCurBalanceSet){
        mCurBalanceSet = 0;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    if(0 != mCurFaderSet){
        mCurFaderSet = 0;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(TELEPHONE != mCurAMPMode){
        mCurAMPMode = TELEPHONE;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setCallMode_HS7()-->OUT");
}

void Amplifier::setStereoMode()//set,set,1
{
    logVerbose(AC,"Amplifier::setStereoMode()-->IN");

    int balanceSet = AudioSetting::GetInstance()->getBalanceSet();
    if(balanceSet != mCurBalanceSet){
        mCurBalanceSet = balanceSet;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    int faderSet = AudioSetting::GetInstance()->getFaderSet();
    if(faderSet != mCurFaderSet){
        mCurFaderSet = faderSet;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(STEREO_ALL != mCurAMPMode){
        mCurAMPMode = STEREO_ALL;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setStereoMode()-->OUT");
}
void Amplifier::setStereoDriverMode()//set,set,2
{
    logVerbose(AC,"Amplifier::setStereoDriverMode()-->IN");

    int balanceSet = AudioSetting::GetInstance()->getBalanceSet();
    if(balanceSet != mCurBalanceSet){
        mCurBalanceSet = balanceSet;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    int faderSet = AudioSetting::GetInstance()->getFaderSet();
    if(faderSet != mCurFaderSet){
        mCurFaderSet = faderSet;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(STEREO_DRIVER != mCurAMPMode){
        mCurAMPMode = STEREO_DRIVER;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setStereoDriverMode()-->OUT");
}

void Amplifier::setStereoRearMode()//set,set,3
{
    logVerbose(AC,"Amplifier::setStereoRearMode()-->IN");

    int balanceSet = AudioSetting::GetInstance()->getBalanceSet();
    if(balanceSet != mCurBalanceSet){
        mCurBalanceSet = balanceSet;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    int faderSet = AudioSetting::GetInstance()->getFaderSet();
    if(faderSet != mCurFaderSet){
        mCurFaderSet = faderSet;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(STEREO_REAR != mCurAMPMode){
        mCurAMPMode = STEREO_REAR;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setStereoRearMode()-->OUT");
}

void Amplifier::setCenterPointMode()//set,set,5
{
    logVerbose(AC,"Amplifier::setCenterPointMode()-->IN");

    int balanceSet = AudioSetting::GetInstance()->getBalanceSet();
    if(balanceSet != mCurBalanceSet){
        mCurBalanceSet = balanceSet;
        AMClient::GetInstance()->setBalance(mCurBalanceSet);
    }

    int faderSet = AudioSetting::GetInstance()->getFaderSet();
    if(faderSet != mCurFaderSet){
        mCurFaderSet = faderSet;
        AMClient::GetInstance()->setFader(mCurFaderSet);
    }

    if(CENTER_POINT != mCurAMPMode){
        mCurAMPMode = CENTER_POINT;
        //set AMP-Mode
        rp.setAmpMode(mCurAMPMode);
    }

    logVerbose(AC,"Amplifier::setCenterPointMode()-->OUT");
}
#endif

void Amplifier::setAMPMuteState(bool isMute)
{
    logVerbose(AC,"Amplifier::setAMPMuteState()-->IN isMute",isMute);

    if(isMute){
        mIsMute = true;
        //set amp mute by rpc
        if(MUTE != mCurAMPMode){
            mCurAMPMode = MUTE;
            //set AMP-Mode
            rp.setAmpMode(mCurAMPMode);
        }
    }else{
        mIsMute = false;
        setAMPByCurSourceState();
    }

    logVerbose(AC,"Amplifier::setAMPMuteState()-->OUT");
}
