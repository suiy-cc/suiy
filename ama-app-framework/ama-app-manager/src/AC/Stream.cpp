/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Stream.cpp
/// @brief contains the implementation of class Stream
///
/// Created by wangqi on 2016/6/15.
/// this file contains the implementation of class Stream
///

#include "Stream.h"
#include "Capi.h"
#include "AudioCtrl.h"
#include "TaskDispatcher.h"
#include "ProcessInfo.h"

Stream::Stream(ama_streamID_t streamID)
{
    logVerbose(AC,"Stream::Stream()-->IN streamID=", ToHEX(streamID));
    mStreamID = streamID;
    logVerbose(AC,"Stream::Stream()-->OUT");
}

Stream::~Stream()
{
    logVerbose(AC,"Stream::~Stream()-->IN");
    logVerbose(AC,"Stream::~Stream()-->OUT");
}

int Stream::getStreamID(void)
{
    logVerbose(AC,"Stream::getStreamID()-->IN");
    logVerbose(AC,"Stream::getStreamID()-->OUT");
    return mStreamID;
}

void Stream::requestAudioDeviceFocus(ama_connectID_t connectID)
{
    logVerbose(AC,"Stream::requestAudioDeviceFocus-->>IN connectID=", ToHEX(connectID));

    auto pConnection = AudioController::GetInstance()->findConnectionFromMapByConnectID(connectID);
    if(!pConnection){
        pConnection = AudioController::GetInstance()->createConnectionIntoMap(connectID);
        mConnectionMap[connectID] = pConnection;
    }

    pConnection->requestAudioDeviceFocus();

    logVerbose(AC,"Stream::requestAudioDeviceFocus-->>OUT");
}
void Stream::releaseAudioDeviceFocus(ama_connectID_t connectID)
{
    logVerbose(AC,"Stream::releaseAudioDeviceFocus-->>IN connectID=", ToHEX(connectID));

    auto pConnection = AudioController::GetInstance()->findConnectionFromMapByConnectID(connectID);
    if(pConnection){
        pConnection->releaseAudioDeviceFocus();
    }else{
        logWarn(AC,"this connecttion is't in map");
    }

    logVerbose(AC,"Stream::releaseAudioDeviceFocus-->>OUT");
}

void Stream::setStreamMuteState(bool isMute)
{
    logVerbose(AC,"Stream::setStreamMuteState-->>IN isMute=",isMute);

    map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    for(it=mConnectionMap.begin(); it!=mConnectionMap.end(); ++it){
        (it->second)->setStreamMute(isMute);
    }

    logVerbose(AC,"Stream::setStreamMuteState-->>OUT");
}

void Stream::execConnectWhenSourceCreated()
{
    logVerbose(AC,"Stream::execConnectWhenSourceCreated-->>IN");

    map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    for(it=mConnectionMap.begin(); it!=mConnectionMap.end(); ++it){
        (it->second)->execConnectWhenSourceCreated();
    }

    logVerbose(AC,"Stream::execConnectWhenSourceCreated-->>OUT");
}

void Stream::execDisconnectWhenSourceRemoved(void)
{
    logVerbose(AC,"Stream::execDisconnectWhenSourceRemoved-->>IN");

    map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    for(it=mConnectionMap.begin(); it!=mConnectionMap.end(); ++it){
        (it->second)->execDisconnectWhenSourceRemoved();
    }

    logVerbose(AC,"Stream::execDisconnectWhenSourceRemoved-->>OUT");
}

void Stream::notifyFocusStateChanged(ama_connectID_t connectID,ama_focusSta_t oldFocusStaForApp,ama_focusSta_t newFocusStaForApp)
{
    logVerbose(AC,"Stream::notifyFocusStateChanged-->>IN newFocusStaForApp:",ToHEX(newFocusStaForApp));
    notifyFocusStaChange2Process(connectID,newFocusStaForApp);

    logInfo(AC,"send a FocusChange broadcast by commonAPI");
    logInfoF(ADBG,"notify connectID[0x%x] focusSta[0x%x] changeTo focusSta[0x%x] because applyStreamID[0x%x]",connectID,oldFocusStaForApp,newFocusStaForApp,AudioController::GetInstance()->getApplyStreamID());
    AudioFocusChangedNotify(connectID,oldFocusStaForApp,newFocusStaForApp,AudioController::GetInstance()->getApplyStreamID());

    logVerbose(AC,"Stream::notifyFocusStateChanged-->>OUT");
}

void Stream::notifyFocusStaChange2Process(ama_connectID_t connectID,ama_focusSta_t focusStaForApp)
{
    logVerbose(AC,"Stream::notifyFocusStaChange2Process-->>IN focusStaForApp:",ToHEX(focusStaForApp));

    ama_focusType_t focusType = GET_FOCUSTYPE_FROM_STREAMID(connectID);
    ama_streamID_t streamID = GET_STREAMID_FROM_CONNECTID(connectID);
    ama_streamType_t streamType = GET_STREAMTYPE_FROM_CONNECTID(connectID);
    ama_AMPID_t ampid = GET_AMPID_FROM_CONNECTID(connectID);
    bool isAnonymous;
    logDebug(AC,"focusType of connectID is :",focusType);

    if((focusType>E_FOCUS_TYPE_SOFTKEY)&&(focusType<E_FOCUS_TYPE_MAX)){
        logInfo(AC,"update app status in ProcessInfo");
        ProcessInfo* info = ProcessInfo::GetInstance();
        info->Lock();
        shared_ptr<Process> process = ProcessInfo::GetInstance()->FindProc(ampid);
        if(process){
            logInfoF(AC,"update app(%x) status in ProcessInfo new focusSta(%x)",ampid,focusStaForApp);
            process->updateFocusStaByAC(streamID,focusStaForApp);
        }
        info->Unlock();
    }

    logVerbose(AC,"Stream::notifyFocusStaChange2Process-->>OUT");
}

void Stream::removeConnectionFromMap(ama_connectID_t connectID)
{
    logVerbose(AC,"Stream::removeConnectionFromMap()-->>IN");

    std::map<ama_connectID_t,shared_ptr<Connection>>::iterator it;
    it = mConnectionMap.find(connectID);
    if(it != mConnectionMap.end()){
        logDebug(AC,"find connection in mConnectionMap");
        mConnectionMap.erase(it);
    }else{
        logError(AC,"can't del connection,because there is't connection in map");
    }

    logVerbose(AC,"Stream::removeConnectionFromMap()-->>OUT");
}
