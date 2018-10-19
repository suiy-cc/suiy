/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Speaker.cpp
/// @brief contains the implementation of class Speaker
///
/// Created by wangqi on 2017/03/20.
/// this file contains the implementation of class Speaker
///

#include "Speaker.h"
#include "ConCtrl.h"
#include "TaskDispatcher.h"
#include "AudioSetting.h"
#include "ama_stateTypes.h"
#include "Capi.h"

Speaker::Speaker()
{
    logVerbose(AC,"Speaker::Speaker()-->IN");
    mWhichSeatUsingSpeaker = E_SEAT_IVI;
    logVerbose(AC,"Speaker::Speaker()-->OUT");
}

Speaker::~Speaker()
{
    logVerbose(AC,"Speaker::~Speaker()-->IN");
    logVerbose(AC,"Speaker::~Speaker()-->OUT");
}

ama_connectID_t Speaker::getAMConnectIDFrom(ama_connectID_t ACConnectID)
{
    logVerbose(AC,"Speaker::getAMConnectIDFrom()-->IN ACConnectID:",ToHEX(ACConnectID));
    ama_seatID_t seatID = (ama_seatID_t)(GET_SEATID_FROM_CONNECTID(ACConnectID));
    ama_streamID_t streamID= GET_STREAMID_FROM_CONNECTID(ACConnectID);
    ama_connectID_t AMConnectID;
    switch(mWhichSeatUsingSpeaker){
    case E_SEAT_IVI:
        AMConnectID = ACConnectID;
        break;
    case E_SEAT_RSE1:
        if(seatID==E_SEAT_IVI){
            AMConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_INVALID_DEVICE);
        }else if(seatID==E_SEAT_RSE1){
            AMConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_SPEAKER);
        }else if(seatID==E_SEAT_RSE2){
            AMConnectID = ACConnectID;
        }else{
            logError(AC,"Wrong seatID");
        }
        break;
    case E_SEAT_RSE2:
        if(seatID==E_SEAT_IVI){
            AMConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_INVALID_DEVICE);
        }else if(seatID==E_SEAT_RSE1){
            AMConnectID = ACConnectID;
        }else if(seatID==E_SEAT_RSE2){
            AMConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,E_IVI_SPEAKER);
        }else{
            logError(AC,"Wrong seatID");
        }
        break;
    default:
        logError(AC,"Wrong mWhichSeatUsingSpeaker");
        AMConnectID = ACConnectID;
        break;
    }
    logVerbose(AC,"Speaker::getAMConnectIDFrom()-->OUT AMConnectID:",AMConnectID);
    return AMConnectID;
}

void Speaker::setWhichSeatUsingSpeaker(ama_seatID_t seatID)
{
    logVerbose(AC,"Speaker::setWhichSeatUsingSpeaker()-->IN seatID:",ToHEX(seatID));
    bool isEnableRSEUseSpeaker = AudioSetting::GetInstance()->isEnableRSEUseSpeaker();
    logDebug(AC,"Speaker::setWhichSeatUsingSpeaker()-->isEnableRSEUseSpeaker:",isEnableRSEUseSpeaker);
    if((E_SEAT_IVI==seatID)||((E_SEAT_IVI!=seatID)&&(AudioSetting::GetInstance()->isEnableRSEUseSpeaker()))){
        if(seatID!=mWhichSeatUsingSpeaker){
            ama_seatID_t oldSeatID = mWhichSeatUsingSpeaker;
            mWhichSeatUsingSpeaker = seatID;
            tellSMSpeakerOwnerChanged();
            ConnectController::GetInstance()->refreshAMConnectionWhenSpeakerChanged(oldSeatID,seatID);
        }else{
            logDebug(AC,"mWhichSeatUsingSpeaker not changed");
        }
    }
    logVerbose(AC,"Speaker::setWhichSeatUsingSpeaker()-->OUT");
}

void Speaker::isEnableRSEUseSpeakerStaChanged(void)
{
    logVerbose(AC,"Speaker::isEnableRSEUseSpeakerStaChanged()-->IN");

    bool isEnableRSEUseSpeaker = AudioSetting::GetInstance()->isEnableRSEUseSpeaker();
    logDebug(AC,"Speaker::isEnableRSEUseSpeakerStaChanged()-->isEnableRSEUseSpeaker:",isEnableRSEUseSpeaker);
    if((E_SEAT_IVI!=mWhichSeatUsingSpeaker)&&(!isEnableRSEUseSpeaker)){
        setWhichSeatUsingSpeaker(E_SEAT_IVI);
    }

    logVerbose(AC,"Speaker::isEnableRSEUseSpeakerStaChanged()-->OUT");
}

void Speaker::abandonToUseSpeaker(ama_seatID_t seatID)
{
    logVerbose(AC,"Speaker::abandonToUseSpeaker()-->IN seatID:",ToHEX(seatID));
    if(seatID==mWhichSeatUsingSpeaker){
        logDebug(AC,"seatID is not using speaker now");
        setWhichSeatUsingSpeaker(E_SEAT_IVI);
    }else{
        logDebug(AC,"seatID is not using speaker now");
    }
    logVerbose(AC,"Speaker::abandonToUseSpeaker()-->OUT");
}

void Speaker::tellSMSpeakerOwnerChanged(void)
{
    logVerbose(AC,"Speaker::tellSMSpeakerOwnerChanged()-->IN");

    BroadcastAMGRSetNotify(E_SpeakerWho,to_string(mWhichSeatUsingSpeaker));

    logVerbose(AC,"Speaker::tellSMSpeakerOwnerChanged()-->OUT");
}
