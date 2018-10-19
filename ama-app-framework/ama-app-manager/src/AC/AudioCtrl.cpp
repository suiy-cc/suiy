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

#include <assert.h>

#include "AudioCtrl.h"
#include "AudioTask.h"
#include "ama_audioTypes.h"
#include "ama_types.h"
#include "AudioSetting.h"
#include "TaskDispatcher.h"
#include "Speaker.h"
#include "wd.h"
#include "ConCtrl.h"
#include "Amplifier.h"

AudioController::AudioController()
:mApplyStreamID(0)
{
    logVerbose(AC,"AudioController::AudioController()-->IN");

    logVerbose(AC,"AudioController::AudioController()-->OUT");
}

AudioController::~AudioController()
{
    logVerbose(AC,"AudioController::~AudioController()-->IN");
    logVerbose(AC,"AudioController::~AudioController()-->OUT");
}

// #ifdef CODE_FOR_HS5
// void AudioController::doTask(shared_ptr<Task> task)
// {
//     logVerbose(AC,"AudioController::doTask()-->IN");
//     assert(task.get());
//     auto pTask = dynamic_pointer_cast<AudioTask>(task);
//
//     // feed the dog
//     if (pTask->IsWatchDotTask()) {
//       wdSetState(E_TASK_ID_AC, WATCHDOG_SUCCESS);
//       return ;
//     }
//
//     logDebug(AC,"do nothing for HS5 in AudioController!");
//
//     logVerbose(AC,"AudioController::doTask()-->OUT");
// }
//
// #else//CODE_FOR_HS5

void AudioController::doTask(shared_ptr<Task> task)
{
    logVerbose(AC,"AudioController::doTask()-->IN");
    assert(task.get());
    auto pTask = dynamic_pointer_cast<AudioTask>(task);

    // feed the dog
    if (pTask->IsWatchDotTask()) {
      wdSetState(E_TASK_ID_AC, WATCHDOG_SUCCESS);
      return ;
    }

    int taskID = pTask->GetID();
    logInfo(AC,"doTask==>""taskType=",pTask->getTaskTypeString());
    shared_ptr<Stream> pStream;
    int streamStep;
    bool isMute;
    switch(pTask->GetType()){
    case E_AC_INIT_TASK:
        audioControllerInit();
        break;
    case REQUEST_AUDIO_FOCUS:
        logInfo(AC," connectID=", ToHEX(pTask->getConnectID()));
        logInfoF(ADBG,"connectID[0x%x]-->>requestAudioFocus()",pTask->getConnectID());
        recordApplyStreamID(GET_STREAMID_FROM_CONNECTID(pTask->getConnectID()));
        handleRequestAudioFocus(pTask->getConnectID());
        break;
    case RELEASE_AUDIO_FOCUS:
        logInfo(AC," connectID=", ToHEX(pTask->getConnectID()));
        logInfoF(ADBG,"connectID[0x%x]-->>abandonAudioFocus()",pTask->getConnectID());
        recordApplyStreamID(GET_STREAMID_FROM_CONNECTID(pTask->getConnectID()));
        handleAbandonAudioFocus(pTask->getConnectID());
        break;
    case SET_STREAM_MUTE_STATE:
        logInfo(AC," streamID=", ToHEX(pTask->getStreamID()),"isMute=", pTask->getMuteState());
        pStream = findStreamFromMapByStreamID(pTask->getStreamID());
        if(pStream){
            pStream->setStreamMuteState(pTask->getMuteState());
        }else{
            logWarn(AC,"do nothing because can't find stream in Vector");
        }
        break;
    case AMSOURCE_CREATED:
        logInfoF(ADBG,"audioSource[%s] is created",pTask->getAMSource().name);
        handleAMSourceCreated(pTask->getAMSource());
        break;
    case AMSOURCE_DESTROYED:
        logInfo(ADBG,"audioSource[",AMClient::GetInstance()->getSourceNameByID(pTask->getAMSourceID()),"] is destroyed");
        handleAMSourceDestroyed(pTask->getAMSourceID());
        break;
    case AMSINK_CREATED:
        handleAMSinkCreated(pTask->getAMSink());
        break;
    case AMSINK_DESTROYED:
        handleAMSinkDestroyed(pTask->getAMSinkID());
        break;
    case CLEAR_APP_AUDIO:
        delAppStreamAndCon(pTask->getAMPID());
        break;
    case SET_GAINFORSPEED_BY_RPC:
        AudioSetting::GetInstance()->setGainForSpeedByRPC(pTask->getGainForSpeed());
        break;
    case REQ_TO_USE_SPEAKER:
        Speaker::GetInstance()->setWhichSeatUsingSpeaker(pTask->getSeatID());
        break;
    case ABN_TO_USE_SPEAKER:
        Speaker::GetInstance()->abandonToUseSpeaker(pTask->getSeatID());
        break;
    case SET_BY_APPMANAGER:
        handleSetReq(pTask->getKey(),pTask->getValue());
        break;
    case SET_MEDIAMUTE_BY_POWER:
        AudioSetting::GetInstance()->setMediaMuteByPowerSta(pTask->getMuteState());
        break;
    case SET_AUDIO_DEVICE_MUTE:
        isMute = pTask->getMuteState();
        if(isMute){
            //we need set AMP Mute first!
            if(E_SEAT_IVI==pTask->getSeatID()){
                //we only set AMP mute when audioDevice==Speaker
                Amplifier::GetInstance()->setAMPMuteState(isMute);
            }
            ConnectController::GetInstance()->setAudioDeviceMuteState(pTask->getSeatID(),isMute);
        }else{
            ConnectController::GetInstance()->setAudioDeviceMuteState(pTask->getSeatID(),isMute);
            if(E_SEAT_IVI==pTask->getSeatID()){
                //we only set AMP mute when audioDevice==Speaker
                Amplifier::GetInstance()->setAMPMuteState(isMute);
            }
        }
        break;
    case DUCK_AUDIO:
        logInfo(AC," durations_ms=", pTask->getDuckDurations());
        logInfo(AC," duckVolume=", pTask->getDuckVolume());
        handleDuckReq(pTask->getDuckDurations(),pTask->getDuckVolume());
        break;
    case UNDUCK_AUDIO:
        logInfo(AC," durations_ms=", pTask->getDuckDurations());
        handleUnduckReq(pTask->getDuckDurations());
        break;
    case DEBUG:
        logInfo(AC," debugCmd=",pTask->getDebugCmd());
        handleDebugCmd(pTask->getDebugCmd());
        break;
    default: logError(AC,"Error task type for AudioController");
        break;
    };
    logVerbose(AC,"AudioController::doTask()-->OUT");
}

