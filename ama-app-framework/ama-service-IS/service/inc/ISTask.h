/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ISTask.h
/// @brief contains class ISTask
///
/// Created by wangqi on 2016/11/21.
/// this file contains the definination of class ISTask
///

#ifndef ISTASK_H_
#define ISTASK_H_

#include "Task.h"
#include "ama_audioTypes.h"
#include "TaskDispatcher.h"

using std::string;

enum ISTaskType{
    REQ_INFORMING_SOUND = 0x0,
    GET_AUDIO_FOCUS,
    LOSS_AUDIO_FOCUS,
    REQ_FEEDBACKTONE_SOUND,
    REQ_PLAYRADAR_SOUND,
    REQ_AVMALERT_SOUND,
};

/// @class ISTask
/// @brief this class is the abstraction of InformingSound task
///
/// this class define a task for the ISController

class ISTask:
            public Task
{
public:
    ISTask(){}
    ~ISTask(){}
    void setConnectID(ama_connectID_t connectID){
        mConnectID = connectID;
    }
    ama_connectID_t getConnectID(void){
        return mConnectID;
    }

    void setDeviceID(ama_audioDeviceType_t deviceID){
        mAudioDeviceID = deviceID;
    }
    ama_audioDeviceType_t getDeviceID(void){
        return mAudioDeviceID;
    }

    void setVolumeType(int volumeType){
        mVolumeType = volumeType;
    }

    int getVolumeType(void){
        return mVolumeType;
    }

    void setFeedbackVol(int feedbackVol){
        mFeedbackVol = feedbackVol;
    }
    int getFeedbackVol(void){
        return mFeedbackVol;
    }

    void setRadarSoundType(ama_radarSoundType_t soundType){
        mCurType = soundType;
    }
    ama_radarSoundType_t getRadarSoundType(void){
        return mCurType;
    }

    void setRadarChannel(ama_radarChannel_t radarChannel){
        mCurChannel = radarChannel;
    }
    ama_radarChannel_t getRadarChannel(void){
        return mCurChannel;
    }

    void setIsPlay(bool isPlay){
        mIsPlay = isPlay;
    }
    bool getIsPlay(){
        return mIsPlay;
    }

    virtual string getTaskTypeString()
	{
		switch(GetType()){
			case REQ_INFORMING_SOUND:    return "REQ_INFORMING_SOUND";
			case GET_AUDIO_FOCUS:    return "GET_AUDIO_FOCUS";
            case LOSS_AUDIO_FOCUS:    return "LOSS_AUDIO_FOCUS";
            case REQ_FEEDBACKTONE_SOUND: return "REQ_FEEDBACKTONE_SOUND";
            case REQ_PLAYRADAR_SOUND: return "REQ_PLAYRADAR_SOUND";
            case REQ_AVMALERT_SOUND: return "REQ_AVMALERT_SOUND";
			default: return "ERROR TASK TYPE";
		}
	}
private:
    ama_connectID_t mConnectID;
    ama_audioDeviceType_t mAudioDeviceID;
    ama_radarSoundType_t mCurType;
    ama_radarChannel_t mCurChannel;
    int mFeedbackVol;
    int mVolumeType;
    bool mIsPlay;
};


#endif //ISTASK_H_
