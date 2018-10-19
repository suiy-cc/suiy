/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ISCtrl.cpp
/// @brief contains the implementation of class ISController
///
/// Created by wangqi on 2016/11/21.
/// this file contains the implementation of class ISController
///

#include <assert.h>

#include "ISCtrl.h"
#include "ISTask.h"
#include "ama_audioTypes.h"
#include "ama_types.h"

#include "TaskDispatcher.h"
#include "SoundDef.h"
#include "logDef.h"
#include "ISAudioStream.h"
#include "AppSDK.h"

ISController::ISController()
{
    logVerbose(IS,"ISController::ISController()-->IN");

    createAllPlaySoundThread();

    logVerbose(IS,"ISController::ISController()-->OUT");
}

ISController::~ISController()
{
    logVerbose(IS,"ISController::~ISController()-->IN");

    stopAllPlaySoundThread();

    logVerbose(IS,"ISController::~ISController()-->OUT");
}

ama_connectID_t ISController::setAmpidForConnectID(ama_connectID_t connectID)
{
    ama_AMPID_t ampid = AMPID_APP(GET_DEVICETYPE_FROM_CONNECTID(connectID),IS_APPID);
    ama_streamID_t streamID = CREATE_STREAMID_BY_AMPID_STREAMTYPE(ampid,GET_STREAMTYPE_FROM_CONNECTID(connectID));
    ama_connectID_t rightConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(
        streamID,GET_DEVICETYPE_FROM_CONNECTID(connectID)
    );
    return rightConnectID;
}

void ISController::doTask(shared_ptr<Task> task)
{
    logVerbose(IS,"ISController::doTask()-->IN");
    shared_ptr<PlaySoundThread> pPlaySoundThread;
    ama_connectID_t connectID;
    assert(task.get());
    auto pTask = dynamic_pointer_cast<ISTask>(task);

    int taskID = pTask->GetID();
    logInfo(IS,"doTask==>""taskType=",pTask->getTaskTypeString());
    switch(pTask->GetType()){
    case REQ_INFORMING_SOUND:
        connectID = pTask->getConnectID();
        connectID = setAmpidForConnectID(connectID);
        logInfo(IS," connectID=", ToHEX(connectID));
        pPlaySoundThread = findPlaySoundThreadFromMapByConnectID(connectID);
        if(pPlaySoundThread){
            pPlaySoundThread->requestToPlay();
        }else{
            logError(IS,"can't find pPlaySoundThread in mAllPlaySoundThreadMap!");
        }
        break;
    case REQ_AVMALERT_SOUND:
        connectID = pTask->getConnectID();
        connectID = setAmpidForConnectID(connectID);
        logInfo(IS," connectID=", ToHEX(connectID));
        pPlaySoundThread = findPlaySoundThreadFromMapByConnectID(connectID);
        if(pPlaySoundThread){
            if(pTask->getIsPlay()){
                pPlaySoundThread->requestToPlayAlways();
            }else{
                pPlaySoundThread->requestToStopPlay();
            }
        }else{
            logError(IS,"can't find pPlaySoundThread in mAllPlaySoundThreadMap!");
        }
        break;
    case GET_AUDIO_FOCUS:
        connectID = pTask->getConnectID();
        connectID = setAmpidForConnectID(connectID);
        logInfo(IS," connectID=", ToHEX(connectID));
        OnGainAudioFocus(connectID);
        break;
    case LOSS_AUDIO_FOCUS:
        connectID = pTask->getConnectID();
        connectID = setAmpidForConnectID(connectID);
        logInfo(IS," connectID=", ToHEX(connectID));
        OnLossAudioFocus(connectID);
        break;
    case REQ_FEEDBACKTONE_SOUND:
        reqFeedbackSound(pTask->getVolumeType(),pTask->getFeedbackVol());
        break;
    case REQ_PLAYRADAR_SOUND:
        logInfo(IS," radarSoundType=", ToHEX(pTask->getRadarSoundType()));
        logInfo(IS," radarChannel=", ToHEX(pTask->getRadarChannel()));
        mpRadarSoundThread->playRadarSound(pTask->getRadarSoundType(),pTask->getRadarChannel());
        if(E_NORMAL_TONE==(pTask->getRadarSoundType())){
            connectID = CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(0x0000,E_STREAM_NOTIFICATION,E_IVI_SPEAKER);
            connectID = setAmpidForConnectID(connectID);
            pPlaySoundThread = findPlaySoundThreadFromMapByConnectID(connectID);
            if(pPlaySoundThread){
                pPlaySoundThread->requestToPlay();
            }else{
                logError(IS,"can't find pPlaySoundThread in mAllPlaySoundThreadMap!");
            }
        }else if(E_RADAR_ALERT_TONE==(pTask->getRadarSoundType())){
            connectID = CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(0x0000,E_STREAM_RADAR_ALERT,E_IVI_SPEAKER);
            connectID = setAmpidForConnectID(connectID);
            pPlaySoundThread = findPlaySoundThreadFromMapByConnectID(connectID);
            if(pPlaySoundThread){
                pPlaySoundThread->requestToPlay();
            }else{
                logError(IS,"can't find pPlaySoundThread in mAllPlaySoundThreadMap!");
            }
        }
        break;
    default: logError(IS,"Error task type for ISController");
        break;
    };
    logVerbose(IS,"ISController::doTask()-->OUT");
}