// #endif//CODE_FOR_HS5

void AudioController::audioControllerInit()
{
    AMClient::GetInstance()->registerCommandAndControllerClient();
    Amplifier::GetInstance()->initDefaultMode();
}

void AudioController::earlyInit(void)
{
    // AMClient::GetInstance()->registerCommandAndControllerClient();
}

void AudioController::handleRequestAudioFocus(ama_connectID_t connectID)
{
    logVerbose(AC,"AudioController::handleRequestAudioFocus-->>IN connectID=", ToHEX(connectID));

    auto pStream = findStreamFromMapByStreamID(GET_STREAMID_FROM_CONNECTID(connectID));
    if(!pStream){
        pStream = createStreamByStreamID(GET_STREAMID_FROM_CONNECTID(connectID));
    }
    pStream->requestAudioDeviceFocus(connectID);

    logVerbose(AC,"AudioController::handleRequestAudioFocus-->>OUT");
}

void AudioController::handleAbandonAudioFocus(ama_connectID_t connectID)
{
    auto pStream = findStreamFromMapByStreamID(GET_STREAMID_FROM_CONNECTID(connectID));
    if(pStream){
        pStream->releaseAudioDeviceFocus(connectID);
    }else{
        logWarn(AC,"do nothing because can't find stream in Vector");
    }
}

shared_ptr<Stream> AudioController::findStreamFromMapByStreamID(ama_streamID_t streamID)
{
    logVerbose(AC,"AudioController::findStreamFromMapByStreamID-->>IN streamID=", ToHEX(streamID));

    std::map<ama_streamID_t,shared_ptr<Stream>>::iterator it;
    it = mAllStreamMap.find(streamID);
    if(it != mAllStreamMap.end()){
        logDebug(AC,"find stream in allStreamMap");
        logVerbose(AC,"AudioController::findStreamFromMapByStreamID-->>OUT");
        return mAllStreamMap[streamID];
    }else{
        logDebug(AC,"can't find stream in allStreamMap, return NULL");
        logVerbose(AC,"AudioController::findStreamFromMapByStreamID-->>OUT");
        return NULL;
    }
}

