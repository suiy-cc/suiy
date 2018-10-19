/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ConnectController.cpp
/// @brief contains the implementation of class ConnectController
///
/// Created by wangqi on 2017/03/20.
/// this file contains the implementation of class ConnectController
///

#include "ConCtrl.h"
#include "AMClient.h"

ConnectController::ConnectController()
{
    logVerbose(AC,"ConnectController::ConnectController()-->IN");

    mAMChannelMap[AMAC_CHANNEL_PRIMARY] = std::make_shared<AMChannel>(AMAC_CHANNEL_PRIMARY);
    mAMChannelMap[AMAC_CHANNEL_SECONDARY] = std::make_shared<AMChannel>(AMAC_CHANNEL_SECONDARY);
    mAMChannelMap[AMAC_CHANNEL_SECONDARY2] = std::make_shared<AMChannel>(AMAC_CHANNEL_SECONDARY2);

    logVerbose(AC,"ConnectController::ConnectController()-->OUT");
}

ConnectController::~ConnectController()
{
    logVerbose(AC,"ConnectController::~ConnectController()-->IN");
    logVerbose(AC,"ConnectController::~ConnectController()-->OUT");
}

void ConnectController::requestConnect(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"ConnectController::requestConnect()-->IN ACConnectID:",ToHEX(ACConnectID));

    saveRequest(ACConnectID,true);
    tryToConnectByRequest(ACConnectID);

    logVerbose(AC,"ConnectController::requestConnect()-->OUT");
}

void ConnectController::requestDisconnect(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"ConnectController::requestDisconnect()-->IN ACConnectID:",ToHEX(ACConnectID));

    saveRequest(ACConnectID,false);
    tryToConnectByRequest(ACConnectID);

    logVerbose(AC,"ConnectController::requestDisconnect()-->OUT");
}

void ConnectController::requestSetVolGain(ama_connectID_t ACConnectID, int gain)
{
    logVerbose(AC,"ConnectController::requestSetVolGain()-->IN ACConnectID:",ToHEX(ACConnectID)," gain:",gain);

    saveRequest(ACConnectID,gain);
    tryToSetVolByRequest(ACConnectID);

    logVerbose(AC,"ConnectController::requestSetVolGain()-->OUT");
}

void ConnectController::requestDuckAudio(ama_connectID_t ACConnectID, int durations_ms,int duckVolume)
{
    logVerbose(AC,"ConnectController::requestDuckAudio()-->IN ACConnectID:",ToHEX(ACConnectID)," durations_ms:",durations_ms," duckVolume:",duckVolume);

    saveDuckRequest(ACConnectID,(100-duckVolume));
    //we need to know gain with duck,so we save duck info first;
    tryToSetDuckAudio(ACConnectID,durations_ms,true);

    logVerbose(AC,"ConnectController::requestDuckAudio()-->OUT");
}

void ConnectController::requestUnduckAudio(ama_connectID_t ACConnectID, int durations_ms)
{
    logVerbose(AC,"ConnectController::requestUnduckAudio()-->IN ACConnectID:",ToHEX(ACConnectID)," durations_ms:",durations_ms);

    //we need to know gain with duck,so we save duck info later;
    tryToSetDuckAudio(ACConnectID,durations_ms,false);
    saveDuckRequest(ACConnectID,100);

    logVerbose(AC,"ConnectController::requestUnduckAudio()-->OUT");
}

void ConnectController::tryToConnectAndSetVolWhenAMSourceCreated(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"ConnectController::tryToConnectAndSetVolWhenAMSourceCreated()-->IN ACConnectID:",ToHEX(ACConnectID));

    saveRequest(ACConnectID,true);
    tryToConnectByRequest(ACConnectID);
    tryToSetVolByRequest(ACConnectID);

    logVerbose(AC,"ConnectController::tryToConnectAndSetVolWhenAMSourceCreated()-->OUT");
}

void ConnectController::setActiveVolGain(ama_connectID_t ACConnectID,int volGain)
{
    logVerbose(AC,"ConnectController::setActiveVolGain()-->IN ACConnectID:",ToHEX(ACConnectID)," volGain:",volGain);

    AMAC_Channel_t AMChannelID = AMClient::GetInstance()->getAMACChannelByConnectID(ACConnectID);
    if(AMChannelID==AMAC_CHANNEL_NULL){
        logError(AC,"ACConnectID don't have a AMChannel");
        return;
    }

    shared_ptr<AMChannel> pAMChannal = mAMChannelMap[AMChannelID];
    if(pAMChannal){
        pAMChannal->setActiveGain(volGain);
    }else{
        logError(AC,"ConnectController don't create aAMChannel for AMChannelID:",AMChannelID);
    }

    logVerbose(AC,"ConnectController::setActiveVolGain()-->OUT");
}