void ISController::reqFeedbackSound(int volumeType,int feedbackVol)
{
    logVerbose(IS,"ISController::reqFeedbackSound()-->IN");

    if(!isVolumeTypeActive(volumeType)){
        ama_audioDeviceType_t audioDeviceType = getDeviceType(volumeType);
        ama_streamID_t streamID = CREATE_STREAMID_BY_SEAT_APP_STREAMTYPE(audioDeviceType,IS_APPID,E_STREAM_FEEDBACK_NOTI);
        ama_connectID_t connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,audioDeviceType);
        logInfo(IS,"create connectID:",connectID);
        shared_ptr<PlaySoundThread> pPlaySoundThread = findPlaySoundThreadFromMapByConnectID(connectID);
        if(pPlaySoundThread){
            //setFeedbackVolume here!
            setByAppManager(E_FeedbackVol,feedbackVol);
            pPlaySoundThread->requestToPlay();
        }else{
            logError(IS,"can't find pPlaySoundThread in mAllPlaySoundThreadMap!");
        }
    }else{
        logInfo(IS,"volumeType is active so we don't need play feedback sound");
    }

    logVerbose(IS,"ISController::reqFeedbackSound()-->OUT");
}

void ISController::OnGainAudioFocus(ama_connectID_t connectID)
{
    logVerbose(IS,"ISController::OnGainAudioFocus()-->IN connectID=", ToHEX(connectID));

    shared_ptr<PlaySoundThread> pPlaySoundThread = findPlaySoundThreadFromMapByConnectID(connectID);
    if(pPlaySoundThread){
        pPlaySoundThread->OnGainAudioFocus();
    }else{
        logError(IS,"can't find pPlaySoundThread in mAllPlaySoundThreadMap!");
    }

    logVerbose(IS,"ISController::OnGainAudioFocus()-->OUT");
}

void ISController::OnLossAudioFocus(ama_connectID_t connectID)
{
    logVerbose(IS,"ISController::OnLossAudioFocus()-->IN connectID=", ToHEX(connectID));

    logVerbose(IS,"ISController::OnLossAudioFocus()-->OUT");
}