shared_ptr<Stream> AudioController::createStreamByStreamID(ama_streamID_t streamID)
{
    logVerbose(AC,"AudioController::createStreamByStreamID-->>IN streamID=", ToHEX(streamID));

    shared_ptr<Stream> pStream = std::make_shared<Stream>(streamID);
    mAllStreamMap[streamID] = pStream;
    logVerbose(AC,"AudioController::createStreamByStreamID-->>OUT");
    return pStream;
}

shared_ptr<Connection> AudioController::findConnectionFromMapByConnectID(ama_connectID_t connectID)
{
    logVerbose(AC,"AudioController::findConnectionFromMapByConnectID-->>IN connectID=", ToHEX(connectID));

    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    it = mAllConnectionMap.find(connectID);
    if(it != mAllConnectionMap.end()){
        logDebug(AC,"find connection in mAllConnectionMap");
        logVerbose(AC,"AudioController::findConnectionFromMapByConnectID-->>OUT");
        return mAllConnectionMap[connectID];
    }else{
        logDebug(AC,"can't find connection in mAllConnectionMap, return NULL");
        return NULL;
    }
}

shared_ptr<Connection> AudioController::createConnectionIntoMap(ama_connectID_t connectID)
{
    logVerbose(AC,"AudioController::createConnectionIntoMap-->>IN connectID=", ToHEX(connectID));
    shared_ptr<Connection> pConnection = std::make_shared<Connection>(connectID);
    mAllConnectionMap[connectID] = pConnection;
    logVerbose(AC,"AudioController::createConnectionIntoMap-->>OUT");
    return pConnection;
}

void AudioController::delConnectionFromMap(ama_connectID_t connectID)
{
    logVerbose(AC,"AudioController::delConnectionFromMap-->>IN connectID=", ToHEX(connectID));

    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    it = mAllConnectionMap.find(connectID);
    if(it != mAllConnectionMap.end()){
        logDebug(AC,"find connection in mAllConnectionMap");
        mAllConnectionMap.erase(it);
    }else{
        logError(AC,"can't del connection,because there is't connection in map");
    }

    logVerbose(AC,"AudioController::delConnectionFromMap-->>OUT");
}

void AudioController::delAppStreamAndCon(ama_AMPID_t AMPID)
{
    logVerbose(AC,"AudioController::delConnectionFromMap-->>IN AMPID=", ToHEX(AMPID));

    //delete connection from mAllConnectionMap
    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator itCon;
    for(itCon=mAllConnectionMap.begin(); itCon!=mAllConnectionMap.end();){
        if(AMPID==GET_AMPID_FROM_CONNECTID((itCon->first))){
            itCon = mAllConnectionMap.erase(itCon);
        }else{
             ++itCon;
        }
    }
    //delete stream form mAllStreamMap
    std::map<ama_streamID_t,shared_ptr<Stream>>::iterator itStream;
    for(itStream=mAllStreamMap.begin(); itStream!=mAllStreamMap.end();){
        if(AMPID==GET_AMPID_FROM_STREAMID((itStream->first))){
            itStream = mAllStreamMap.erase(itStream);
        }else{
            ++itStream;
        }
    }

    logVerbose(AC,"AudioController::delConnectionFromMap-->>OUT");
}

void AudioController::handleAMSourceCreated(AMAC_SourceType_s AMSource)
{
    logVerbose(AC,"AudioController::handleAMSourceCreated()-->IN");

    string sourceName = AMSource.name;
    AMClient::GetInstance()->addAMSourceIntoMap(sourceName, AMSource.sourceID);

    string sourceNameCopy = sourceName;
    if(sourceNameCopy.substr(0,2) == "0x"){
        logDebug(AC,"sourceName start with 0x");
        int streamID = std::stol(sourceName,nullptr,0);
        logDebug(AC,"streamID = ", ToHEX(streamID));
        auto pStream = findStreamFromMapByStreamID(streamID);
        if(pStream){
            pStream->execConnectWhenSourceCreated();
        }
    }else if(sourceName=="radarStream"){
        logDebug(AC,"radarStream created");
        findAllRadarStreamAndexecConnectWhenSourceCreated();
    }else if("bt_audio_stream"==sourceName){
        logDebug(AC,"sourceName==bt_audio_stream");
        execConnectWhenSourceCreatedByStreamType(E_STREAM_BT_AUDIO);
    }else if("bt_phone_stream"==sourceName){
        logDebug(AC,"sourceName==bt_phone_stream");
        execConnectWhenSourceCreatedByStreamType(E_STREAM_BT_CALL);
    }else if("carplayAudio"==sourceName){
        logDebug(AC,"sourceName==carplayAudio");
        execConnectWhenSourceCreatedByStreamType(E_STREAM_CARPLAY_AUDIO);
    }else if("carplayAlt"==sourceName){
        logDebug(AC,"sourceName==carplayAlt");
        execConnectWhenSourceCreatedByStreamType(E_STREAM_CARPLAY_ALT);
    }else if("carplaySiri"==sourceName){
        logDebug(AC,"sourceName==carplaySiri");
        execConnectWhenSourceCreatedByStreamType(E_STREAM_CARPLAY_SIRI);
    }else if("carplayCall"==sourceName){
        logDebug(AC,"sourceName==carplayCall");
        execConnectWhenSourceCreatedByStreamType(E_STREAM_CARPLAY_CALL);
    }
    // else if("splash-player"==sourceName){
    //     logDebug(AC,"sourceName==player");
    //     FocusController::GetInstance()->requestAudioDeviceFocus(0x10ff6001);
    // }

    logVerbose(AC,"AudioController::handleAMSourceCreated()-->OUT");
}