void ConnectController::setAudioDeviceMuteState(ama_audioDeviceType_t audioDeviceID,bool isEnableMute)
{
    logVerbose(AC,"ConnectController::setAudioDeviceMuteState()-->IN audioDeviceID:",ToHEX(audioDeviceID)," isEnableMute:",isEnableMute);

    AMAC_Channel_t AMChannelID;
    switch(audioDeviceID){
    case E_IVI_SPEAKER:  AMChannelID=AMAC_CHANNEL_PRIMARY; break;
    case E_RSE_L_HEADSET:  AMChannelID=AMAC_CHANNEL_SECONDARY; break;
    case E_RSE_R_HEADSET:  AMChannelID=AMAC_CHANNEL_SECONDARY2; break;
    default: AMChannelID=AMAC_CHANNEL_PRIMARY;logError(AC,"audioDeviceID is not defined");break;
    }

    shared_ptr<AMChannel> pAMChannal = mAMChannelMap[AMChannelID];
    if(pAMChannal){
        pAMChannal->setChannelMuteState(isEnableMute);
    }else{
        logError(AC,"ConnectController don't create aAMChannel for AMChannelID:",AMChannelID);
    }

    logVerbose(AC,"ConnectController::setAudioDeviceMuteState()-->OUT");
}

void ConnectController::saveRequest(ama_connectID_t ACConnectID,bool isConnectIt)
{
    logVerbose(AC,"ConnectController::saveRequest()-->IN ACConnectID:",ToHEX(ACConnectID)," isConnectIt:",isConnectIt);

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        if((it->first)->ACConnectID()==ACConnectID){
            (it->first)->setConnectReq(isConnectIt);
            return;
        }
    }

    shared_ptr<ConnectRequest> pConnectRequest = std::make_shared<ConnectRequest>(ACConnectID);
    pConnectRequest->setConnectReq(isConnectIt);
    mRequestToConnectMap[pConnectRequest] = NULL;


    logVerbose(AC,"ConnectController::saveRequest()-->OUT");
}

void ConnectController::saveRequest(ama_connectID_t ACConnectID,int volGain)
{
    logVerbose(AC,"ConnectController::saveRequest()-->IN ACConnectID:",ToHEX(ACConnectID)," volGain:",volGain);

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        if((it->first)->ACConnectID()==ACConnectID){
            (it->first)->setVolGain(volGain);
            return;
        }
    }

    shared_ptr<ConnectRequest> pConnectRequest = std::make_shared<ConnectRequest>(ACConnectID);
    pConnectRequest->setVolGain(volGain);
    mRequestToConnectMap[pConnectRequest] = NULL;

    logVerbose(AC,"ConnectController::saveRequest()-->OUT");
}

void ConnectController::saveDuckRequest(ama_connectID_t ACConnectID,int duckPercentage)
{
    logVerbose(AC,"ConnectController::saveDuckRequest()-->IN ACConnectID:",ToHEX(ACConnectID)," duckPercentage:",duckPercentage);

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        if((it->first)->ACConnectID()==ACConnectID){
            (it->first)->setDuckPercentage(duckPercentage);
            return;
        }
    }

    shared_ptr<ConnectRequest> pConnectRequest = std::make_shared<ConnectRequest>(ACConnectID);
    pConnectRequest->setDuckPercentage(duckPercentage);
    mRequestToConnectMap[pConnectRequest] = NULL;

    logVerbose(AC,"ConnectController::saveDuckRequest()-->OUT");
}

void ConnectController::tryToConnectByRequest(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"ConnectController::tryToConnectByRequest()-->IN ACConnectID:",ToHEX(ACConnectID));

    ama_connectID_t AMConnectID = getAMConnectIDBySpeakerMode(ACConnectID);

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        if((it->first)->ACConnectID()==ACConnectID){
            if((it->second)){
                if((it->first)->isNeedToConnect()){
                    (it->second)->connect();
                }else{
                    (it->second)->disconnect();
                }
            }else{
                shared_ptr<AMConnection> pAMConnection = std::make_shared<AMConnection>(AMConnectID,ACConnectID);
                if((it->first)->isNeedToConnect()){
                    pAMConnection->connect();
                }else{
                    pAMConnection->disconnect();
                }
                (it->second) = pAMConnection;
            }
            return;
        }
    }

    logError(AC,"can't find connectRequest in mRequestToConnectMap->ACConnectID:",ToHEX(ACConnectID));

    logVerbose(AC,"ConnectController::tryToConnectByRequest()-->OUT");
}

