/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AMConnection.cpp
/// @brief contains the implementation of class AMConnection
///
/// Created by wangqi on 2017/03/20.
/// this file contains the implementation of class AMConnection
///

#include "AMConnection.h"
#include "AMClient.h"
#include "ConCtrl.h"

using namespace std::placeholders;

AMConnection::AMConnection(ama_connectID_t AMConnectID,ama_connectID_t ACConnectID)
{
    logVerbose(AC,"AMConnection::AMConnection()-->IN AMConnectID:",ToHEX(AMConnectID));

    mpFadeInThread = std::make_shared<FadeInThread>(std::bind(&AMConnection::setVolGainByAMClient,this,_1));
    mpFadeInThread->start();

    mAMConnectID = AMConnectID;
    mACConnectID = ACConnectID;
    if(E_INVALID_DEVICE!=GET_DEVICETYPE_FROM_CONNECTID(AMConnectID)){
        mIsValidConnection = true;
    }else{
        mIsValidConnection = false;
    }

    ama_streamType_t streamTypeID = GET_STREAMTYPE_FROM_CONNECTID(mACConnectID);
    if((streamTypeID==E_STREAM_MUSIC)||(streamTypeID==E_STREAM_VIDEO_AUDIO)
        ||(streamTypeID==E_STREAM_BT_AUDIO)){
        mIsActiveChannelConnection = true;
    }else{
        mIsActiveChannelConnection = false;
    }

    mIsConnect = false;
    mVolGain = AM_MUTE_DB;

    logVerbose(AC,"AMConnection::AMConnection()-->OUT");
}

AMConnection::~AMConnection()
{
    logVerbose(AC,"AMConnection::~AMConnection()-->IN AMConnectID:",ToHEX(mAMConnectID));

    mpFadeInThread->stop();

    logVerbose(AC,"AMConnection::~AMConnection()-->OUT");
}

void AMConnection::setDefProperties(void)
{
    logVerbose(AC,"AMConnection::setDefProperties()-->IN");

    mIsConnect = false;
    recordVolGain(AM_MUTE_DB);

    logVerbose(AC,"AMConnection::setDefProperties()-->OUT");
}

void AMConnection::recordVolGain(int volGain)
{
    logVerbose(AC,"AMConnection::recordVolGain()-->IN volGain:",volGain);

    mVolGain = volGain;
    if(mIsActiveChannelConnection){
        ConnectController::GetInstance()->setActiveVolGain(mACConnectID,mVolGain);
    }

    logVerbose(AC,"AMConnection::recordVolGain()-->OUT");
}

void AMConnection::connect(void)
{
    logVerbose(AC,"AMConnection::connect()-->IN AMConnectID:",ToHEX(mAMConnectID));

    if(mIsValidConnection){
        ama_Error_e ret;
        if(!mIsConnect){
            ret = AMClient::GetInstance()->connectByAMClient(mAMConnectID);
            if(E_OK == ret){
                mIsConnect = true;
            }else{
                logWarn(AC,"connect mAMConnectID:",mAMConnectID,"failed errorID = ",ret);
            }
        }else{
            logWarn(AC,"AMConnection already connect");
        }
    }else{
        logDebug(AC,"do nothing because it's a invalid connection");
    }

    logVerbose(AC,"AMConnection::connect()-->OUT");
}

void AMConnection::disconnect(void)
{
    logVerbose(AC,"AMConnection::disconnect()-->IN AMConnectID:",ToHEX(mAMConnectID));

    if(mIsValidConnection){
        ama_Error_e ret;
        if(mIsConnect){
            ret = AMClient::GetInstance()->disconnectByAMClient(mAMConnectID);
            if(E_OK == ret){
                setDefProperties();
            }else{
                logWarn(AC,"connect mAMConnectID:",mAMConnectID,"failed errorID = ",ret);
            }
        }else{
            logWarn(AC,"AMConnection already disconnect");
        }
    }else{
        logDebug(AC,"do nothing because it's a invalid connection");
    }

    logVerbose(AC,"AMConnection::disconnect()-->OUT");
}

void AMConnection::setVolGain(int gain)
{
    logVerbose(AC,"AMConnection::setVolGain()-->IN gain:",gain);

    if(mIsValidConnection){
        ama_volumeType_t volumeType = GET_VOLUMETYPE_FROM_CONNECTID(mAMConnectID);
        if(volumeType <= E_VOLUME_TYPE_SOFTKEY){
            setVolGainByAMClient(gain);
        }else{
            setVolGainFadeIN(gain);
        }
    }else{
        logDebug(AC,"do nothing because it's a invalid connection");
    }

    logVerbose(AC,"AMConnection::setVolGain()-->OUT");
}

void AMConnection::duckAudio(int durations_ms,int formGain,int toGain)
{
    logVerbose(AC,"AMConnection::duckAudio()-->IN durations_ms:",durations_ms," formGain:",formGain," toGain:",toGain);

    if(formGain!=toGain){
        if(mIsValidConnection){
            mpFadeInThread->setVolumeByFadeIn(E_DUCK_CARPLAY,durations_ms,formGain,toGain);
        }else{
            logDebug(AC,"do nothing because it's a invalid connection");
        }
    }else{//formGain==toGain
        logError(AC,"do nothing when formGain==toGain");
    }

    logVerbose(AC,"AMConnection::duckAudio()-->OUT");
}

void AMConnection::setVolGainFadeIN(int gain)
{
    logVerbose(AC,"AMConnection::setVolGainFadeIN()-->IN gain:",gain);

    if((mIsConnect)&&(gain!=mVolGain)){
        mpFadeInThread->setVolumeByFadeIn(E_SYSTEM_DEF,SYSTEM_DEFAULT_DURATION,mVolGain,gain);
    }else if(!mIsConnect){
        logError(AC,"can not set volGain when connection disconnect state");
    }else if(gain==mVolGain){
        logDebug(AC,"current volGain is already right");
    }

    logVerbose(AC,"AMConnection::setVolGainFadeIN()-->OUT");
}

bool AMConnection::setVolGainByAMClient(int gain)
{
    logVerbose(AC,"AMConnection::setVolGainByAMClient()-->IN gain:",gain);

    ama_Error_e ret;
    bool isSuccessed = false;
    if((mIsConnect)&&(gain!=mVolGain)){
        ret = AMClient::GetInstance()->setConnectionVolGain(mAMConnectID,gain);
        if(E_OK == ret){
            recordVolGain(gain);
            isSuccessed = true;
        }else{
            logWarn(AC,"set volGain for mAMConnectID:",mAMConnectID,"failed errorID = ",ret);
        }
    }else if(!mIsConnect){
        logWarn(AC,"can not set volGain when connection disconnect state");
    }else if(gain==mVolGain){
        logDebug(AC,"current volGain is already right");
        isSuccessed = true;
    }

    logVerbose(AC,"AMConnection::setVolGainByAMClient()-->OUT");
    return isSuccessed;
}