void AudioController::execConnectWhenSourceCreatedByStreamType(ama_streamType_t streamType)
{
    logVerbose(AC,"AudioController::execConnectWhenSourceCreatedByStreamType()-->IN streamType:",ToHEX(streamType));

    std::map<ama_streamID_t,shared_ptr<Stream>>::iterator it;
    for(it=mAllStreamMap.begin(); it!=mAllStreamMap.end(); ++it){
        if(streamType==GET_STREAMTYPE_FROM_STREAMID((it->first))){
            (it->second)->execConnectWhenSourceCreated();
        }
    }

    logVerbose(AC,"AudioController::execConnectWhenSourceCreatedByStreamType()-->OUT");
}

void AudioController::execDisconnectWhenSourceDestroyByStreamType(ama_streamType_t streamType)
{
    logVerbose(AC,"AudioController::execDisconnectWhenSourceDestroyByStreamType()-->IN streamType:",ToHEX(streamType));

    std::map<ama_streamID_t,shared_ptr<Stream>>::iterator it;
    for(it=mAllStreamMap.begin(); it!=mAllStreamMap.end(); ++it){
        if(streamType==GET_STREAMTYPE_FROM_STREAMID((it->first))){
            (it->second)->execDisconnectWhenSourceRemoved();
        }
    }

    logVerbose(AC,"AudioController::execDisconnectWhenSourceDestroyByStreamType()-->OUT");
}

void AudioController::findAllRadarStreamAndexecConnectWhenSourceCreated(void)
{
    logVerbose(AC,"AudioController::findAllRadarStreamAndexecConnectWhenSourceCreated()-->IN");

    ama_streamType_t streamType;

    std::map<ama_streamID_t,shared_ptr<Stream>>::iterator it;
    for(it=mAllStreamMap.begin(); it!=mAllStreamMap.end(); ++it){
        streamType = GET_STREAMTYPE_FROM_STREAMID(it->first);
        if(streamType<=E_STREAM_RR_RADAR){
            (it->second)->execConnectWhenSourceCreated();
        }
    }

    logVerbose(AC,"AudioController::findAllRadarStreamAndexecConnectWhenSourceCreated()-->OUT");
}

void AudioController::findAllRadarStreamAndexecDisconnectWhenSourceDestroy(void)
{
    logVerbose(AC,"AudioController::findAllRadarStreamAndexecDisconnectWhenSourceDestroy()-->IN");

    ama_streamType_t streamType;

    std::map<ama_streamID_t,shared_ptr<Stream>>::iterator it;
    for(it=mAllStreamMap.begin(); it!=mAllStreamMap.end(); ++it){
        streamType = GET_STREAMTYPE_FROM_STREAMID(it->first);
        if(streamType<=E_STREAM_RR_RADAR){
            (it->second)->execDisconnectWhenSourceRemoved();
        }
    }

    logVerbose(AC,"AudioController::findAllRadarStreamAndexecDisconnectWhenSourceDestroy()-->OUT");
}