void ConnectController::tryToSetVolByRequest(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"ConnectController::tryToSetVolByRequest()-->IN ACConnectID:",ToHEX(ACConnectID));

    ama_connectID_t AMConnectID = getAMConnectIDBySpeakerMode(ACConnectID);

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        if((it->first)->ACConnectID()==ACConnectID){
            if((it->second)){
                (it->second)->setVolGain(((it->first)->getVolGainWithDuck()));
            }else{
                //we need ctreat AMconnection when there isn't
                shared_ptr<AMConnection> pAMConnection = std::make_shared<AMConnection>(AMConnectID,ACConnectID);
                (it->second) = pAMConnection;
            }
            return;
        }
    }

    logError(AC,"can't find connectRequest in mRequestToConnectMap->ACConnectID:",ToHEX(ACConnectID));

    logVerbose(AC,"ConnectController::tryToSetVolByRequest()-->OUT");
}

void ConnectController::tryToSetDuckAudio(ama_connectID_t ACConnectID, int durations_ms,bool isDuck)
{
    logVerbose(AC,"ConnectController::tryToSetDuckAudio()-->IN ACConnectID:",ToHEX(ACConnectID));

    ama_connectID_t AMConnectID = getAMConnectIDBySpeakerMode(ACConnectID);

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        if((it->first)->ACConnectID()==ACConnectID){
            if((it->second)){
                int formGain;
                int toGain;
                if(isDuck){
                    formGain = (it->first)->getVolGainWithoutDuck();
                    toGain = (it->first)->getVolGainWithDuck();
                }else{
                    formGain = (it->first)->getVolGainWithDuck();
                    toGain = (it->first)->getVolGainWithoutDuck();
                }
                (it->second)->duckAudio(durations_ms,formGain,toGain);
            }else{
                //we need ctreat AMconnection when there isn't
                shared_ptr<AMConnection> pAMConnection = std::make_shared<AMConnection>(AMConnectID,ACConnectID);
                (it->second) = pAMConnection;
            }
            return;
        }
    }

    logError(AC,"can't find connectRequest in mRequestToConnectMap->ACConnectID:",ToHEX(ACConnectID));

    logVerbose(AC,"ConnectController::tryToSetDuckAudio()-->OUT");
}

ama_connectID_t ConnectController::getAMConnectIDBySpeakerMode(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"ConnectController::getAMConnectIDBySpeakerMode()-->IN ACConnectID:",ToHEX(ACConnectID));
    logVerbose(AC,"ConnectController::getAMConnectIDBySpeakerMode()-->OUT");
    return Speaker::GetInstance()->getAMConnectIDFrom(ACConnectID);
}

void ConnectController::refreshAMConnectionWhenSpeakerChanged(ama_seatID_t oldSeatID,ama_seatID_t newSeatID)
{
    logVerbose(AC,"ConnectController::refreshAMConnectionWhenSpeakerChanged()-->IN oldSeatID;",oldSeatID," newSeatID:",newSeatID);

    makeRightAMConnectionWhenSpeakerChanged(oldSeatID);

    makeRightAMConnectionWhenSpeakerChanged(newSeatID);

    logVerbose(AC,"ConnectController::refreshAMConnectionWhenSpeakerChanged()-->OUT");
}

void ConnectController::makeRightAMConnectionWhenSpeakerChanged(ama_seatID_t seatID)
{
    logVerbose(AC,"ConnectController::makeNewAMConnectionWhenSpeakerChanged()-->IN seatID;",seatID);

    ama_connectID_t newAMConnectID;
    ama_connectID_t ACConnectID;

    //check all connections
    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>>::iterator it;
    for(it=mRequestToConnectMap.begin();it!=mRequestToConnectMap.end();++it){
        //find connection from this seatID
        ACConnectID = (it->first)->ACConnectID();
        if(seatID==GET_SEATID_FROM_CONNECTID(ACConnectID)){
            shared_ptr<ConnectRequest> pConnectRequest = (it->first);
            shared_ptr<AMConnection> pOldAMConnection = (it->second);
            //disconnect old AMConnection
            if(pOldAMConnection){
                pOldAMConnection->disconnect();
            }

            //make new AMConnection
            newAMConnectID = getAMConnectIDBySpeakerMode(ACConnectID);
            shared_ptr<AMConnection> pNewAMConnection = std::make_shared<AMConnection>(newAMConnectID,ACConnectID);
            if(pConnectRequest->isNeedToConnect()){
                pNewAMConnection->connect();
                pNewAMConnection->setVolGain(pConnectRequest->getVolGainWithDuck());
            }
            mRequestToConnectMap[pConnectRequest] = pNewAMConnection;
        }
    }

    logVerbose(AC,"ConnectController::makeNewAMConnectionWhenSpeakerChanged()-->OUT");
}
