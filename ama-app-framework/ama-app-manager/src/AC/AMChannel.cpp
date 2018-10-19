/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AMChannel.cpp
/// @brief contains the implementation of class AMChannel
///
/// Created by wangqi on 2017/03/28.
/// this file contains the implementation of class AMChannel
///

#include "AMChannel.h"
#include "AMClient.h"

AMChannel::AMChannel(AMAC_Channel_t AMChannelID)
:mBassGain(0)
,mMidGain(0)
,mTrebleGain(0)
,mBassSet(0)
,mMidSet(0)
,mTrebleSet(0)
,mIsChannelMute(false)
,mActiveGain(AM_MUTE_DB)
,mIsMediaActive(false)
{
    logVerbose(AC,"AMChannel::AMChannel()-->IN AMChannelID:",AMChannelID);

    mAMChannelID = AMChannelID;
    switch(mAMChannelID){
    case AMAC_CHANNEL_PRIMARY:  mAudioDeviceID=E_IVI_SPEAKER; break;
    case AMAC_CHANNEL_SECONDARY:  mAudioDeviceID=E_RSE_L_HEADSET; break;
    case AMAC_CHANNEL_SECONDARY2:  mAudioDeviceID=E_RSE_R_HEADSET; break;
    default: mAudioDeviceID=E_IVI_SPEAKER;logError(AC,"AMChannel is not defined");break;
    }

    logVerbose(AC,"AMChannel::AMChannel()-->OUT");
}

AMChannel::~AMChannel()
{
    logVerbose(AC,"AMChannel::~AMChannel()-->IN");
    logVerbose(AC,"AMChannel::~AMChannel()-->OUT");
}

void AMChannel::OnAudioSettingChanged()
{
    logVerbose(AC,"AMChannel::OnAudioSettingChanged()-->IN");

    int bassSet = AudioSetting::GetInstance()->getBassSet(mAudioDeviceID);
    int midSet = AudioSetting::GetInstance()->getMidSet(mAudioDeviceID);
    int trebleSet = AudioSetting::GetInstance()->getTrebleSet(mAudioDeviceID);

    if((mBassSet!=bassSet)||(mMidSet!=midSet)||(mTrebleSet!=trebleSet)){
        mBassSet = bassSet;
        mMidSet = midSet;
        mTrebleSet = trebleSet;
        updateBMTWhenCurStaChanged();
    }

    logVerbose(AC,"AMChannel::OnAudioSettingChanged()-->OUT");
}

void AMChannel::OnConnectStaChanged(ama_connectID_t connectID,bool isConnect)
{
    logVerbose(AC,"AMChannel::OnConnectStaChanged()-->IN");

    if(mAudioDeviceID==GET_DEVICETYPE_FROM_CONNECTID(connectID)){
        if(E_FOCUS_TYPE_MEDIA==GET_FOCUSTYPE_FROM_CONNECTID(connectID)){
            if(isConnect!=mIsMediaActive){
                mIsMediaActive = isConnect;
                updateBMTWhenCurStaChanged();
            }
        }
    }
    logVerbose(AC,"AMChannel::OnConnectStaChanged()-->OUT");
}

void AMChannel::setChannelMuteState(bool isEnableMute)
{
    logVerbose(AC,"AMChannel::setChannelMuteState()-->IN isEnableMute:",isEnableMute);

    if(isEnableMute != mIsChannelMute){
        ama_Error_e ret = AMClient::GetInstance()->setChannelMuteState(mAMChannelID,isEnableMute);
        if(E_OK == ret){
            mIsChannelMute = isEnableMute;
        }else{
            logError(AC,"set mute state of Channel:",ToHEX(mAMChannelID)," failed");
        }
    }else{
        logDebug(AC,"current mute state already right");
    }

    logVerbose(AC,"AMChannel::setChannelMuteState()-->OUT");
}

void AMChannel::setActiveGain(int activeGain)
{
    logVerbose(AC,"AMChannel::setActiveGain()-->IN activeGain:",activeGain);

    if(activeGain!=mActiveGain){
        mActiveGain = activeGain;
        updateBMTWhenCurStaChanged();
    }

    logVerbose(AC,"AMChannel::setActiveGain()-->OUT");
}

//we call this function when mXXXSet,mActiveGain,mIsMediaActive changed!!!
void AMChannel::updateBMTWhenCurStaChanged(void)
{
    logVerbose(AC,"AMChannel::updateBMTWhenCurStaChanged()-->IN");

    if(mIsMediaActive){
        int bassGain = AMClient::GetInstance()->getBassGain(mBassSet,mActiveGain);
        int midGain = AMClient::GetInstance()->getMidGain(mMidSet,mActiveGain);
        int trebleGain = AMClient::GetInstance()->getTrebleGain(mTrebleSet,mActiveGain);
        setBassGain(bassGain);
        setMidGain(midGain);
        setTrebleGain(trebleGain);
    }else{
        setBassGain(0);
        setMidGain(0);
        setTrebleGain(0);
    }

    logVerbose(AC,"AMChannel::updateBMTWhenCurStaChanged()-->OUT");
}

void AMChannel::setBassGain(int bassGain)
{
    logVerbose(AC,"AMChannel::setBassGain()-->IN bassGain:",bassGain);

    if(bassGain != mBassGain){
        ama_Error_e ret = AMClient::GetInstance()->setBassGain(mAMChannelID,bassGain);
        if(E_OK == ret){
            mBassGain = bassGain;
        }else{
            logError(AC,"set bass gain of Channel:",ToHEX(mAMChannelID)," failed");
        }
    }else{
        logDebug(AC,"current bassGain already right");
    }

    logVerbose(AC,"AMChannel::setBassGain()-->OUT");
}

void AMChannel::setMidGain(int midGain)
{
    logVerbose(AC,"AMChannel::setMidGain()-->IN midGain:",midGain);

    if(midGain != mMidGain){
        ama_Error_e ret = AMClient::GetInstance()->setMidGain(mAMChannelID,midGain);
        if(E_OK == ret){
            mMidGain = midGain;
        }else{
            logError(AC,"set midGain of Channel:",ToHEX(mAMChannelID)," failed");
        }
    }else{
        logDebug(AC,"current midGain already right");
    }

    logVerbose(AC,"AMChannel::setMidGain()-->OUT");
}

void AMChannel::setTrebleGain(int trebleGain)
{
    logVerbose(AC,"AMChannel::setTrebleGain()-->IN trebleGain:",trebleGain);

    if(trebleGain != mTrebleGain){
        ama_Error_e ret = AMClient::GetInstance()->setTrebleGain(mAMChannelID,trebleGain);
        if(E_OK == ret){
            mTrebleGain = trebleGain;
        }else{
            logError(AC,"set trebleGain of Channel:",ToHEX(mAMChannelID)," failed");
        }
    }else{
        logDebug(AC,"current trebleGain already right");
    }

    logVerbose(AC,"AMChannel::setTrebleGain()-->OUT");
}