void AudioController::handleAMSourceDestroyed(AMAC_SourceID_t AMSourceID)
{
    logVerbose(AC,"AudioController::handleAMSourceDestroyed()-->IN");

    string sourceName = AMClient::GetInstance()->getSourceNameByID(AMSourceID);

    if(sourceName.substr(0,2) == "0x"){
        logDebug(AC,"sourceName start with 0x");
        int streamID = std::stol(sourceName.c_str(), 0,0);
        logDebug(AC,"streamID = ", ToHEX(streamID));
        auto pStream = findStreamFromMapByStreamID(streamID);
        if(pStream){
            pStream->execDisconnectWhenSourceRemoved();
        }
    }else if(sourceName=="radarStream"){
        logDebug(AC,"radarStream created");
        findAllRadarStreamAndexecDisconnectWhenSourceDestroy();
    }else if("bt_audio_stream"==sourceName){
        logDebug(AC,"sinkName==bt_audio_stream");
        execDisconnectWhenSourceDestroyByStreamType(E_STREAM_BT_AUDIO);
    }else if("bt_phone_stream"==sourceName){
        logDebug(AC,"sinkName==bt_phone_stream");
        execDisconnectWhenSourceDestroyByStreamType(E_STREAM_BT_CALL);
    }else if("carplayAudio"==sourceName){
        logDebug(AC,"sinkName==carplayAudio");
        execDisconnectWhenSourceDestroyByStreamType(E_STREAM_CARPLAY_AUDIO);
    }else if("carplayAlt"==sourceName){
        logDebug(AC,"sinkName==carplayAlt");
        execDisconnectWhenSourceDestroyByStreamType(E_STREAM_CARPLAY_ALT);
    }else if("carplaySiri"==sourceName){
        logDebug(AC,"sinkName==carplaySiri");
        execDisconnectWhenSourceDestroyByStreamType(E_STREAM_CARPLAY_SIRI);
    }else if("carplayCall"==sourceName){
        logDebug(AC,"sinkName==carplayCall");
        execDisconnectWhenSourceDestroyByStreamType(E_STREAM_CARPLAY_CALL);
    }
    // else if("splash-player"==sourceName){
    //     logDebug(AC,"sourceName==player");
    //     FocusController::GetInstance()->releaseAudioDeviceFocus(0x10ff6001);
    // }

    AMClient::GetInstance()->delAMSourceFromMap(AMSourceID);

    logVerbose(AC,"AudioController::handleAMSourceDestroyed()-->OUT");
}

void AudioController::handleAMSinkCreated(AMAC_SinkType_s AMSink)
{
    logVerbose(AC,"AudioController::handleAMSinkCreated()-->IN");

    string sinkName = AMSink.name;
    AMClient::GetInstance()->addAMSinkIntoMap(sinkName, AMSink.sinkID);

    string sourceNameCopy = sinkName;
    if(sourceNameCopy.substr(0,2) == "0x"){
        logDebug(AC,"mic sourceName start with 0x");
        int streamID = std::stol(sinkName.c_str(), 0,0);
        logDebug(AC,"mic streamID = ", ToHEX(streamID));
        auto pStream = findStreamFromMapByStreamID(streamID);
        if(pStream){
            pStream->execConnectWhenSourceCreated();
        }
    }

    logVerbose(AC,"AudioController::handleAMSinkCreated()-->OUT");
}

void AudioController::handleAMSinkDestroyed(AMAC_SinkID_t AMSinkID)
{
    logVerbose(AC,"AudioController::handleAMSinkDestroyed()-->IN");

    AMClient::GetInstance()->delAMSinkFromMap(AMSinkID);

    logVerbose(AC,"AudioController::handleAMSinkDestroyed()-->OUT");
}

void AudioController::handleDebugCmd(int debugCmd)
{
    logVerbose(AC,"AudioController::handleDebugCmd()-->IN");

    switch(debugCmd){
    case E_SHOW_CONNECTIONS:
            debugShowConnections();
            break;
    case E_SHOW_AMSOURCES:
            AMClient::GetInstance()->debugShowAMSources();
            break;
    case E_SHOW_AMSINKS:
            AMClient::GetInstance()->debugShowAMSinks();
            break;
    case E_SHOW_CONRECORD:
            AMClient::GetInstance()->debugShowConRecord();
            break;
    case E_SHOW_ROUTINGMAP:
            AMClient::GetInstance()->debugShowRoutingMap();
            break;
    case E_SHOW_FOCUSSTA:
            FocusController::GetInstance()->debugShowFocusSta();
            break;
    case E_SHOW_AUDIOSETTING:
            AudioSetting::GetInstance()->debugShowAudioSetting();
            break;
    case E_SET_SEAT_IVI_USE_SPEAKER:
            Speaker::GetInstance()->setWhichSeatUsingSpeaker(E_SEAT_IVI);
            break;
    case E_SET_SEAT_LRSE_USE_SPEAKER:
            Speaker::GetInstance()->setWhichSeatUsingSpeaker(E_SEAT_RSE1);
            break;
    case E_SET_SEAT_RRSE_USE_SPEAKER:
            Speaker::GetInstance()->setWhichSeatUsingSpeaker(E_SEAT_RSE2);
            break;
    default:
            logError(AC,"it's not a AC debug command!");
    }

    logVerbose(AC,"AudioController::handleDebugCmd()-->OUT");
}

