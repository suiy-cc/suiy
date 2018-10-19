/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioTask.h
/// @brief contains class AudioTask
///
/// Created by wangqi on 2016/6/16.
/// this file contains the definination of class AudioTask
///

#ifndef AUDIOTASK_H_
#define AUDIOTASK_H_

//sdk->AMACInterface
#include <AMACInterface.h>

#include "Task.h"
#include "AMGRLogContext.h"
#include "ama_audioTypes.h"
#include "ama_types.h"

using std::string;

enum AudioTaskType{
    E_AC_INIT_TASK  = 0x0,

    REQUEST_AUDIO_FOCUS,
    RELEASE_AUDIO_FOCUS,
    SET_STREAM_MUTE_STATE,

    AMSOURCE_CREATED,
    AMSOURCE_DESTROYED,
    AMSINK_CREATED,
    AMSINK_DESTROYED,

    CLEAR_APP_AUDIO,

    SET_GAINFORSPEED_BY_RPC,
    SET_MEDIAMUTE_BY_POWER,
    SET_AUDIO_DEVICE_MUTE,

    REQ_TO_USE_SPEAKER,
    ABN_TO_USE_SPEAKER,

    SET_BY_APPMANAGER,

    DUCK_AUDIO,
    UNDUCK_AUDIO,

    DEBUG
};

class AudioTask:
            public Task
{
public:
    AudioTask(){}
    ~AudioTask(){}
    void setConnectID(ama_connectID_t connectID){
        mConnectID = connectID;
    }
    ama_connectID_t getConnectID(void){
        return mConnectID;
    }

    void setSeatID(ama_seatID_t seatID){
        mSeatID = seatID;
    }
    ama_seatID_t getSeatID(void){
        return mSeatID;
    }

    void setStreamID(ama_streamID_t streamID){
        mStreamID = streamID;
    }
    ama_streamID_t getStreamID(void){
        return mStreamID;
    }

    void setMuteState(bool isMute){
        mIsMute = isMute;
    }
    bool getMuteState(void){
        return mIsMute;
    }

    void setDebugCmd(int debugCmd){
        mDebugCmd = debugCmd;
    }

    int getDebugCmd(void){
        return mDebugCmd;
    }

    void setAMSource(AMAC_SourceType_s AMSource){
        mAMSource = AMSource;
    }
    AMAC_SourceType_s getAMSource(void){
        return mAMSource;
    }

    void setAMSourceID(AMAC_SourceID_t AMSourceID){
        mAMSourceID = AMSourceID;
    }
    AMAC_SourceID_t getAMSourceID(void){
        return mAMSourceID;
    }

    void setAMSink(AMAC_SinkType_s AMSink){
        mAMSink = AMSink;
    }
    AMAC_SinkType_s getAMSink(void){
        return mAMSink;
    }

    void setAMSinkID(AMAC_SinkID_t AMSinkID){
        mAMSinkID = AMSinkID;
    }
    AMAC_SinkID_t getAMSinkID(void){
        return mAMSinkID;
    }

    void setAMPID(ama_AMPID_t AMPID){
        mAMPID = AMPID;
    }
    ama_AMPID_t getAMPID(){
        return mAMPID;
    }

    void setPowerLimit(int powerLimit){
        mPowerLimit = powerLimit;
    }
    ama_AMPID_t getPowerLimit(){
        return mPowerLimit;
    }

    void setGainForSpeed(int gainForSpeed){
        mGainForSpeed = gainForSpeed;
    }
    ama_AMPID_t getGainForSpeed(){
        return mGainForSpeed;
    }

    void setKey(int key){
        mKey = key;
    }
    int getKey(void){
        return mKey;
    }

    void setValue(int value){
        mValue = value;
    }
    int getValue(void){
        return mValue;
    }

    void setDuckDurations(int duckDurations){
        mDuckDurationsms = duckDurations;
    }
    int getDuckDurations(void){
        return mDuckDurationsms;
    }

    void setDuckVolume(int duckVolume){
        mDuckVolume = duckVolume;
    }
    int getDuckVolume(void){
        return mDuckVolume;
    }

    virtual string getTaskTypeString()
	{
		switch(GetType()){
            case E_AC_INIT_TASK:    return "E_AC_INIT_TASK";
			case REQUEST_AUDIO_FOCUS:    return "REQUEST_AUDIO_FOCUS";
			case RELEASE_AUDIO_FOCUS:    return "RELEASE_AUDIO_FOCUS";
            case SET_STREAM_MUTE_STATE:  return "SET_STREAM_MUTE_STATE";
            case AMSOURCE_CREATED:  return "AMSOURCE_CREATED";
            case AMSOURCE_DESTROYED:  return "AMSOURCE_DESTROYED";
            case AMSINK_CREATED:  return "AMSINK_CREATED";
            case AMSINK_DESTROYED:  return "AMSINK_DESTROYED";
            case CLEAR_APP_AUDIO:  return "CLEAR_APP_AUDIO";
            case REQ_TO_USE_SPEAKER:  return "REQ_TO_USE_SPEAKER";
            case ABN_TO_USE_SPEAKER:  return "ABN_TO_USE_SPEAKER";
            case SET_GAINFORSPEED_BY_RPC:  return "SET_GAINFORSPEED_BY_RPC";
            case SET_MEDIAMUTE_BY_POWER:  return "SET_MEDIAMUTE_BY_POWER";
            case SET_AUDIO_DEVICE_MUTE:  return "SET_AUDIO_DEVICE_MUTE";
            case SET_BY_APPMANAGER:  return "SET_BY_APPMANAGER";
            case DUCK_AUDIO:  return "DUCK_AUDIO";
            case UNDUCK_AUDIO:  return "UNDUCK_AUDIO";
            case DEBUG:             return "DEBUG";
			default: return "ERROR TASK TYPE";
		}
	}
private:
    ama_connectID_t mConnectID;
    ama_streamID_t mStreamID;
    ama_seatID_t mSeatID;
    ama_AMPID_t mAMPID;
    bool mIsMute;

    AMAC_SourceType_s mAMSource;
    AMAC_SourceID_t mAMSourceID;
    AMAC_SinkType_s mAMSink;
    AMAC_SinkID_t mAMSinkID;

    int mPowerLimit;
    int mGainForSpeed;

    int mKey;
    int mValue;

    int mDebugCmd;

    int mDuckDurationsms;
    int mDuckVolume;

    int mFeedbackVol;
};

void notifyPowerStaChangedToAC(ama_seatID_t seatID,bool isPowerOn);
void setMediaMuteByPowerStaToAC(bool isMediaMute);
void sendInitTaskToAC(void);
void sendSetDeviceMuteTaskToAC(ama_seatID_t seatID,bool isMute);

#endif //AUDIOTASK_H_