void ISController::createAllPlaySoundThread(void)
{
    logVerbose(IS,"ISController::createAllPlaySoundThread()-->IN");

    shared_ptr<PlaySoundThread> pPlaySoundThread;
    shared_ptr<ISAudioStream> pAudioStream;

    //create ivi softKey pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_SOFTKEY,E_IVI_SPEAKER);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(SOFTKEY_PLAYFILE_PATH);

    //create LRSE softKey pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_SOFTKEY,E_RSE_L_HEADSET);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(SOFTKEY_PLAYFILE_PATH);

    //create RRSE softKey pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_SOFTKEY,E_RSE_R_HEADSET);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(SOFTKEY_PLAYFILE_PATH);

    //create ivi feedbackSound pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_FEEDBACK_NOTI,E_IVI_SPEAKER);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(FEEDBACK_PLAYFILE);

    //create LRSE feedbackSound pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_FEEDBACK_NOTI,E_RSE_L_HEADSET);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(FEEDBACK_PLAYFILE);

    //create RRSE feedbackSound pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_FEEDBACK_NOTI,E_RSE_R_HEADSET);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(FEEDBACK_PLAYFILE);

    //create E_STREAM_NOTIFICATION pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_NOTIFICATION,E_IVI_SPEAKER);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(INFORMINGSOUND_PLAYFILE);

    //create E_STREAM_BACKDOOR_NOTI pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_BACKDOOR_NOTI,E_IVI_SPEAKER);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(BACKDOORSOUND_PLAYFILE);

    //create E_STREAM_RADAR_ALERT pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_RADAR_ALERT,E_IVI_SPEAKER);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(RADAR_ALERT_PLAYFILE);

    //create E_STREAM_RADAR_ALERT pulseAudio stream
    pAudioStream = std::make_shared<ISAudioStream>(E_STREAM_AVM_ALERT,E_IVI_SPEAKER);
    pPlaySoundThread = std::make_shared<PlaySoundThread>(pAudioStream);
    mAllPlaySoundThreadMap[pAudioStream->getConnectID()] = pPlaySoundThread;
    pPlaySoundThread->start();
    pPlaySoundThread->setPlayFilePath(AVM_MOD_PLAYFILE);

    mpRadarSoundThread = std::make_shared<RadarSoundThread>();
    mpRadarSoundThread->start();

    logVerbose(IS,"ISController::createAllPlaySoundThread()-->OUT");
}

shared_ptr<PlaySoundThread> ISController::findPlaySoundThreadFromMapByConnectID(ama_connectID_t connectID)
{
    logVerbose(IS,"ISController::findPlaySoundThreadFromMapByConnectID()-->IN");

    std::map<ama_connectID_t,shared_ptr<PlaySoundThread>> ::iterator it;
    it = mAllPlaySoundThreadMap.find(connectID);
    if(it != mAllPlaySoundThreadMap.end()){
        logDebug(IS,"find stream in mAllPlaySoundThreadMap");
        logVerbose(IS,"ISController::findPlaySoundThreadFromMapByConnectID-->>OUT");
        return mAllPlaySoundThreadMap[connectID];
    }else{
        logError(IS,"can't find paStream in mAllPlaySoundThreadMap!");
        logVerbose(IS,"ISController::findPlaySoundThreadFromMapByConnectID-->>OUT");
        return NULL;
    }
}

void ISController::stopAllPlaySoundThread(void)
{
    logVerbose(IS,"ISController::stopAllPlaySoundThread()-->IN");

    std::map<ama_connectID_t,shared_ptr<PlaySoundThread>> ::iterator it;
    for(it=mAllPlaySoundThreadMap.begin();it!=mAllPlaySoundThreadMap.end(); ++it){
        it->second->stop();
    }

    mpRadarSoundThread->stop();

    logVerbose(IS,"ISController::stopAllPlaySoundThread()-->OUT");
}

ama_audioDeviceType_t ISController::getDeviceType(int volumeType)
{
    logVerbose(IS,"ISController::getDeviceType()-->IN");

    ama_audioDeviceType_t audioDeviceType = E_IVI_SPEAKER;

    switch(volumeType){
        case E_MediaVolMixWithNavi:
        case E_MeidaVolMixWithRadar:
        case E_NaviVolMixWithPhone:
        case E_FrontRadarVol:
        case E_RearRadarVol:
        case E_IVINotificationVol:
        case E_IVISoftKeyVol:
        case E_IVIMediaVol:
        case E_SMSTTSVol:
        case E_VRTTSVol:
        case E_NaviTTSVol:
        case E_PhoneVol:
        case E_ECallVol:
            audioDeviceType = E_IVI_SPEAKER;
            break;
        case E_LRseNotificationVol:
        case E_LRseSoftKeyVol:
        case E_LRseMediaVol:
            audioDeviceType = E_RSE_L_HEADSET;
            break;
        case E_RRseNotificationVol:
        case E_RRseSoftKeyVol:
        case E_RRseMediaVol:
            audioDeviceType = E_RSE_R_HEADSET;
            break;
        default:
            logError(IS,"we don't handle this volumeType here");
            break;
    }

    logVerbose(IS,"ISController::getDeviceType()-->OUT");
    return audioDeviceType;
}