void AudioController::handleSetReq(int key,int value)
{
    logVerbose(AC,"AudioController::handleSetReq()-->IN");

    AudioSetting::GetInstance()->handleAudioSetting(key,value);

    logVerbose(AC,"AudioController::handleSetReq()-->OUT");
}

void AudioController::handleDuckReq(int durations_ms,int duckVolume)
{
    logVerbose(AC,"AudioController::handleDuckReq()-->IN");

    logDebug(AC,"set all connection duck");
    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator itCon;
    for(itCon=mAllConnectionMap.begin(); itCon!=mAllConnectionMap.end();itCon++){
        (itCon->second)->duckAudio(durations_ms,duckVolume);
    }

    logVerbose(AC,"AudioController::handleDuckReq()-->OUT");
}

void AudioController::handleUnduckReq(int durations_ms)
{
    logVerbose(AC,"AudioController::handleUnduckReq()-->IN");

    logDebug(AC,"set all connection unduck");
    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator itCon;
    for(itCon=mAllConnectionMap.begin(); itCon!=mAllConnectionMap.end();itCon++){
        (itCon->second)->unduckAudio(durations_ms);
    }

    logVerbose(AC,"AudioController::handleUnduckReq()-->OUT");
}

void AudioController::debugShowConnections(void)
{
    logVerbose(DBG,"AudioController::debugShowConnections()-->IN");

    int index = 0;
    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    for(it=mAllConnectionMap.begin();it!=mAllConnectionMap.end();++it,++index){
        logDebug(DBG,"connection index:",index);
        logDebug(DBG,"connectionID:", ToHEX(it->first));
        logDebug(DBG,"connection focusState:", ToHEX((it->second)->focusSta()));
        // logDebug(DBG,"connection isConnect:",(it->second)->isConnect());
        // logDebug(DBG,"connection currentVolGain:",(it->second)->currentVolGain());
        logDebug(DBG,"connection isStreamMute:",(it->second)->isStreamMute());
    }

    logVerbose(DBG,"AudioController::debugShowConnections()-->OUT");
}

void notifyPowerStaChangedToAC(ama_seatID_t seatID,bool isPowerOn)
{
    logVerbose(AC,"AudioController->notifyPowerStaChangedToAC()-->IN isPowerOn:",isPowerOn);

    //make a RVC mode connectID
    ama_streamID_t streamID = CREATE_STREAMID_BY_SEAT_APP_STREAMTYPE(seatID,0x00,E_STREAM_POWER_OFF);
    ama_connectID_t connectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID,seatID);

    auto pTask = std::make_shared<AudioTask>();
    if(!isPowerOn){
        pTask->SetType(REQUEST_AUDIO_FOCUS);
    }else{
        pTask->SetType(RELEASE_AUDIO_FOCUS);
    }
	pTask->setConnectID(connectID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AC,"AudioController->notifyPowerStaChangedToAC()-->OUT");
}

void setMediaMuteByPowerStaToAC(bool isMediaMute)
{
    logVerbose(AC,"AudioController->setMediaMuteByPowerStaToAC()-->IN isMediaMute:",isMediaMute);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(SET_MEDIAMUTE_BY_POWER);
	pTask->setMuteState(isMediaMute);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AC,"AudioController->setMediaMuteByPowerStaToAC()-->OUT");
}

void sendInitTaskToAC(void)
{
    DomainVerboseLog chatter(AC, __STR_FUNCTIONP__);
    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(E_AC_INIT_TASK);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

void sendSetDeviceMuteTaskToAC(ama_seatID_t seatID,bool isMute)
{
    DomainVerboseLog chatter(AC, __STR_FUNCTIONP__);
    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(SET_AUDIO_DEVICE_MUTE);
    pTask->setSeatID(seatID);
    pTask->setMuteState(isMute);
    SendQueryTaskNWait(pTask);
}
