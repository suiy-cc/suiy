/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AMClient.cpp
/// @brief contains the implementation of class AMClient
///
/// Created by wangqi on 2016/6/16.
/// this file contains the implementation of class AMClient
///

#include <memory>
#include <cstring>
#include <string.h>
#include <algorithm>

#include "TaskDispatcher.h"
#include "AudioTask.h"
#include "PLCTask.h"
#include "ID.h"

#include "AMClient.h"
#include "AudioCtrl.h"
#include "Capi.h"

using std::string;
using std::shared_ptr;

static AMRoutingIDTable_s msFinalRoutingIDTable;
static std::mutex mThreadMutex;

static void amcmd_cbAMACNewSource(AMAC_SourceType_s source, void* pUserData){
    logVerbose(AC,"amcmd_cbAMACNewSource-->>IN");
    logVerbose(AC,"amcmd_cbAMACNewSource-->>source.sourceID=",source.sourceID);
    logVerbose(AC,"amcmd_cbAMACNewSource-->>source.domainID",source.domainID);
    string sourceName = source.name;
    logVerbose(AC,"amcmd_cbAMACNewSource-->>source.name=",sourceName);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(AMSOURCE_CREATED);
	pTask->setAMSource(source);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AC,"amcmd_cbAMACNewSource-->>OUT");
	return ;
}
static void amcmd_cbAMACRemovedSource (AMAC_SourceID_t sourceID, void* pUserData){
    logVerbose(AC,"amcmd_cbAMACRemovedSource-->>IN, sourceID=",sourceID);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(AMSOURCE_DESTROYED);
	pTask->setAMSourceID(sourceID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AC,"amcmd_cbAMACRemovedSource-->>OUT");
	return ;
}
static void amcmd_cbAMACNewSink(AMAC_SinkType_s sink,void* pUserData){
    logVerbose(AC,"amcmd_cbAMACNewSink-->>IN");
    logVerbose(AC,"amcmd_cbAMACNewSink-->>sink.sinkID=",sink.sinkID);
    logVerbose(AC,"amcmd_cbAMACNewSink-->>sink.domainID",sink.domainID);
    string sinkName = sink.name;
    logVerbose(AC,"amcmd_cbAMACNewSink-->>sink.name=",sinkName);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(AMSINK_CREATED);
	pTask->setAMSink(sink);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AC,"amcmd_cbAMACNewSink-->>OUT");
	return ;
}
static void amcmd_cbAMACRemovedSink(AMAC_SinkID_t sinkID, void* pUserData){
    logVerbose(AC,"amcmd_cbAMACRemovedSink-->>IN sinkID:",sinkID);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(AMSINK_DESTROYED);
	pTask->setAMSinkID(sinkID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(AC,"amcmd_cbAMACRemovedSink-->>OUT");
	return ;
}

static int am_ctrl_connect_cb(AMAC_SourceID_t sourceID, AMAC_SinkID_t sinkID,
							AMAC_SourceID_t sourceList[_MAX_CONNECT_STEP_],int32_t *sourceNum,
							AMAC_SinkID_t   sinkList[_MAX_CONNECT_STEP_]  ,int32_t *sinkNum,
							void* pUserData)
{
    logVerbose(AC,"am_ctrl_connect_cb-->>IN");

    AMRoutingIDTable_s* pRoutingIDTable = (AMRoutingIDTable_s*)pUserData;

    logDebug(AC,"sourceNum=",pRoutingIDTable->connectionCntOfPath);
    logDebug(AC,"sinkNum=",pRoutingIDTable->connectionCntOfPath);
    *sourceNum = pRoutingIDTable->connectionCntOfPath;
    *sinkNum = pRoutingIDTable->connectionCntOfPath;

    for(int i=0;i < pRoutingIDTable->connectionCntOfPath;i++){
        logDebug(AC,"sourceList[",i,"]=",pRoutingIDTable->conIDArray[i].first);
        sourceList[i] = pRoutingIDTable->conIDArray[i].first;
        logDebug(AC,"sinkList[",i,"]=",pRoutingIDTable->conIDArray[i].second);
        sinkList[i] = pRoutingIDTable->conIDArray[i].second;
    };

    logVerbose(AC,"am_ctrl_connect_cb-->>OUT");
    return 0;
}

static int am_ctrl_disconnect_cb(AMAC_SourceID_t sourceID, AMAC_SinkID_t sinkID,
							AMAC_SourceID_t sourceList[_MAX_CONNECT_STEP_],int32_t *sourceNum,
							AMAC_SinkID_t   sinkList[_MAX_CONNECT_STEP_]  ,int32_t *sinkNum,
							void* pUserData)
{
    logVerbose(AC,"am_ctrl_disconnect_cb-->>IN");

    AMRoutingIDTable_s* pRoutingIDTable = (AMRoutingIDTable_s*)pUserData;

    logDebug(AC,"sourceNum=",pRoutingIDTable->connectionCntOfPath);
    logDebug(AC,"sinkNum=",pRoutingIDTable->connectionCntOfPath);
    *sourceNum = pRoutingIDTable->connectionCntOfPath;
    *sinkNum = pRoutingIDTable->connectionCntOfPath;

    //we need disconnect codec route first,so we disconnect form end to start
    for(int i=((pRoutingIDTable->connectionCntOfPath)-1);i>=0;i--){
        logDebug(AC,"sourceList[",i,"]=",pRoutingIDTable->conIDArray[i].first);
        sourceList[i] = pRoutingIDTable->conIDArray[i].first;
        logDebug(AC,"sinkList[",i,"]=",pRoutingIDTable->conIDArray[i].second);
        sinkList[i] = pRoutingIDTable->conIDArray[i].second;
    };

    logVerbose(AC,"am_ctrl_disconnect_cb-->>OUT");
    return 0;
}

static int am_ctrl_setvolume_cb(AMAC_SourceID_t sourceID, AMAC_SinkID_t sinkID, AMAC_Volume_t volume,
							AMAC_SourceID_t sourceList[_MAX_CONNECT_STEP_], AMAC_Volume_t sourceVolumeList[_MAX_CONNECT_STEP_], int32_t *sourceNum,
							AMAC_SourceID_t sinkList[_MAX_CONNECT_STEP_], AMAC_Volume_t sinkVolumeList[_MAX_CONNECT_STEP_], int32_t *sinkNum,
							void* pUserData)
{
    logVerbose(AC,"am_ctrl_setvolume_cb-->>IN");

    AMRoutingIDTable_s* pRoutingIDTable = (AMRoutingIDTable_s*)pUserData;

    logDebug(AC,"sourceNum=",pRoutingIDTable->connectionCntOfPath);
    logDebug(AC,"sinkNum=",pRoutingIDTable->connectionCntOfPath);
    *sourceNum = pRoutingIDTable->connectionCntOfPath;
    *sinkNum = pRoutingIDTable->connectionCntOfPath;

    for(int i=0;i < pRoutingIDTable->connectionCntOfPath;i++){
        logDebug(AC,"sourceList[",i,"]=",pRoutingIDTable->conIDArray[i].first);
        sourceList[i] = pRoutingIDTable->conIDArray[i].first;
        if(i>=pRoutingIDTable->codecConnectStartIndex){
            sourceVolumeList[i] = pRoutingIDTable->volume_db;
        }else{
            sourceVolumeList[i] = 0;
        }
        logDebug(AC,"set sourceVolumeList[",i,"]=",sourceVolumeList[i]);

        logDebug(AC,"sinkList[",i,"]=",pRoutingIDTable->conIDArray[i].second);
        sinkList[i] = pRoutingIDTable->conIDArray[i].second;
        if(i>=pRoutingIDTable->codecConnectStartIndex){
            sinkVolumeList[i] = pRoutingIDTable->volume_db;
        }else{
            sinkVolumeList[i] = 0;
        }
        logDebug(AC,"set sinkVolumeList[",i,"]=",sinkVolumeList[i]);
    };

    logVerbose(AC,"am_ctrl_setvolume_cb-->>OUT");
    return 0;
}

static int am_ctrl_setmute_cb(AMAC_SourceID_t sourceID, AMAC_SinkID_t sinkID, int16_t mute,
							AMAC_SourceID_t sourceList[_MAX_CONNECT_STEP_], int16_t sourceMuteList[_MAX_CONNECT_STEP_], int32_t *sourceNum,
							AMAC_SourceID_t sinkList[_MAX_CONNECT_STEP_], int16_t sinkMuteList[_MAX_CONNECT_STEP_], int32_t *sinkNum,
							void* pUserData)
{
    logVerbose(AC,"am_ctrl_setmute_cb-->>IN");

    AMRoutingIDTable_s* pRoutingIDTable = (AMRoutingIDTable_s*)pUserData;

    logDebug(AC,"sourceNum=",pRoutingIDTable->connectionCntOfPath);
    logDebug(AC,"sinkNum=",pRoutingIDTable->connectionCntOfPath);
    *sourceNum = pRoutingIDTable->connectionCntOfPath;
    *sinkNum = pRoutingIDTable->connectionCntOfPath;

    for(int i=0;i < pRoutingIDTable->connectionCntOfPath;i++){
        logDebug(AC,"sourceList[",i,"]=",pRoutingIDTable->conIDArray[i].first);
        sourceList[i] = pRoutingIDTable->conIDArray[i].first;
        logDebug(AC,"preset sourceVolumeList[",i,"]=0");
        sourceMuteList[i] = AM_MUTE_FLAG;
        logDebug(AC,"sinkList[",i,"]=",pRoutingIDTable->conIDArray[i].second);
        sinkList[i] = pRoutingIDTable->conIDArray[i].second;
        logDebug(AC,"preset sinkVolumeList[",i,"]=0");
        sinkMuteList[i] = AM_MUTE_FLAG;
    };

    logVerbose(AC,"am_ctrl_setmute_cb-->>OUT");
    return 0;
}

static AMACCommandClient mCmdClientcb={
    newSource:amcmd_cbAMACNewSource,
    removedSource:amcmd_cbAMACRemovedSource,
    newSink:amcmd_cbAMACNewSink,
    removedSink:amcmd_cbAMACRemovedSink,
};

static AMACControllerClient mCtrlClientcb = {
    connect:am_ctrl_connect_cb,
    disconnect:am_ctrl_disconnect_cb,
    setvolume:am_ctrl_setvolume_cb,
    setmute:am_ctrl_setmute_cb,
    mUserData:&msFinalRoutingIDTable,
};

AMClient::AMClient()
:mLastMediaAppIVI(0x0)
,mLastMediaAppLRSE(0x0)
,mLastMediaAppRRSE(0x0)
{
    logVerbose(AC,"AMClient::AMClient()-->IN");

    createRoutingMap();

    //can not register Command and Controller Client because
    //it use callback right now,but AMClient didn't finish
    //registerCommandAndControllerClient();

    logVerbose(AC,"AMClient::AMClient()-->OUT");
}

AMClient::~AMClient()
{
    logVerbose(AC,"AMClient::~AMClient()-->IN");
    logVerbose(AC,"AMClient::~AMClient()-->OUT");
}

void AMClient::registerCommandAndControllerClient(void)
{
    logVerbose(AC,"AMClient::registerCommandAndControllerClient()-->IN");
    int ret = 0;

    ret = registerAMACCommandClient(&mCmdClientcb);
    if (ret != 0){
        logError(AC,"registerAMACCommandClient failed errorCode=",ret);
        return ;
    }else{
        logDebug(AC,"registerAMACCommandClient successed");
    }

    ret = registerAMACControllerClient(&mCtrlClientcb);
    if (ret != 0){
        logError(AC,"registerAMACControllerClient failed errorCode=",ret);
        return ;
    }else{
        logDebug(AC,"registerAMACControllerClient successed");
    }

    if(!isManagerStartFirstTime()){
        //we must disconnect all connection when ama-app-mananger restart
        ret = disconnectAllByAMACCommandClient();
        if (ret != 0){
            logError(AC,"disconnectAllByAMACCommandClient failed errorCode=",ret);
            return ;
        }else{
            logDebug(AC,"disconnectAllByAMACCommandClient successed");
        }
    }

    logVerbose(AC,"AMClient::registerCommandAndControllerClient()-->OUT");
}

bool AMClient::isManagerStartFirstTime(void)
{
    if(0 == (access("/tmp/IVI-PowerState",F_OK))){
        ///tmp/IVI-PowerState is exist
        return false;
    }else{
        return true;
    }
}

ama_Error_e AMClient::connectByAMClient(ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::connectByAMClient()-->IN connectID=", ToHEX(connectID));

    std::unique_lock<std::mutex> uLock(mThreadMutex);
    bool isConnect = true;
    //we set ampMode here,and we must set ampMode first,then connect
    notifyConnectStaChanged(connectID,isConnect);
    int ret = connectStepByStep(connectID);
    if(E_OK == ret){
        recordLastConnectMediaApp(connectID,isConnect);
        recordActiveConnection(connectID,isConnect);
        updateActiveStreamTypeForHardKey();
    }

    logVerbose(AC,"AMClient::connectByAMClient()-->OUT");
    return (ama_Error_e)ret;
}

void AMClient::recordActiveConnection(ama_connectID_t connectID, bool isConnect)
{
    logVerbose(AC,"AMClient::recordActiveConnection()-->IN connectID=", ToHEX(connectID)," isConnect:",isConnect);

    if(isConnect){
        for(std::vector<ama_connectID_t> ::iterator it=mActiveConnectionVec.begin();it!=mActiveConnectionVec.end(); ++it){
            if(*it == connectID){
                logVerbose(AC,"connectID already in mActiveConnectionVec");
                return;
            }
        }
        mActiveConnectionVec.push_back(connectID);
    }else{
        for(std::vector<ama_connectID_t> ::iterator it=mActiveConnectionVec.begin();it!=mActiveConnectionVec.end(); ++it){
            if(*it == connectID){
                logVerbose(AC,"remove connectID from mActiveConnectionVec");
                mActiveConnectionVec.erase(it);
                return;
            }
        }
        logError(AC,"can't remove connectID from mActiveConnectionVec because there isn't the connectID in vector");
    }

    logVerbose(AC,"AMClient::recordActiveConnection()-->OUT");
}

ama_AMPID_t AMClient::getLastConnectMediaApp(ama_audioDeviceType_t deviceID)
{
    logVerbose(AC,"AMClient::getLastConnectMediaApp()-->IN");

    switch(deviceID){
        case E_IVI_SPEAKER:     return mLastMediaAppIVI;
        case E_RSE_L_HEADSET:   return mLastMediaAppLRSE;
        case E_RSE_R_HEADSET:   return mLastMediaAppRRSE;
        default: logError(AC,"audioDeviceID is not defined");return 0x0;
    }

    logVerbose(AC,"AMClient::getLastConnectMediaApp()-->OUT");
}

void AMClient::recordLastConnectMediaApp(ama_connectID_t connectID, bool isConnect)
{
    logVerbose(AC,"AMClient::recordLastConnectMediaApp()-->IN");

    ama_audioDeviceType_t deviceID = GET_DEVICETYPE_FROM_CONNECTID(connectID);
    ama_focusType_t focusType = GET_FOCUSTYPE_FROM_CONNECTID(connectID);
    ama_streamType_t streamType = GET_STREAMTYPE_FROM_CONNECTID(connectID);
    ama_AMPID_t ampid = GET_AMPID_FROM_CONNECTID(connectID);
    bool isAnonymous = false;

    if(isConnect){
        if(focusType == E_FOCUS_TYPE_MEDIA){
            switch(deviceID){
                case E_IVI_SPEAKER:     mLastMediaAppIVI = ampid; break;
                case E_RSE_L_HEADSET:   mLastMediaAppLRSE = ampid; break;
                case E_RSE_R_HEADSET:   mLastMediaAppRRSE = ampid; break;
                default: logError(AC,"audioDeviceID is not defined");break;
            }
            logDebug(AC,"recordLastConnectMediaApp-->",ampid);
        }

        //because btaudio request by bt service
        isAnonymous = IS_ANONYMOUS(ampid);
        if((E_STREAM_BT_AUDIO==streamType)&&(isAnonymous)){
            logDebug(AC,"recordLastConnectMediaApp because bt service play bt audio-->",ampid);
            //set last media app == 0x1004,music app play btMusic
            ampid = AMPID_APP(E_SEAT_IVI,E_APPID_MUSIC);
            mLastMediaAppIVI = ampid;
        }
    }

    logVerbose(AC,"AMClient::recordLastConnectMediaApp()-->OUT");
}

ama_volumeType_t AMClient::getActiveStreamTypeWhenHardKeySet()
{
    logVerbose(AC,"AMClient::getActiveStreamTypeWhenHardKeySet()-->IN");

    ama_volumeType_t volumeType;

    std::vector<ama_volumeInfo_t> volInfoVec;
    ama_volumeInfo_t ECallVolInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_ECALL,E_IVI_SPEAKER);
    ama_volumeInfo_t phoneVolInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_PHONE,E_IVI_SPEAKER);
    ama_volumeInfo_t VRTTSVolInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_VRTTS,E_IVI_SPEAKER);
    ama_volumeInfo_t mediaVolInfo = CREATE_VOLINFO_BY_VOLUMETYPE_DEVICETYPE(E_VOLUME_TYPE_MEDIA,E_IVI_SPEAKER);
    ama_volumeInfo_t getVolInfo;
    for(std::vector<ama_connectID_t> ::iterator it=mActiveConnectionVec.begin();it!=mActiveConnectionVec.end(); ++it){
        getVolInfo = GET_VOLUMEINFO_FROM_CONNECTID((*it));
        logDebug(AC,"activeConnectID:",ToHEX(*it));
        logDebug(AC,"getVolInfo:",ToHEX(getVolInfo));
        //only get speaker connect
        if(E_IVI_SPEAKER==GET_DEVICETYPE_FROM_CONNECTID((*it))){
            if((ECallVolInfo==getVolInfo)
                || (phoneVolInfo== getVolInfo)
                || (VRTTSVolInfo== getVolInfo)
                || (mediaVolInfo== getVolInfo)){
                volInfoVec.push_back(getVolInfo);
            }
        }
    }

    //ECall > Phone > VRTTS > Media in funspec
    //and streamTypeClass defined also ECall > Phone > VRTTS > Media
    if(volInfoVec.empty()){//no stream sounding in those four types
        logInfo("AC,activeStreamType:E_VOLUME_TYPE_MEDIA");
        volumeType = E_VOLUME_TYPE_MEDIA;
    }else if(volInfoVec.size()==1){
        getVolInfo = volInfoVec.front();
        volumeType = GET_VOLUMEINFO_FROM_VOLUMEINFO(getVolInfo);
    }else{
        logError(AC,"there is not a right situation");
        sort(volInfoVec.begin(),volInfoVec.end());
        getVolInfo = volInfoVec.back();
        volumeType = GET_VOLUMEINFO_FROM_VOLUMEINFO(getVolInfo);
    }

    logVerbose(AC,"AMClient::getActiveStreamTypeWhenHardKeySet()-->OUT activeStreamType:",volumeType);
    return volumeType;
}

void AMClient::updateActiveStreamTypeForHardKey()
{
    logVerbose(AC,"AMClient::updateActiveStreamTypeForHardKey()-->IN");

    ama_volumeType_t volumeType = getActiveStreamTypeWhenHardKeySet();
    BroadcastAMGRSetNotify(E_ActiveStreamForHardKey,to_string(volumeType));

    logVerbose(AC,"AMClient::updateActiveStreamTypeForHardKey()-->OUT");
}

bool AMClient::isVolInfoActive(ama_volumeInfo_t volInfo)
{
    logVerbose(AC,"AMClient::isVolInfoActive()-->IN volInfo=", ToHEX(volInfo));

    for(std::vector<ama_connectID_t> ::iterator it=mActiveConnectionVec.begin();it!=mActiveConnectionVec.end(); ++it){
        if(volInfo == GET_VOLUMEINFO_FROM_CONNECTID((*it))){
            logVerbose(AC,"connectID already in mActiveConnectionVec");
            logVerbose(AC,"AMClient::isVolInfoActive()-->OUT");
            return true;
        }
    }

    logVerbose(AC,"AMClient::isVolInfoActive()-->OUT");
    return false;
}

int AMClient::connectStepByStep(ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::connectStepByStep()-->IN connectID=", ToHEX(connectID));

    int ret = 0;
    ama_connectInfo_t connectInfo = GET_CONNECTINFO_FROM_CONNECTID(connectID);
    logInfo(AC,"connect connectInfo=",connectInfo);

    AMRoutingNameTable_s* pRoutingNameTable;

    ret = getRoutingNameTable(connectInfo,&pRoutingNameTable);
    if(E_OK != ret){
        return ret;
    }

    AMRoutingIDTable_s routingIDTable;

    ret = getRoutingIDTable(pRoutingNameTable,&routingIDTable,connectID);
    if(E_OK != ret){
        return ret;
    }

    ret = getConFinalRoutingIDTableByConnectionSta(&routingIDTable, &msFinalRoutingIDTable);
    if(E_OK != ret){
        return ret;
    }

    logVerbose(AC,"exec registerAMACControllerClient()");
    ret = registerAMACControllerClient(&mCtrlClientcb);
    if (ret != 0){
        logError(AC,"registerAMACControllerClient failed errorCode=",ret);
        return E_UNKNOWN;
    }else{
        logDebug(AC,"registerAMACControllerClient successed");
    }

    logVerbose(AC,"exec connnetByAMACCommandClient()");
    ret = connnetByAMACCommandClient(0, 0);
    if(0 == ret){
        logInfo(AC,"connnetByAMACCommandClient successed");
        logInfoF(ADBG,"connect connectID[0x%x] in audioManager",connectID);
        recordConStaOfRoutingTable(&routingIDTable, true);
    }else{
        logError(AC,"connnetByAMACCommandClient failed");
        return ret;
    }

    if((pRoutingNameTable->gainNum) != NO_GAIN_SET){
        logVerbose(AC,"exec setConnectGainByAMACCommandClient()");
        ret = setConnectGainByAMACCommandClient(pRoutingNameTable->amacChannel,pRoutingNameTable->gainNum,pRoutingNameTable->gainList);
        if(0 == ret){
            logInfo(AC,"setConnectGainByAMACCommandClient successed");
        }else{
            logError(AC,"setConnectGainByAMACCommandClient failed");
            return ret;
        }
    }

    logVerbose(AC,"AMClient::connectStepByStep()-->OUT");
    return E_OK;
}

ama_Error_e AMClient::disconnectByAMClient(ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::disconnectByAMClient()-->IN connectID=", ToHEX(connectID));

    std::unique_lock<std::mutex> uLock(mThreadMutex);
    int ret = disconnectStepByStep(connectID);
    if(E_OK == ret){
        bool isConnect = false;
        notifyConnectStaChanged(connectID,isConnect);
        recordActiveConnection(connectID,isConnect);
        updateActiveStreamTypeForHardKey();
    }

    logVerbose(AC,"AMClient::disconnectByAMClient()-->OUT");
    return (ama_Error_e)ret;
}

int AMClient::disconnectStepByStep(ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::disconnectStepByStep()-->IN connectID=", ToHEX(connectID));

    int ret = E_OK;
    ama_connectInfo_t connectInfo = GET_CONNECTINFO_FROM_CONNECTID(connectID);
    logDebug(AC,"disconnect connectInfo=",connectInfo);

    AMRoutingNameTable_s* pRoutingNameTable;

    ret = getRoutingNameTable(connectInfo,&pRoutingNameTable);
    if(E_OK != ret){
        return ret;
    }

    AMRoutingIDTable_s routingIDTable;

    ret = getRoutingIDTable(pRoutingNameTable,&routingIDTable,connectID);
    if(E_OK != ret){
        return ret;
    }

    ret = getDisconFinalRoutingIDTableByConnectionSta(&routingIDTable, &msFinalRoutingIDTable);
     if(E_OK != ret){
        return ret;
    }


    logVerbose(AC,"exec registerAMACControllerClient()");
    ret = registerAMACControllerClient(&mCtrlClientcb);

    if (ret != 0){
        logError(AC,"registerAMACControllerClient failed errorCode=",ret);
        return E_UNKNOWN;
    }else{
        logDebug(AC,"registerAMACControllerClient successed");
    }

    logVerbose(AC,"exec setMuteByAMACCommandClient() before disconnect it");
    ret = setMuteByAMACCommandClient(0, 0, 0);
    if(0 == ret){
        logInfo(AC,"setMuteByAMACCommandClient successed");
    }else{
        logError(AC,"setMuteByAMACCommandClient failed ");
        //we don't need to return when we set mute before disconnect it
        // return ret;
    }

    // logVerbose(AC,"delay 30ms before disconnect it");
    usleep(30*1000);

    logVerbose(AC,"exec disconnectByAMACCommandClient()");
    ret = disconnnetByAMACCommandClient(0, 0);
    if(E_OK == ret){
        logInfo(AC,"disconnectByAMACCommandClient successed");
        logInfoF(ADBG,"disconnect connectID[0x%x] in audioManager",connectID);
        recordConStaOfRoutingTable(&routingIDTable, false);
    }else{
        logError(AC,"disconnectByAMACCommandClient failed");
        return ret;
    }

    logVerbose(AC,"AMClient::disconnectStepByStep()-->OUT");
    return E_OK;
}

ama_Error_e AMClient::setConnectionVolGain(ama_connectID_t connectID, int gain)
{
    logVerbose(AC,"AMClient::setConnectionVolGain()-->IN connectID=", ToHEX(connectID)," gain=",gain);

    std::unique_lock<std::mutex> uLock(mThreadMutex);
    int ret = 0;
    ama_connectInfo_t connectInfo = GET_CONNECTINFO_FROM_CONNECTID(connectID);
    logInfo(AC,"connectInfo=",connectInfo);

    AMRoutingNameTable_s* pRoutingNameTable;

    ret = getRoutingNameTable(connectInfo,&pRoutingNameTable);
    if(E_OK != ret){
        return (ama_Error_e)ret;
    }

    ret = getRoutingIDTable(pRoutingNameTable,&msFinalRoutingIDTable,connectID);
    if(E_OK != ret){
        return (ama_Error_e)ret;
    }

    msFinalRoutingIDTable.volume_db = gain;

    logVerbose(AC,"exec registerAMACControllerClient()");
    ret = registerAMACControllerClient(&mCtrlClientcb);

    if (ret != 0){
        logError(AC,"registerAMACControllerClient failed errorCode=",ret);
        return E_UNKNOWN;
    }else{
        logDebug(AC,"registerAMACControllerClient successed");
    }

    logVerbose(AC,"exec setVolumeByAMACCommandClient()");
    ret = setVolumeByAMACCommandClient(0, 0, 0);
    if(0 == ret){
        logInfo(AC,"setVolumeByAMACCommandClient successed ");
        logInfoF(ADBG,"set volumeGain[%d] for connectID[0x%x] in audioManager",gain,connectID);
    }else{
        logError(AC,"setVolumeByAMACCommandClient failed ");
        return (ama_Error_e)ret;
    }

    logVerbose(AC,"AMClient::setConnectionVolGain()-->OUT");
    return E_OK;
}

int AMClient::setStreamMute(ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::setStreamMute()-->IN connectID=", ToHEX(connectID));

    int ret = 0;
    ama_connectInfo_t connectInfo = GET_CONNECTINFO_FROM_CONNECTID(connectID);
    logDebug(AC,"connectInfo=",connectInfo);

    AMRoutingNameTable_s* pRoutingNameTable;

    ret = getRoutingNameTable(connectInfo,&pRoutingNameTable);
    if(E_OK != ret){
        return ret;
    }

    ret = getRoutingIDTable(pRoutingNameTable,&msFinalRoutingIDTable,connectID);
    if(E_OK != ret){
        return ret;
    }

    logVerbose(AC,"exec registerAMACControllerClient()");
    ret = registerAMACControllerClient(&mCtrlClientcb);

    if (ret != 0){
        logError(AC,"registerAMACControllerClient failed errorCode=",ret);
        return E_UNKNOWN;
    }else{
        logDebug(AC,"registerAMACControllerClient successed");
    }

    logVerbose(AC,"exec setMuteByAMACCommandClient()");
    ret = setMuteByAMACCommandClient(0, 0, 0);
    if(0 == ret){
        logInfo(AC,"setMuteByAMACCommandClient successed");
    }else{
        logError(AC,"setMuteByAMACCommandClient failed ");
        return ret;
    }

    logVerbose(AC,"AMClient::setStreamMute()-->OUT");
    return E_OK;
}

int AMClient::getAMSourceIDFromMap(string sourceName)
{
    logVerbose(AC,"AMClient::getAMSourceIDFromMap()-->IN sourceName=",sourceName);

    std::map<string,shared_ptr<AMSource>>::iterator it;
    it = mAMSourceMap.find(sourceName);
    if(it != mAMSourceMap.end()){
        logVerbose(AC,"AMClient::getAMSourceIDFromMap()-->OUT AMSourceID=",mAMSourceMap[sourceName]->sourceID());
        return mAMSourceMap[sourceName]->sourceID();
    }else{
        logWarn(AC,"can't find sourceName in mAMSourceMap");
        logVerbose(AC,"AMClient::getAMSourceIDFromMap()-->OUT");
        return E_NO_SOURCE;
    }
}
int AMClient::getAMSinkIDFromMap(string sinkName)
{
    logVerbose(AC,"AMClient::getAMSinkIDFromMap()-->IN sinkName=",sinkName);

    std::map<string,shared_ptr<AMSink>>::iterator it;
    it = mAMSinkMap.find(sinkName);
    if(it != mAMSinkMap.end()){
        logVerbose(AC,"AMClient::getAMSinkIDFromMap()-->OUT AMSinkID=",mAMSinkMap[sinkName]->sinkID());
        return mAMSinkMap[sinkName]->sinkID();
    }else{
        logWarn(AC,"can't find sinkName in mAMSinkMap");
        logVerbose(AC,"AMClient::getAMSinkIDFromMap()-->OUT");
        return E_NO_SINK;
    }
}

void AMClient::addAMSourceIntoMap(string sourceName,AMAC_SourceID_t sourceID)
{
    logVerbose(AC,"AMClient::addAMSourceIntoMap()-->IN sourceName=",sourceName," sourceID=",sourceID);

    std::map<string,shared_ptr<AMSource>>::iterator it;
    it = mAMSourceMap.find(sourceName);
    if(it != mAMSourceMap.end()){
        logError(AC,"newSource:",sourceName," already in mAMSourceMap");
    }else{
        logInfo(AC,"can't find newSource in mAMSourceMap, create AMSource ...");
        shared_ptr<AMSource> pAMSource = std::make_shared<AMSource>(sourceID,sourceName);
        mAMSourceMap[sourceName] = pAMSource;
    }

    logVerbose(AC,"AMClient::addAMSourceIntoMap()-->OUT");
}

void AMClient::delAMSourceFromMap(AMAC_SourceID_t sourceID)
{
    logVerbose(AC,"AMClient::delAMSourceFromMap()-->IN sourceID=",sourceID);

    for(std::map<string,shared_ptr<AMSource>>::iterator it=mAMSourceMap.begin();
        it!=mAMSourceMap.end(); ++it){
        if((it->second)->sourceID() == sourceID){
            mAMSourceMap.erase(it);
            logVerbose(AC,"AMClient::delAMSourceFromMap()-->OUT");
            return;
        }
    }
    logError(AC,"can't find AMSource in mAMSourceMap");
    logVerbose(AC,"AMClient::delAMSourceFromMap()-->OUT");
}

string AMClient::getSourceNameByID(AMAC_SourceID_t sourceID)
{
    logVerbose(AC,"AMClient::getSourceNameByID()-->IN sourceID=", ToHEX(sourceID));

    for(std::map<string,shared_ptr<AMSource>>::iterator it=mAMSourceMap.begin();
        it!=mAMSourceMap.end(); ++it){
        if((it->second)->sourceID() == sourceID){
            logVerbose(AC,"AMClient::getSourceNameByID()-->OUT");
            return ((it->second)->name());
        }
    }
    logError(AC,"can't find AMSource in mAMSourceMap");
    logVerbose(AC,"AMClient::getSourceNameByID()-->OUT");
    return "";
}


void AMClient::addAMSinkIntoMap(string sinkName,AMAC_SinkID_t sinkID)
{
    logVerbose(AC,"AMClient::addAMSinkIntoMap()-->IN sinkName=",sinkName," sinkID=",sinkID);

    std::map<string,shared_ptr<AMSink>>::iterator it;
    it = mAMSinkMap.find(sinkName);
    if(it != mAMSinkMap.end()){
        logError(AC,"newSink:",sinkName," already in mAMSinkMap");
    }else{
        logInfo(AC,"can't find newSink in mAMSinkMap, create AMSink ...");
        shared_ptr<AMSink> pAMSink = std::make_shared<AMSink>(sinkID,sinkName);
        mAMSinkMap[sinkName] = pAMSink;
    }

    logVerbose(AC,"AMClient::addAMSinkIntoMap()-->OUT");
}

void AMClient::delAMSinkFromMap(AMAC_SinkID_t sinkID)
{
    logVerbose(AC,"AMClient::delAMSinkFromMap()-->IN sinkID=",sinkID);

    for(std::map<string,shared_ptr<AMSink>>::iterator it=mAMSinkMap.begin();
        it!=mAMSinkMap.end(); ++it){
        if((it->second)->sinkID() == sinkID){
            mAMSinkMap.erase(it);
            logVerbose(AC,"AMClient::delAMSinkFromMap()-->OUT");
            return;
        }
    }
    logError(AC,"can't find AMSink in mAMSinkMap");
    logVerbose(AC,"AMClient::delAMSinkFromMap()-->OUT");
}

AMAC_Channel_t AMClient::getAMACChannelByConnectID(ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::getAMACChannelByConnectID()-->IN connectID=", ToHEX(connectID));

    AMAC_Channel_t AMACChannel = AMAC_CHANNEL_NULL;
    ama_connectInfo_t connectInfo = GET_CONNECTINFO_FROM_CONNECTID(connectID);
    std::map<ama_connectInfo_t,AMRoutingNameTable_s*>::iterator it;
    it = mRoutingMap.find(connectInfo);
    if(it != mRoutingMap.end()){
        logDebug(AC,"find the routingTable of connectInfo");
        AMACChannel = (mRoutingMap[connectInfo])->amacChannel;
    }else{
        logError(AC,"can't find routingTable of connectInfo in mRoutingMap");
    }

    logVerbose(AC,"AMClient::getAMACChannelByConnectID()-->OUT");
    return AMACChannel;
}

int AMClient::getRoutingNameTable(ama_connectInfo_t connectInfo, AMRoutingNameTable_s** ppRoutingTable)
{
    logVerbose(AC,"AMClient::getRoutingNameTable()-->IN connectInfo=", ToHEX(connectInfo));

    std::map<ama_connectInfo_t,AMRoutingNameTable_s*>::iterator it;
    it = mRoutingMap.find(connectInfo);
    if(it != mRoutingMap.end()){
        logVerbose(AC,"AMClient::getRoutingNameTable()-->OUT find the routingTable of connectInfo");
        *ppRoutingTable = mRoutingMap[connectInfo];
        // logPrintAMRoutingNameTable(*ppRoutingTable);
        return E_OK;
    }else{
        logError(AC,"can't find routingTable of connectInfo in mRoutingMap");
        logVerbose(AC,"AMClient::getRoutingNameTable()-->OUT");
        return E_NO_ROUTINGMAP;
    }
}

string AMClient::convertSourceNameIfPASource(string sourceName,ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::convertSourceNameIfPASource()-->IN connectID=", ToHEX(connectID), "sourceName=",sourceName);

    string convertSourceName;
    if(sourceName == PULSEAUDIO_STREAM){
        ama_streamID_t streamID = GET_STREAMID_FROM_CONNECTID(connectID);
        convertSourceName = ToHEX(streamID);
    }else{
        convertSourceName = sourceName;
    }

    logVerbose(AC,"AMClient::convertSourceNameIfPASource()-->OUT convertSourceName:",convertSourceName);
    return convertSourceName;
}

string AMClient::convertSinkNameIfPASink(string sinkName,ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::convertSinkNameIfPASink()-->IN connectID=", ToHEX(connectID), "sinkName=",sinkName);

    string convertSinkName;
    if(sinkName == PULSEAUDIO_STREAM){
        ama_streamID_t streamID = GET_STREAMID_FROM_CONNECTID(connectID);
        convertSinkName = ToHEX(streamID);
    }else{
        convertSinkName = sinkName;
    }

    logVerbose(AC,"AMClient::convertSinkNameIfPASink()-->OUT convertSinkName:",convertSinkName);
    return convertSinkName;
}

int AMClient::getRoutingIDTable(AMRoutingNameTable_s* pRoutingNameTable, AMRoutingIDTable_s *pRoutingIdTable,ama_connectID_t connectID)
{
    logVerbose(AC,"AMClient::getRoutingIDTable()-->IN");

    pRoutingIdTable->connectionCntOfPath = pRoutingNameTable->connectionCntOfPath;
    pRoutingIdTable->codecConnectStartIndex = pRoutingNameTable->codecConnectStartIndex;
    pRoutingIdTable->pRoutingNameTable = pRoutingNameTable;

    int ret;
    AMAC_SourceID_t sourceID;
    AMAC_SinkID_t sinkID;
    string sourceName;
    string sinkName;
    for(int i=0;i < pRoutingIdTable->connectionCntOfPath;i++){
        sourceName = convertSourceNameIfPASource(pRoutingNameTable->conNameArray[i].first,connectID);

        ret = getAMSourceIDFromMap(sourceName);
        if(ret >= E_OK){
            sourceID = ret;
        }else{
            return E_NO_SOURCE;
        }

        sinkName = convertSinkNameIfPASink(pRoutingNameTable->conNameArray[i].second,connectID);
        ret = getAMSinkIDFromMap(sinkName);
        if(ret >= E_OK){
            sinkID = ret;
        }else{
            return E_NO_SINK;
        }

        pRoutingIdTable->conIDArray[i] = make_pair(sourceID,sinkID);
    };

    logVerbose(AC,"AMClient::getRoutingIDTable()-->OUT");
    return E_OK;
}

int AMClient::getConFinalRoutingIDTableByConnectionSta(AMRoutingIDTable_s *pRoutingIdTable,AMRoutingIDTable_s *pFinalRoutingIdTable)
{
    logVerbose(AC,"AMClient::getFinalRoutingIDTableByConnectionSta()-->IN");

    int finalConnectionIndex = 0;
    for(int i=0;i < pRoutingIdTable->connectionCntOfPath;i++){
        if((isConnectionNeedExec(pRoutingIdTable->conIDArray[i]))&&(isConnectionCanExecByAMAC(pRoutingIdTable->conIDArray[i]))){
            pFinalRoutingIdTable->conIDArray[finalConnectionIndex] = pRoutingIdTable->conIDArray[i];
            finalConnectionIndex++;
        }
    }

    pFinalRoutingIdTable->connectionCntOfPath = finalConnectionIndex;
    pFinalRoutingIdTable->codecConnectStartIndex = pRoutingIdTable->codecConnectStartIndex;
    pFinalRoutingIdTable->pRoutingNameTable = pRoutingIdTable->pRoutingNameTable;

    logVerbose(AC,"AMClient::getFinalRoutingIDTableByConnectionSta()-->OUT");
    return E_OK;
}

int AMClient::getDisconFinalRoutingIDTableByConnectionSta(AMRoutingIDTable_s *pRoutingIdTable,AMRoutingIDTable_s *pFinalRoutingIdTable)
{
    logVerbose(AC,"AMClient::getDisconFinalRoutingIDTableByConnectionSta()-->IN");

    int finalConnectionIndex = 0;
    for(int i=0;i < pRoutingIdTable->connectionCntOfPath;i++){
        if((isDisconnectNeedExec(pRoutingIdTable->conIDArray[i]))&&(isDisconnectCanExecByAMAC(pRoutingIdTable->conIDArray[i]))){
            pFinalRoutingIdTable->conIDArray[finalConnectionIndex] = pRoutingIdTable->conIDArray[i];
            finalConnectionIndex++;
        }
    }

    pFinalRoutingIdTable->connectionCntOfPath = finalConnectionIndex;
    pFinalRoutingIdTable->codecConnectStartIndex = pRoutingIdTable->codecConnectStartIndex;

    logVerbose(AC,"AMClient::getDisconFinalRoutingIDTableByConnectionSta()-->OUT");
    return E_OK;
}

void AMClient::recordConStaOfRoutingTable(AMRoutingIDTable_s *pRoutingIdTable,bool isConnect)
{
    logVerbose(AC,"AMClient::recordConStaOfRoutingTable()-->IN");

    for(int i=0;i < pRoutingIdTable->connectionCntOfPath;i++){
        recordConnectState(pRoutingIdTable->conIDArray[i], isConnect);
    }

    logVerbose(AC,"AMClient::recordConStaOfRoutingTable()-->OUT");
}

bool AMClient::isConnectionNeedExec(AMConnectionIDPair connection)
{
    logVerbose(AC,"AMClient::isConnectionNeedExec()-->IN");
    logInfo(AC,"connection.sourceID=",connection.first," connection.sinkID=",connection.second);

    int connectionCnt;
    std::map<AMConnectionIDPair,int>::iterator it;
    it = mConnectionState.find(connection);
    if(it != mConnectionState.end()){
        connectionCnt = mConnectionState[connection];
        if(connectionCnt > 0){
            logInfo(AC,"connection has connect,and last connectionCnt=",connectionCnt);
            logVerbose(AC,"AMClient::isConnectionNeedExec()-->OUT");
            return false;
        }else if(connectionCnt == 0){
            logInfo(AC,"connection need to connect,and last connectionCnt=",connectionCnt);
            logVerbose(AC,"AMClient::isConnectionNeedExec()-->OUT");
            return true;
        }else{
            logError(AC,"connection has error state");
            mConnectionState[connection] = 0;
            logVerbose(AC,"AMClient::isConnectionNeedExec()-->OUT");
            return true;
        }
    }else{
        logInfo(AC,"don't have a connection state, create a state of connection ...");
        mConnectionState[connection] = 0;
        logVerbose(AC,"AMClient::isConnectionNeedExec()-->OUT");
        return true;
    }
}

bool AMClient::isConnectionCanExecByAMAC(AMConnectionIDPair connection)
{
    logVerbose(AC,"AMClient::isConnectionCanExecByAMAC()-->IN");
    logInfo(AC,"connection.sourceID=",connection.first," connection.sinkID=",connection.second);

    int ret =0;
    ret = checkConnectionExistByAMACCommandClient(connection.first,connection.second);
    if(1==ret){
        logDebug(AC,"connection is exist in AMAC,so we can't connect it!");
        logVerbose(AC,"AMClient::isConnectionCanExecByAMAC()-->OUT");
        return false;
    }else{
        logDebug(AC,"connection is not exist in AMAC,so we can connect it!");
        logVerbose(AC,"AMClient::isConnectionCanExecByAMAC()-->OUT");
        return true;
    }
}


bool AMClient::isDisconnectNeedExec(AMConnectionIDPair disconnection)
{
    logVerbose(AC,"AMClient::isDisconnectNeedExec()-->IN");
    logDebug(AC,"disconnection.sourceID=",disconnection.first," disconnection.sinkID=",disconnection.second);

    int connectionCnt;
    std::map<AMConnectionIDPair,int>::iterator it;
    it = mConnectionState.find(disconnection);
    if(it != mConnectionState.end()){
        connectionCnt = mConnectionState[disconnection];
        if(connectionCnt > 1){
            logInfo(AC,"connection don't need to disconnect,other connection also need this connection,and last connectionCnt=",connectionCnt);
            logVerbose(AC,"AMClient::isDisconnectNeedExec()-->OUT");
            return false;
        }else if(connectionCnt == 1){
            logInfo(AC,"connection need to disconnect,and last connectionCnt=",connectionCnt);
            logVerbose(AC,"AMClient::isDisconnectNeedExec()-->OUT");
            return true;
        }else if(connectionCnt == 0){
            logError(AC,"connection have already disconnect,and last connectionCnt=",connectionCnt);
            mConnectionState[disconnection] = 0;
            logVerbose(AC,"AMClient::isDisconnectNeedExec()-->OUT");
            return true;
        }else{
            logError(AC,"connection has error state");
            mConnectionState[disconnection] = 0;
            logVerbose(AC,"AMClient::isDisconnectNeedExec()-->OUT");
            return true;
        }
    }else{
        logError(AC,"don't have a connection state");
        mConnectionState[disconnection] = 0;
        logVerbose(AC,"AMClient::isDisconnectNeedExec()-->OUT");
        return true;
    }
}

bool AMClient::isDisconnectCanExecByAMAC(AMConnectionIDPair disconnection)
{
    logVerbose(AC,"AMClient::isDisconnectCanExecByAMAC()-->IN");
    logDebug(AC,"disconnection.sourceID=",disconnection.first," disconnection.sinkID=",disconnection.second);

    int ret =0;
    ret = checkConnectionExistByAMACCommandClient(disconnection.first,disconnection.second);
    if(1==ret){
        logDebug(AC,"connection is exist in AMAC,so we can disconnect it!");
        logVerbose(AC,"AMClient::isDisconnectCanExecByAMAC()-->OUT");
        return true;
    }else{
        logDebug(AC,"connection is not exist in AMAC,so we can't disconnect it!");
        logVerbose(AC,"AMClient::isDisconnectCanExecByAMAC()-->OUT");
        return false;
    }
}

void AMClient::recordConnectState(AMConnectionIDPair connection,bool isConnect)
{
    logVerbose(AC,"AMClient::recordConnectState()-->IN isConnect=",isConnect);
    logDebug(AC,"connection.sourceID=",connection.first," connection.sinkID=",connection.second);

    int connectionCnt;
    std::map<AMConnectionIDPair,int>::iterator it;
    it = mConnectionState.find(connection);
    if(it != mConnectionState.end()){
        connectionCnt = mConnectionState[connection];
        if(isConnect){
            if(connectionCnt >= 0){
                mConnectionState[connection] = connectionCnt+1;
                logDebug(AC,"changeConSta,now connectionCnt=",mConnectionState[connection]);
                logVerbose(AC,"AMClient::recordConnectState()-->OUT");
                return;
            }else{
                logError(AC,"Error State: conSta < 0!");
                mConnectionState[connection] = 0;
                logDebug(AC,"changeConSta,now connectionCnt=",mConnectionState[connection]);
                logVerbose(AC,"AMClient::recordConnectState()-->OUT");
                return;
            }
        }else{
            if(connectionCnt > 0){
                mConnectionState[connection] = connectionCnt-1;
                logDebug(AC,"changeConSta,now connectionCnt=",mConnectionState[connection]);
                logVerbose(AC,"AMClient::recordConnectState()-->OUT");
                return;
            }else{
                logError(AC,"Error State: conSta <= 0!");
                mConnectionState[connection] = 0;
                logDebug(AC,"changeConSta,now connectionCnt=",mConnectionState[connection]);
                logVerbose(AC,"AMClient::recordConnectState()-->OUT");
                return;
            }
        }
    }else{
        logError(AC,"don't have a connection state, create a state of connection ...");
        if(isConnect){
            mConnectionState[connection] = 1;
        }else{
            mConnectionState[connection] = 0;
        }
        logVerbose(AC,"AMClient::isConnectionNeedExec()-->OUT");
        return;
    }
}

int AMClient::getVolGainByStep(ama_connectID_t connectID,int volStep)
{
      logVerbose(AC,"AMClient::getVolGainByStep()-->IN volStep=",volStep);

      int volGain = AM_MUTE_DB;
      if((volStep>=0)&&(volStep<=MAX_VOL_STEP)){
          ama_connectInfo_t connectInfo = GET_CONNECTINFO_FROM_CONNECTID(connectID);
          std::map<ama_connectInfo_t,AMRoutingNameTable_s*>::iterator it;
          it = mRoutingMap.find(connectInfo);
          if(it != mRoutingMap.end()){
              logDebug(AC,"find the routingTable of connectInfo");
              volGain = ((mRoutingMap[connectInfo])->pVolGainTable)[volStep];
          }else{
              logError(AC,"can't find routingTable of connectInfo in mRoutingMap");
          }
      }else if(volStep == MUTE_VOL_STEP){
          volGain = AM_MUTE_DB;
      }else{
          logError(AC,"volStep is wrong");
      }

      logVerbose(AC,"AMClient::getVolGainByStep()-->OUT volGain=",volGain);
      return volGain;
}

void AMClient::logPrintAMRoutingNameTable(AMRoutingNameTable_s* pRoutingNameTable)
{
    logVerbose(DBG,"AMClient::logPrintAMRoutingNameTable()-->IN");

    logDebug(DBG,"pRoutingNameTable->connectionCntOfPath=",pRoutingNameTable->connectionCntOfPath);
    logDebug(DBG,"pRoutingNameTable->codecConnectStartIndex=",pRoutingNameTable->codecConnectStartIndex);

    for(int i=0;i < pRoutingNameTable->connectionCntOfPath;i++){
        logDebug(DBG,"pRoutingNameTable->conNameArray[",i,"].sourceName=",
        pRoutingNameTable->conNameArray[i].first);
        logDebug(DBG,"pRoutingNameTable->conNameArray[",i,"].sinkName=",
        pRoutingNameTable->conNameArray[i].second);
    }

    logVerbose(DBG,"AMClient::logPrintAMRoutingNameTable()-->OUT");
}

ama_Error_e AMClient::setBassGain(AMAC_Channel_t amacChannel,int bassGain)
{
    logVerbose(AC,"AMClient::setSpeakerBass()-->IN");

    int ret;
    logDebug(AC,"setBassByAMACCommandClient-->>");
    ret = setBassByAMACCommandClient(amacChannel,_BASS_FREQ_SET_,bassGain);
    if(0 == ret){
        logInfo(AC,"setBassByAMACCommandClient successed");
    }else{
        logError(AC,"setBassByAMACCommandClient failed ret:",ret);
        return E_UNKNOWN;
    }

    logVerbose(AC,"AMClient::setSpeakerBass()-->OUT");
    return E_OK;
}

ama_Error_e AMClient::setMidGain(AMAC_Channel_t amacChannel,int bassGain)
{
    logVerbose(AC,"AMClient::setMidGain()-->IN");

    int ret;
    logDebug(AC,"setMidByAMACCommandClient-->>");
    ret = setMidByAMACCommandClient(amacChannel,_MID_FREQ_SET_,bassGain);
    if(0 == ret){
        logInfo(AC,"setMidByAMACCommandClient successed");
    }else{
        logError(AC,"setBassByAMACCommandClient failed ret:",ret);
        return E_UNKNOWN;
    }

    logVerbose(AC,"AMClient::setMidGain()-->OUT");
    return E_OK;
}
ama_Error_e AMClient::setTrebleGain(AMAC_Channel_t amacChannel,int bassGain)
{
    logVerbose(AC,"AMClient::setTrebleGain()-->IN");

    int ret;
    logDebug(AC,"setTrebleByAMACCommandClient-->>");
    ret = setTrebleByAMACCommandClient(amacChannel,_TREBLE_FREQ_SET_,bassGain);
    if(0 == ret){
        logInfo(AC,"setTrebleByAMACCommandClient successed");
    }else{
        logError(AC,"setBassByAMACCommandClient failed ret:",ret);
        return E_UNKNOWN;
    }

    logVerbose(AC,"AMClient::setTrebleGain()-->OUT");
    return E_OK;
}

ama_Error_e AMClient::setChannelMuteState(AMAC_Channel_t amacChannel,bool isEnableMute)
{
    logVerbose(AC,"AMClient::setChannelMuteState()-->IN amacChannel:",amacChannel," isEnableMute:",isEnableMute);

    int ret;
    logDebug(AC,"setChannelMuteByAMACCommandClient-->>");
    if(isEnableMute){
        ret = setChannelMuteByAMACCommandClient(amacChannel,ENABLE_MUTE);
    }else{
        ret = setChannelMuteByAMACCommandClient(amacChannel,DISABLE_MUTE);
    }

    if(0 == ret){
        logInfo(AC,"setChannelMuteByAMACCommandClient successed");
    }else{
        logError(AC,"setChannelMuteByAMACCommandClient failed ret:",ret);
        return E_UNKNOWN;
    }

    logVerbose(AC,"AMClient::setChannelMuteState()-->OUT");
    return E_OK;
}

int AMClient::getBassGain(int BassSet,int volGain)
{
    logVerbose(AC,"AMClient::getBassGain()-->IN BassSet=",BassSet," volGain=",volGain);

    if(AM_MUTE_DB == volGain){
        volGain = MIN_AM_GAIN_DB;
    }

    if((volGain < MIN_AM_GAIN_DB) || (volGain > MAX_AM_GAIN_DB)){
        logError(AC,"Wrong volGain:",volGain);
        return 0;
    }

    int volSet = GainToVolSet[((volGain/10)+80)];
    int BassGain = BassGainTable[(BassSet+6)][volSet];
    logVerbose(AC,"AMClient::getBassGain()-->OUT BassGain=",BassGain);
    return BassGain;
}

int AMClient::getMidGain(int MidSet,int volGain)
{
    logVerbose(AC,"AMClient::getMidGain()-->IN MidSet=",MidSet," volGain=",volGain);

    if(AM_MUTE_DB == volGain){
        volGain = MIN_AM_GAIN_DB;
    }

    if((volGain < MIN_AM_GAIN_DB) || (volGain > MAX_AM_GAIN_DB)){
        logError(AC,"Wrong volGain:",volGain);
        return 0;
    }

    int volSet = GainToVolSet[((volGain/10)+80)];
    int MidGain = MidGainTable[(MidSet+6)][volSet];
    logVerbose(AC,"AMClient::getMidGain()-->OUT MidGain=",MidGain);
    return MidGain;
}

int AMClient::getTrebleGain(int TrebleSet,int volGain)
{
    logVerbose(AC,"AMClient::getTrebleGain()-->IN TrebleSet=",TrebleSet," volGain=",volGain);

    if(AM_MUTE_DB == volGain){
        volGain = MIN_AM_GAIN_DB;
    }

    if((volGain < MIN_AM_GAIN_DB) || (volGain > MAX_AM_GAIN_DB)){
        logError(AC,"Wrong volGain:",volGain);
        return 0;
    }

    int volSet = GainToVolSet[((volGain/10)+80)];
    int TrebleGain = TrebleGainTable[(TrebleSet+6)][volSet];
    logVerbose(AC,"AMClient::getTrebleGain()-->OUT TrebleGain=",TrebleGain);
    return TrebleGain;
}

int AMClient::setBalance(int balanceSet)
{
    logVerbose(AC,"AMClient::setBalance()-->IN balanceSet=",balanceSet);

    int ret;
    logDebug(AC,"setBalanceByAMACCommandClient-->>");
    ret = setBalanceByAMACCommandClient(Balance_BLTable[(balanceSet+6)],Balance_BRTable[(balanceSet+6)]);
    if(0 == ret){
        logInfo(AC,"setBalanceByAMACCommandClient successed");
        logInfoF(ADBG,"setBalance[%d] by AMACInterface!",balanceSet);
    }else{
        logError(AC,"setBalanceByAMACCommandClient failed ");
        return ret;
    }

    logVerbose(AC,"AMClient::setBalance()-->OUT");
    return E_OK;
}

int AMClient::setFader(int faderSet)
{
    logVerbose(AC,"AMClient::setFader()-->IN faderSet=",faderSet);

    int ret;
    logDebug(AC,"setFaderByAMACCommandClient-->>");
    ret = setFaderByAMACCommandClient(Fader_ffrTable[(faderSet+6)],Fader_freTable[(faderSet+6)]);
    if(0 == ret){
        logInfo(AC,"setFaderByAMACCommandClient successed");
        logInfoF(ADBG,"setFader[%d] by AMACInterface!",faderSet);
    }else{
        logError(AC,"setFaderByAMACCommandClient failed ");
        return ret;
    }

    logVerbose(AC,"AMClient::setFader()-->OUT");
    return E_OK;
}

void AMClient::debugShowAMSources(void)
{
    logVerbose(DBG,"AMClient::debugShowAMSources()-->IN");

    int index = 0;
    std::map<string,shared_ptr<AMSource>>::iterator it;
    for(it=mAMSourceMap.begin();it!=mAMSourceMap.end();++it,++index){
        logDebug(DBG,"AMSource index:",index);
        logDebug(DBG,"AMSourceID::", ToHEX((it->second)->sourceID()));
        logDebug(DBG,"AMSource Name:",(it->second)->name());
    }

    logVerbose(DBG,"AMClient::debugShowAMSources()-->OUT");
}
void AMClient::debugShowAMSinks(void)
{
    logVerbose(DBG,"AMClient::debugShowAMSinks()-->IN");

    int index = 0;
    std::map<string,shared_ptr<AMSink>>::iterator it;
    for(it=mAMSinkMap.begin();it!=mAMSinkMap.end();++it,++index){
        logDebug(DBG,"AMSink index:",index);
        logDebug(DBG,"AMSinkID::", ToHEX((it->second)->sinkID()));
        logDebug(DBG,"AMSink Name:",(it->second)->name());
    }

    logVerbose(DBG,"AMClient::debugShowAMSinks()-->OUT");
}

void AMClient::debugShowConRecord(void)
{
    logVerbose(DBG,"AMClient::debugShowConRecord()-->IN");

    int index = 0;
    std::map<AMConnectionIDPair,int>::iterator it;
    for(it=mConnectionState.begin();it!=mConnectionState.end();++it,++index){
        logDebug(DBG,"AMConRecord index:",index);
        logDebug(DBG,"AMConRecord AMSourceID:", ToHEX((it->first).first));
        logDebug(DBG,"AMConRecord AMSinkID:", ToHEX((it->first).second));
        logDebug(DBG,"AMConRecord conCount:",it->second);
    }

    logVerbose(DBG,"AMClient::debugShowConRecord()-->OUT");
}

void AMClient::debugShowRoutingMap(void)
{
    logVerbose(DBG,"AMClient::debugShowRoutingMap()-->IN");

    int index = 0;
    std::map<ama_connectInfo_t,AMRoutingNameTable_s*>::iterator it;
    for(it=mRoutingMap.begin();it!=mRoutingMap.end();++it,++index){
        logDebug(DBG,"RoutingMap index:",index);
        logDebug(DBG,"RoutingMap connectInfo:", ToHEX(it->first));
        logPrintAMRoutingNameTable(it->second);
    }

    logVerbose(DBG,"AMClient::debugShowRoutingMap()-->OUT");
}

void AMClient::attach(ConnectStaChangedObserver *pConnectStaChangedObserver)
{
    logVerbose(AC,"AMClient::attach()-->IN");

    mObserverList.push_back(pConnectStaChangedObserver);

    logVerbose(AC,"AMClient::attach()-->OUT");
}

void AMClient::detach(ConnectStaChangedObserver *pConnectStaChangedObserver)
{
    logVerbose(AC,"AMClient::detach()-->IN");

    mObserverList.remove(pConnectStaChangedObserver);

    logVerbose(AC,"AMClient::detach()-->OUT");

}

void AMClient::notifyConnectStaChanged(ama_connectID_t connectID,bool isConnect)
{
    logVerbose(AC,"AMClient::notifyConnectStaChanged()-->IN");
    logDebugF(AC,"connectID[%x];isConnect[%d] \n",connectID,isConnect);

    list<ConnectStaChangedObserver *>::iterator it;
    for(it=mObserverList.begin(); it!=mObserverList.end(); ++it){
        (*it)->OnConnectStaChanged(connectID,isConnect);
    }

    logVerbose(AC,"AMClient::notifyConnectStaChanged()-->OUT");
}


void AMClient::createRoutingMap(void)
{
    logVerbose(AC,"AMClient::createRoutingMap()-->IN");

    /******************************************************************************/
    /*                         IVI media to speaker                               */
    /******************************************************************************/
    //create ivi music source to speaker routing map
    iviMusicToSpeaker_RT.connectionCntOfPath = 2;
    iviMusicToSpeaker_RT.codecConnectStartIndex = 1;
    iviMusicToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviMusicToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviMusicToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviMusicToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviMusicToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviMusicToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviMusicToSpeaker_RT.gainList, iviMusicToSpeaker_gainList, sizeof(iviMusicToSpeaker_gainList));
    ama_connectInfo_t iviMusicToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_MUSIC,E_IVI_SPEAKER);
    mRoutingMap[iviMusicToSpeaker_RT_ci] = &iviMusicToSpeaker_RT;
    ama_connectInfo_t serviceMusicToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_MUSIC,E_IVI_SPEAKER);
    mRoutingMap[serviceMusicToSpeaker_RT_ci] = &iviMusicToSpeaker_RT;

    //create ivi video source to speaker routing map
    iviVideoToSpeaker_RT.connectionCntOfPath = 2;
    iviVideoToSpeaker_RT.codecConnectStartIndex = 1;
    iviVideoToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviVideoToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviVideoToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviVideoToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviVideoToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviVideoToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviVideoToSpeaker_RT.gainList, iviVideoToSpeaker_gainList, sizeof(iviVideoToSpeaker_gainList));
    ama_connectInfo_t iviVideoToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_VIDEO_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[iviVideoToSpeaker_RT_ci] = &iviVideoToSpeaker_RT;
    ama_connectInfo_t serviceVideoToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_VIDEO_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[serviceVideoToSpeaker_RT_ci] = &iviVideoToSpeaker_RT;

    //create ivi btMedia source to speaker routing map
    iviBTMediaToSpeaker_RT.connectionCntOfPath = 2;
    iviBTMediaToSpeaker_RT.codecConnectStartIndex = 1;
    iviBTMediaToSpeaker_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D6p");
    iviBTMediaToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviBTMediaToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviBTMediaToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviBTMediaToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviBTMediaToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviBTMediaToSpeaker_RT.gainList, iviBTMediaToSpeaker_gainList, sizeof(iviBTMediaToSpeaker_gainList));
    ama_connectInfo_t iviBtMediaToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_BT_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[iviBtMediaToSpeaker_RT_ci] = &iviBTMediaToSpeaker_RT;
    ama_connectInfo_t serviceBtMediaToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_BT_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[serviceBtMediaToSpeaker_RT_ci] = &iviBTMediaToSpeaker_RT;

    //create ivi carplayAudio source to speaker routing map
    iviCarplayAudioToSpeaker_RT.connectionCntOfPath = 2;
    iviCarplayAudioToSpeaker_RT.codecConnectStartIndex = 1;
    iviCarplayAudioToSpeaker_RT.conNameArray[0] = make_pair("carplayAudio","pa_pcmC0D6p");
    iviCarplayAudioToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviCarplayAudioToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviCarplayAudioToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviCarplayAudioToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviCarplayAudioToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviCarplayAudioToSpeaker_RT.gainList, iviCarplayAudioToSpeaker_gainList, sizeof(iviCarplayAudioToSpeaker_gainList));
    ama_connectInfo_t iviCarplayAudioToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_CARPLAY_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[iviCarplayAudioToSpeaker_RT_ci] = &iviCarplayAudioToSpeaker_RT;
    ama_connectInfo_t serviceCarplayAudioToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_CARPLAY_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[serviceCarplayAudioToSpeaker_RT_ci] = &iviCarplayAudioToSpeaker_RT;

    //create ivi Ipod source to speaker routing map
    iviIpodToSpeaker_RT.connectionCntOfPath = 2;
    iviIpodToSpeaker_RT.codecConnectStartIndex = 1;
    iviIpodToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviIpodToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviIpodToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviIpodToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviIpodToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviIpodToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviIpodToSpeaker_RT.gainList, iviIpodToSpeaker_gainList, sizeof(iviIpodToSpeaker_gainList));
    ama_connectInfo_t iviIpodToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_IPOD_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[iviIpodToSpeaker_RT_ci] = &iviIpodToSpeaker_RT;
    ama_connectInfo_t serviceIpodToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_IPOD_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[serviceIpodToSpeaker_RT_ci] = &iviIpodToSpeaker_RT;


    //create ivi 3THApp source to speaker routing map
    ivi3ThAppAudioToSpeaker_RT.connectionCntOfPath = 2;
    ivi3ThAppAudioToSpeaker_RT.codecConnectStartIndex = 1;
    ivi3ThAppAudioToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    ivi3ThAppAudioToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    ivi3ThAppAudioToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    ivi3ThAppAudioToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    ivi3ThAppAudioToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t ivi3ThAppAudioToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(ivi3ThAppAudioToSpeaker_RT.gainList, ivi3ThAppAudioToSpeaker_gainList, sizeof(ivi3ThAppAudioToSpeaker_gainList));
    ama_connectInfo_t ivi3ThAppAudioToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_3THAPP_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[ivi3ThAppAudioToSpeaker_RT_ci] = &ivi3ThAppAudioToSpeaker_RT;
    ama_connectInfo_t service3ThAppAudioToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_3THAPP_AUDIO,E_IVI_SPEAKER);
    mRoutingMap[service3ThAppAudioToSpeaker_RT_ci] = &ivi3ThAppAudioToSpeaker_RT;


    /******************************************************************************/
    /*                         IVI VRTTS to speaker                               */
    /******************************************************************************/
    //create ivi VRTTS source to speaker routing map
    iviVRTTSToSpeaker_RT.connectionCntOfPath = 2;
    iviVRTTSToSpeaker_RT.codecConnectStartIndex = 1;
    iviVRTTSToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviVRTTSToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviVRTTSToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviVRTTSToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviVRTTSToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviVRTTSToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviVRTTSToSpeaker_RT.gainList, iviVRTTSToSpeaker_gainList, sizeof(iviVRTTSToSpeaker_gainList));
    ama_connectInfo_t iviVRTTSToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_VR_TTS,E_IVI_SPEAKER);
    mRoutingMap[iviVRTTSToSpeaker_RT_ci] = &iviVRTTSToSpeaker_RT;
    ama_connectInfo_t serviceVRTTSToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_VR_TTS,E_IVI_SPEAKER);
    mRoutingMap[serviceVRTTSToSpeaker_RT_ci] = &iviVRTTSToSpeaker_RT;


    /******************************************************************************/
    /*                         IVI SIRI to speaker                               */
    /******************************************************************************/
    //create ivi VRTTS source to speaker routing map
    iviCarplaySiriToSpeaker_RT.connectionCntOfPath = 2;
    iviCarplaySiriToSpeaker_RT.codecConnectStartIndex = 1;
    iviCarplaySiriToSpeaker_RT.conNameArray[0] = make_pair("carplaySiri","pa_pcmC0D6p");
    iviCarplaySiriToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviCarplaySiriToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviCarplaySiriToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviCarplaySiriToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviCarplaySiriToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviCarplaySiriToSpeaker_RT.gainList, iviCarplaySiriToSpeaker_gainList, sizeof(iviCarplaySiriToSpeaker_gainList));
    ama_connectInfo_t iviCarplaySiriToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_CARPLAY_SIRI,E_IVI_SPEAKER);
    mRoutingMap[iviCarplaySiriToSpeaker_RT_ci] = &iviCarplaySiriToSpeaker_RT;
    ama_connectInfo_t serviceCarplaySiriToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_CARPLAY_SIRI,E_IVI_SPEAKER);
    mRoutingMap[serviceCarplaySiriToSpeaker_RT_ci] = &iviCarplaySiriToSpeaker_RT;

    /******************************************************************************/
    /*                         IVI VRTTS to speaker                               */
    /******************************************************************************/
    //create ivi SMSTTS source to speaker routing map
    iviSMSTTSToSpeaker_RT.connectionCntOfPath = 2;
    iviSMSTTSToSpeaker_RT.codecConnectStartIndex = 1;
    iviSMSTTSToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviSMSTTSToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    iviSMSTTSToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviSMSTTSToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviSMSTTSToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviSMSTTSToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviSMSTTSToSpeaker_RT.gainList, iviSMSTTSToSpeaker_gainList, sizeof(iviSMSTTSToSpeaker_gainList));
    ama_connectInfo_t iviSMSTTSToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_SMS_TTS,E_IVI_SPEAKER);
    mRoutingMap[iviSMSTTSToSpeaker_RT_ci] = &iviSMSTTSToSpeaker_RT;
    ama_connectInfo_t serviceSMSTTSToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_SMS_TTS,E_IVI_SPEAKER);
    mRoutingMap[serviceSMSTTSToSpeaker_RT_ci] = &iviSMSTTSToSpeaker_RT;

    /******************************************************************************/
    /*                       IVI phone to speaker FL                              */
    /******************************************************************************/
    //create ivi btCall source to speaker routing map
    iviBTCallToSpeaker_RT.connectionCntOfPath = 2;
    iviBTCallToSpeaker_RT.codecConnectStartIndex = 1;
    iviBTCallToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviBTCallToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_FL");
    //gain info
    iviBTCallToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviBTCallToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    //iviBTCallToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    iviBTCallToSpeaker_RT.pVolGainTable = VolSetToGain_BT;
    AMAC_Gain_t iviBTCallToSpeaker_gainList[LONG_GAIN_NUM] = {0x03FD,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviBTCallToSpeaker_RT.gainList, iviBTCallToSpeaker_gainList, sizeof(iviBTCallToSpeaker_gainList));
    ama_connectInfo_t iviBtCallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_BT_CALL,E_IVI_SPEAKER);
    mRoutingMap[iviBtCallToSpeaker_RT_ci] = &iviBTCallToSpeaker_RT;
    ama_connectInfo_t serviceBtCallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_BT_CALL,E_IVI_SPEAKER);
    mRoutingMap[serviceBtCallToSpeaker_RT_ci] = &iviBTCallToSpeaker_RT;

    //create ivi CallRing source to speaker routing map
    iviCallRingToSpeaker_RT.connectionCntOfPath = 2;
    iviCallRingToSpeaker_RT.codecConnectStartIndex = 1;
    iviCallRingToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviCallRingToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    //gain info
    iviCallRingToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviCallRingToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviCallRingToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviCallRingToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviCallRingToSpeaker_RT.gainList, iviCallRingToSpeaker_gainList, sizeof(iviCallRingToSpeaker_gainList));
    ama_connectInfo_t iviCallRingToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_CALL_RING,E_IVI_SPEAKER);
    mRoutingMap[iviCallRingToSpeaker_RT_ci] = &iviCallRingToSpeaker_RT;
    ama_connectInfo_t serviceCallRingToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_CALL_RING,E_IVI_SPEAKER);
    mRoutingMap[serviceCallRingToSpeaker_RT_ci] = &iviCallRingToSpeaker_RT;

    //create ivi CallRing source to speaker routing map
    iviCarplayCallToSpeaker_RT.connectionCntOfPath = 2;
    iviCarplayCallToSpeaker_RT.codecConnectStartIndex = 1;
    iviCarplayCallToSpeaker_RT.conNameArray[0] = make_pair("carplayCall","pa_pcmC0D6p");
    iviCarplayCallToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_PRIMARY");
    //gain info
    iviCarplayCallToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviCarplayCallToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviCarplayCallToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviCarplayCallToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviCarplayCallToSpeaker_RT.gainList, iviCarplayCallToSpeaker_gainList, sizeof(iviCarplayCallToSpeaker_gainList));
    ama_connectInfo_t iviCarplayCallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_CARPLAY_CALL,E_IVI_SPEAKER);
    mRoutingMap[iviCarplayCallToSpeaker_RT_ci] = &iviCarplayCallToSpeaker_RT;
    ama_connectInfo_t serviceCarplayCallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_CARPLAY_CALL,E_IVI_SPEAKER);
    mRoutingMap[serviceCarplayCallToSpeaker_RT_ci] = &iviCarplayCallToSpeaker_RT;

    /******************************************************************************/
    /*                     IVI ?source to L-RSE heakset                           */
    /******************************************************************************/
    //create ivi music source to L-RSE heakset routing map
    iviMusicToLHeadset_RT.connectionCntOfPath = 2;
    iviMusicToLHeadset_RT.codecConnectStartIndex = 1;
    iviMusicToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviMusicToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_RSE_L");
    //gain info
    iviMusicToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    iviMusicToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    iviMusicToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t iviMusicToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(iviMusicToLHeadset_RT.gainList, iviMusicToLHeadset_gainList, sizeof(iviMusicToLHeadset_gainList));
    ama_connectInfo_t iviMusicToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_MUSIC,E_RSE_L_HEADSET);
    mRoutingMap[iviMusicToLHeadset_RT_ci] = &iviMusicToLHeadset_RT;
    ama_connectInfo_t serviceMusicToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_MUSIC,E_RSE_L_HEADSET);
    mRoutingMap[serviceMusicToLHeadset_RT_ci] = &iviMusicToLHeadset_RT;

    //create ivi video source to L-RSE heakset routing map
    iviVideoToLHeadset_RT.connectionCntOfPath = 2;
    iviVideoToLHeadset_RT.codecConnectStartIndex = 1;
    iviVideoToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviVideoToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_RSE_L");
    //gain info
    iviVideoToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    iviVideoToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    iviVideoToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t iviVideoToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(iviVideoToLHeadset_RT.gainList, iviVideoToLHeadset_gainList, sizeof(iviVideoToLHeadset_gainList));
    ama_connectInfo_t iviVideoToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_VIDEO_AUDIO,E_RSE_L_HEADSET);
    mRoutingMap[iviVideoToLHeadset_RT_ci] = &iviVideoToLHeadset_RT;
    ama_connectInfo_t serviceVideoToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_VIDEO_AUDIO,E_RSE_L_HEADSET);
    mRoutingMap[serviceVideoToLHeadset_RT_ci] = &iviVideoToLHeadset_RT;

    //create ivi BtMusic source to L-RSE heakset routing map
    iviBTMediaToLHeadset_RT.connectionCntOfPath = 2;
    iviBTMediaToLHeadset_RT.codecConnectStartIndex = 1;
    iviBTMediaToLHeadset_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D6p");
    iviBTMediaToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_RSE_L");
    //gain info
    iviBTMediaToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    iviBTMediaToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    iviBTMediaToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t iviBTMediaToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(iviBTMediaToLHeadset_RT.gainList, iviBTMediaToLHeadset_gainList, sizeof(iviBTMediaToLHeadset_gainList));
    ama_connectInfo_t iviBTMediaToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_BT_AUDIO,E_RSE_L_HEADSET);
    mRoutingMap[iviBTMediaToLHeadset_RT_ci] = &iviBTMediaToLHeadset_RT;
    ama_connectInfo_t serviceBTMediaToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_BT_AUDIO,E_RSE_L_HEADSET);
    mRoutingMap[serviceBTMediaToLHeadset_RT_ci] = &iviBTMediaToLHeadset_RT;

    /******************************************************************************/
    /*                     IVI ?source to R-RSE heakset                           */
    /******************************************************************************/
    //create ivi music source to L-RSE heakset routing map
    iviMusicToRHeadset_RT.connectionCntOfPath = 2;
    iviMusicToRHeadset_RT.codecConnectStartIndex = 1;
    iviMusicToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviMusicToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_RSE_R");
    //gain info
    iviMusicToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    iviMusicToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    iviMusicToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t iviMusicToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(iviMusicToRHeadset_RT.gainList, iviMusicToRHeadset_gainList, sizeof(iviMusicToRHeadset_gainList));
    ama_connectInfo_t iviMusicToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_MUSIC,E_RSE_R_HEADSET);
    mRoutingMap[iviMusicToRHeadset_RT_ci] = &iviMusicToRHeadset_RT;
    ama_connectInfo_t serviceMusicToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_MUSIC,E_RSE_R_HEADSET);
    mRoutingMap[serviceMusicToRHeadset_RT_ci] = &iviMusicToRHeadset_RT;

    //create ivi video source to L-RSE heakset routing map
    iviVideoToRHeadset_RT.connectionCntOfPath = 2;
    iviVideoToRHeadset_RT.codecConnectStartIndex = 1;
    iviVideoToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D6p");
    iviVideoToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_RSE_R");
    //gain info
    iviVideoToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    iviVideoToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    iviVideoToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t iviVideoToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(iviVideoToRHeadset_RT.gainList, iviVideoToRHeadset_gainList, sizeof(iviVideoToRHeadset_gainList));
    ama_connectInfo_t iviVideoToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_VIDEO_AUDIO,E_RSE_R_HEADSET);
    mRoutingMap[iviVideoToRHeadset_RT_ci] = &iviVideoToRHeadset_RT;
    ama_connectInfo_t serviceVideoToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_VIDEO_AUDIO,E_RSE_R_HEADSET);
    mRoutingMap[serviceVideoToRHeadset_RT_ci] = &iviVideoToRHeadset_RT;

    //create ivi BtMusic source to L-RSE heakset routing map
    iviBTMediaToRHeadset_RT.connectionCntOfPath = 2;
    iviBTMediaToRHeadset_RT.codecConnectStartIndex = 1;
    iviBTMediaToRHeadset_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D6p");
    iviBTMediaToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_IIS0","SINK_RSE_R");
    //gain info
    iviBTMediaToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    iviBTMediaToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    iviBTMediaToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t iviBTMediaToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(iviBTMediaToRHeadset_RT.gainList, iviBTMediaToRHeadset_gainList, sizeof(iviBTMediaToRHeadset_gainList));
    ama_connectInfo_t iviBTMediaToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_BT_AUDIO,E_RSE_R_HEADSET);
    mRoutingMap[iviBTMediaToRHeadset_RT_ci] = &iviBTMediaToRHeadset_RT;
    ama_connectInfo_t serviceBTMediaToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_BT_AUDIO,E_RSE_R_HEADSET);
    mRoutingMap[serviceBTMediaToRHeadset_RT_ci] = &iviBTMediaToRHeadset_RT;

    /******************************************************************************/
    /*                          IVI Tbox to speaker                               */
    /******************************************************************************/
    //create BCall source to speaker routing map
    iviBCallToSpeaker_RT.connectionCntOfPath = 1;
    iviBCallToSpeaker_RT.codecConnectStartIndex = 0;
    iviBCallToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_AIN01","SINK_PRIMARY");
    //gain info
    iviBCallToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviBCallToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviBCallToSpeaker_RT.pVolGainTable = VolSetToGain_TE;
    AMAC_Gain_t iviBCallToSpeaker_gainList[LONG_GAIN_NUM] = {0x02D1,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviBCallToSpeaker_RT.gainList, iviBCallToSpeaker_gainList, sizeof(iviBCallToSpeaker_gainList));
    ama_connectInfo_t iviBCallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_B_CALL,E_IVI_SPEAKER);
    mRoutingMap[iviBCallToSpeaker_RT_ci] = &iviBCallToSpeaker_RT;
    ama_connectInfo_t serviceBCallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_B_CALL,E_IVI_SPEAKER);
    mRoutingMap[serviceBCallToSpeaker_RT_ci] = &iviBCallToSpeaker_RT;


    //create ICall source to speaker routing map
    iviICallToSpeaker_RT.connectionCntOfPath = 1;
    iviICallToSpeaker_RT.codecConnectStartIndex = 0;
    iviICallToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_AIN01","SINK_PRIMARY");
    //gain info
    iviICallToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviICallToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviICallToSpeaker_RT.pVolGainTable = VolSetToGain_TE;
    AMAC_Gain_t iviICallToSpeaker_gainList[LONG_GAIN_NUM] = {0x02D1,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviICallToSpeaker_RT.gainList, iviICallToSpeaker_gainList, sizeof(iviICallToSpeaker_gainList));
    ama_connectInfo_t iviICallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_I_CALL,E_IVI_SPEAKER);
    mRoutingMap[iviICallToSpeaker_RT_ci] = &iviICallToSpeaker_RT;
    ama_connectInfo_t serviceICallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_I_CALL,E_IVI_SPEAKER);
    mRoutingMap[serviceICallToSpeaker_RT_ci] = &iviICallToSpeaker_RT;

    //create ECall source to speaker routing map
    iviECallToSpeaker_RT.connectionCntOfPath = 1;
    iviECallToSpeaker_RT.codecConnectStartIndex = 0;
    iviECallToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_AIN01","SINK_PRIMARY");
    //gain info
    iviECallToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviECallToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviECallToSpeaker_RT.pVolGainTable = VolSetToGain_TE;
    AMAC_Gain_t iviECallToSpeaker_gainList[LONG_GAIN_NUM] = {0x02D1,0x0503,0x0503,0x0503,0x0503};
    memcpy(iviECallToSpeaker_RT.gainList, iviECallToSpeaker_gainList, sizeof(iviECallToSpeaker_gainList));
    ama_connectInfo_t iviECallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_E_CALL,E_IVI_SPEAKER);
    mRoutingMap[iviECallToSpeaker_RT_ci] = &iviECallToSpeaker_RT;
    ama_connectInfo_t serviceECallToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_E_CALL,E_IVI_SPEAKER);
    mRoutingMap[serviceECallToSpeaker_RT_ci] = &iviECallToSpeaker_RT;

    /******************************************************************************/
    /*                          IVI tuner to speaker                              */
    /******************************************************************************/
    //create ivi tuner source to speaker routing map
    iviTunerToSpeaker_RT.connectionCntOfPath = 1;
    iviTunerToSpeaker_RT.codecConnectStartIndex = 0;
    iviTunerToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    //gain info
    iviTunerToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviTunerToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviTunerToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviTunerToSpeaker_gainList[LONG_GAIN_NUM] = {0x01E0,0x01FE,0x01FE,0x01FE,0x01FE};
    memcpy(iviTunerToSpeaker_RT.gainList, iviTunerToSpeaker_gainList, sizeof(iviTunerToSpeaker_gainList));
    ama_connectInfo_t iviTunerToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_TUNER,E_IVI_SPEAKER);
    mRoutingMap[iviTunerToSpeaker_RT_ci] = &iviTunerToSpeaker_RT;
    ama_connectInfo_t serviceTunerToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_TUNER,E_IVI_SPEAKER);
    mRoutingMap[serviceTunerToSpeaker_RT_ci] = &iviTunerToSpeaker_RT;

    //create ivi FM source to speaker routing map
    iviFMToSpeaker_RT.connectionCntOfPath = 1;
    iviFMToSpeaker_RT.codecConnectStartIndex = 0;
    iviFMToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    //gain info
    iviFMToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviFMToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviFMToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviFMToSpeaker_gainList[LONG_GAIN_NUM] = {0x01E0,0x01FE,0x01FE,0x01FE,0x01FE};
    memcpy(iviFMToSpeaker_RT.gainList, iviFMToSpeaker_gainList, sizeof(iviFMToSpeaker_gainList));
    ama_connectInfo_t iviFMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_FM,E_IVI_SPEAKER);
    mRoutingMap[iviFMToSpeaker_RT_ci] = &iviFMToSpeaker_RT;
    ama_connectInfo_t serviceFMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_FM,E_IVI_SPEAKER);
    mRoutingMap[serviceFMToSpeaker_RT_ci] = &iviFMToSpeaker_RT;

    //create ivi AM source to speaker routing map
    iviAMToSpeaker_RT.connectionCntOfPath = 1;
    iviAMToSpeaker_RT.codecConnectStartIndex = 0;
    iviAMToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    //gain info
    iviAMToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviAMToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviAMToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviAMToSpeaker_gainList[LONG_GAIN_NUM] = {0x0662,0x0100,0x0100,0x0100,0x0100};
    memcpy(iviAMToSpeaker_RT.gainList, iviAMToSpeaker_gainList, sizeof(iviAMToSpeaker_gainList));
    ama_connectInfo_t iviAMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_AM,E_IVI_SPEAKER);
    mRoutingMap[iviAMToSpeaker_RT_ci] = &iviAMToSpeaker_RT;
    ama_connectInfo_t serviceAMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_AM,E_IVI_SPEAKER);
    mRoutingMap[serviceAMToSpeaker_RT_ci] = &iviAMToSpeaker_RT;

    //create ivi FM Second source to speaker routing map
    iviFMSecToSpeaker_RT.connectionCntOfPath = 1;
    iviFMSecToSpeaker_RT.codecConnectStartIndex = 0;
    iviFMSecToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_SEC","SINK_PRIMARY");
    //gain info
    iviFMSecToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    iviFMSecToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    iviFMSecToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t iviFMSecToSpeaker_gainList[LONG_GAIN_NUM] = {0x01E0,0x01FE,0x01FE,0x01FE,0x01FE};
    memcpy(iviFMSecToSpeaker_RT.gainList, iviFMSecToSpeaker_gainList, sizeof(iviFMSecToSpeaker_gainList));
    ama_connectInfo_t iviFMSecToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_FM_SEC,E_IVI_SPEAKER);
    mRoutingMap[iviFMSecToSpeaker_RT_ci] = &iviFMSecToSpeaker_RT;
    ama_connectInfo_t serviceFMSecToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_FM_SEC,E_IVI_SPEAKER);
    mRoutingMap[serviceFMSecToSpeaker_RT_ci] = &iviFMSecToSpeaker_RT;

    /******************************************************************************/
    /*                       L-RSE tuner to L-headset                             */
    /******************************************************************************/
    //create L-RSE tuner source to L-headset routing map
    LRSETunerToLHeadset_RT.connectionCntOfPath = 1;
    LRSETunerToLHeadset_RT.codecConnectStartIndex = 0;
    LRSETunerToLHeadset_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_RSE_L");
    ama_connectInfo_t LRSETunerToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_TUNER,E_RSE_L_HEADSET);
    //gain info
    LRSETunerToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    LRSETunerToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSETunerToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSETunerToLHeadset_gainList[SHORT_GAIN_NUM] = {0x01E0,0x01FE,0x01FE};
    memcpy(LRSETunerToLHeadset_RT.gainList, LRSETunerToLHeadset_gainList, sizeof(LRSETunerToLHeadset_gainList));
    mRoutingMap[LRSETunerToLHeadset_RT_ci] = &LRSETunerToLHeadset_RT;

    //create L-RSE FM source to L-headset routing map
    LRSEFMToLHeadset_RT.connectionCntOfPath = 1;
    LRSEFMToLHeadset_RT.codecConnectStartIndex = 0;
    LRSEFMToLHeadset_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_RSE_L");
    ama_connectInfo_t LRSEFMToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_FM,E_RSE_L_HEADSET);
    //gain info
    LRSEFMToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    LRSEFMToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEFMToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEFMToLHeadset_gainList[SHORT_GAIN_NUM] = {0x01E0,0x01FE,0x01FE};
    memcpy(LRSEFMToLHeadset_RT.gainList, LRSEFMToLHeadset_gainList, sizeof(LRSEFMToLHeadset_gainList));
    mRoutingMap[LRSEFMToLHeadset_RT_ci] = &LRSEFMToLHeadset_RT;

    //create L-RSE AM source to L-headset routing map
    LRSEAMToLHeadset_RT.connectionCntOfPath = 1;
    LRSEAMToLHeadset_RT.codecConnectStartIndex = 0;
    LRSEAMToLHeadset_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_RSE_L");
    ama_connectInfo_t LRSEAMToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_AM,E_RSE_L_HEADSET);
    //gain info
    LRSEAMToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    LRSEAMToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEAMToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEAMToLHeadset_gainList[SHORT_GAIN_NUM] = {0x0662,0x0100,0x0100};
    memcpy(LRSEAMToLHeadset_RT.gainList, LRSEAMToLHeadset_gainList, sizeof(LRSEAMToLHeadset_gainList));
    mRoutingMap[LRSEAMToLHeadset_RT_ci] = &LRSEAMToLHeadset_RT;

    /******************************************************************************/
    /*                       L-RSE tuner to speaker                               */
    /******************************************************************************/
    //create L-RSE FM source to speaker routing map
    LRSEFMToSpeaker_RT.connectionCntOfPath = 1;
    LRSEFMToSpeaker_RT.codecConnectStartIndex = 0;
    LRSEFMToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    ama_connectInfo_t LRSEFMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_FM,E_IVI_SPEAKER);
    //gain info
    LRSEFMToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    LRSEFMToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    LRSEFMToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t LRSEFMToSpeaker_gainList[LONG_GAIN_NUM] = {0x01E0,0x01FE,0x01FE,0x01FE,0x01FE};
    memcpy(LRSEFMToSpeaker_RT.gainList, LRSEFMToSpeaker_gainList, sizeof(LRSEFMToSpeaker_gainList));
    mRoutingMap[LRSEFMToSpeaker_RT_ci] = &LRSEFMToSpeaker_RT;
    //create L-RSE AM source to speaker routing map
    LRSEAMToSpeaker_RT.connectionCntOfPath = 1;
    LRSEAMToSpeaker_RT.codecConnectStartIndex = 0;
    LRSEAMToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    ama_connectInfo_t LRSEAMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_AM,E_IVI_SPEAKER);
    //gain info
    LRSEAMToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    LRSEAMToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    LRSEAMToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t LRSEAMToSpeaker_gainList[LONG_GAIN_NUM] = {0x0662,0x0100,0x0100,0x0100,0x0100};
    memcpy(LRSEAMToSpeaker_RT.gainList, LRSEAMToSpeaker_gainList, sizeof(LRSEAMToSpeaker_gainList));
    mRoutingMap[LRSEAMToSpeaker_RT_ci] = &LRSEAMToSpeaker_RT;

    /******************************************************************************/
    /*                       R-RSE tuner to R-headset                             */
    /******************************************************************************/
    //create R-RSE tuner source to R-headset routing map
    RRSETunerToRHeadset_RT.connectionCntOfPath = 1;
    RRSETunerToRHeadset_RT.codecConnectStartIndex = 0;
    RRSETunerToRHeadset_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_RSE_R");
    ama_connectInfo_t RRSETunerToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_TUNER,E_RSE_R_HEADSET);
    //gain info
    RRSETunerToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    RRSETunerToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSETunerToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSETunerToRHeadset_gainList[SHORT_GAIN_NUM] = {0x01E0,0x01FE,0x01FE};
    memcpy(RRSETunerToRHeadset_RT.gainList, RRSETunerToRHeadset_gainList, sizeof(RRSETunerToRHeadset_gainList));
    mRoutingMap[RRSETunerToRHeadset_RT_ci] = &RRSETunerToRHeadset_RT;

    //create R-RSE FM source to R-headset routing map
    RRSEFMToRHeadset_RT.connectionCntOfPath = 1;
    RRSEFMToRHeadset_RT.codecConnectStartIndex = 0;
    RRSEFMToRHeadset_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_RSE_R");
    ama_connectInfo_t RRSEFMToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_FM,E_RSE_R_HEADSET);
    //gain info
    RRSEFMToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    RRSEFMToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEFMToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEFMToRHeadset_gainList[SHORT_GAIN_NUM] = {0x01E0,0x01FE,0x01FE};
    memcpy(RRSEFMToRHeadset_RT.gainList, RRSEFMToRHeadset_gainList, sizeof(RRSEFMToRHeadset_gainList));
    mRoutingMap[RRSEFMToRHeadset_RT_ci] = &RRSEFMToRHeadset_RT;

    //create R-RSE AM source to R-headset routing map
    RRSEAMToRHeadset_RT.connectionCntOfPath = 1;
    RRSEAMToRHeadset_RT.codecConnectStartIndex = 0;
    RRSEAMToRHeadset_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_RSE_R");
    ama_connectInfo_t RRSEAMToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_AM,E_RSE_R_HEADSET);
    //gain info
    RRSEAMToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    RRSEAMToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEAMToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEAMToRHeadset_gainList[SHORT_GAIN_NUM] = {0x0662,0x0100,0x0100};
    memcpy(RRSEAMToRHeadset_RT.gainList, RRSEAMToRHeadset_gainList, sizeof(RRSEAMToRHeadset_gainList));
    mRoutingMap[RRSEAMToRHeadset_RT_ci] = &RRSEAMToRHeadset_RT;

    /******************************************************************************/
    /*                       R-RSE tuner to speaker                               */
    /******************************************************************************/
    //create L-RSE FM source to speaker routing map
    RRSEFMToSpeaker_RT.connectionCntOfPath = 1;
    RRSEFMToSpeaker_RT.codecConnectStartIndex = 0;
    RRSEFMToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    ama_connectInfo_t RRSEFMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_FM,E_IVI_SPEAKER);
    //gain info
    RRSEFMToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    RRSEFMToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    RRSEFMToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t RRSEFMToSpeaker_gainList[LONG_GAIN_NUM] = {0x01E0,0x01FE,0x01FE,0x01FE,0x01FE};
    memcpy(RRSEFMToSpeaker_RT.gainList, RRSEFMToSpeaker_gainList, sizeof(RRSEFMToSpeaker_gainList));
    mRoutingMap[RRSEFMToSpeaker_RT_ci] = &RRSEFMToSpeaker_RT;
    //create L-RSE AM source to speaker routing map
    RRSEAMToSpeaker_RT.connectionCntOfPath = 1;
    RRSEAMToSpeaker_RT.codecConnectStartIndex = 0;
    RRSEAMToSpeaker_RT.conNameArray[0] = make_pair("SOURCE_FM_AM_IN","SINK_PRIMARY");
    ama_connectInfo_t RRSEAMToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_AM,E_IVI_SPEAKER);
    //gain info
    RRSEAMToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    RRSEAMToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    RRSEAMToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t RRSEAMToSpeaker_gainList[LONG_GAIN_NUM] = {0x0662,0x0100,0x0100,0x0100,0x0100};
    memcpy(RRSEAMToSpeaker_RT.gainList, RRSEAMToSpeaker_gainList, sizeof(RRSEAMToSpeaker_gainList));
    mRoutingMap[RRSEAMToSpeaker_RT_ci] = &RRSEAMToSpeaker_RT;

    /******************************************************************************/
    /*                    L-RSE ?sources to L-headset                             */
    /******************************************************************************/
    //create L-RSE music source to L-headset routing map
    LRSEMusicToLHeadset_RT.connectionCntOfPath = 2;
    LRSEMusicToLHeadset_RT.codecConnectStartIndex = 1;
    LRSEMusicToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pl");
    LRSEMusicToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_RSE_L");
    ama_connectInfo_t LRSEMusicToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_MUSIC,E_RSE_L_HEADSET);
    //gain info
    LRSEMusicToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    LRSEMusicToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEMusicToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEMusicToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(LRSEMusicToLHeadset_RT.gainList, LRSEMusicToLHeadset_gainList, sizeof(LRSEMusicToLHeadset_gainList));
    mRoutingMap[LRSEMusicToLHeadset_RT_ci] = &LRSEMusicToLHeadset_RT;

    //create L-RSE tuner source to L-headset routing map
    LRSEVideoToLHeadset_RT.connectionCntOfPath = 2;
    LRSEVideoToLHeadset_RT.codecConnectStartIndex = 1;
    LRSEVideoToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pl");
    LRSEVideoToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_RSE_L");
    ama_connectInfo_t LRSEVideoToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_VIDEO_AUDIO,E_RSE_L_HEADSET);
    //gain info
    LRSEVideoToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    LRSEVideoToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEVideoToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEVideoToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(LRSEVideoToLHeadset_RT.gainList, LRSEVideoToLHeadset_gainList, sizeof(LRSEVideoToLHeadset_gainList));
    mRoutingMap[LRSEVideoToLHeadset_RT_ci] = &LRSEVideoToLHeadset_RT;

    //create L-RSE BtMedia source to L-headset routing map
    LRSEBTMediaToLHeadset_RT.connectionCntOfPath = 2;
    LRSEBTMediaToLHeadset_RT.codecConnectStartIndex = 1;
    LRSEBTMediaToLHeadset_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D4pl");
    LRSEBTMediaToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_RSE_L");
    ama_connectInfo_t LRSEBTMediaToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_BT_AUDIO,E_RSE_L_HEADSET);
    //gain info
    LRSEBTMediaToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    LRSEBTMediaToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEBTMediaToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEBTMediaToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(LRSEBTMediaToLHeadset_RT.gainList, LRSEBTMediaToLHeadset_gainList, sizeof(LRSEBTMediaToLHeadset_gainList));
    mRoutingMap[LRSEBTMediaToLHeadset_RT_ci] = &LRSEBTMediaToLHeadset_RT;

    /******************************************************************************/
    /*                    L-RSE ?sources to R-headset                             */
    /******************************************************************************/
    //create L-RSE music source to R-headset routing map
    LRSEMusicToRHeadset_RT.connectionCntOfPath = 2;
    LRSEMusicToRHeadset_RT.codecConnectStartIndex = 1;
    LRSEMusicToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pl");
    LRSEMusicToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_RSE_R");
    ama_connectInfo_t LRSEMusicToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_MUSIC,E_RSE_R_HEADSET);
    //gain info
    LRSEMusicToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    LRSEMusicToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEMusicToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEMusicToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(LRSEMusicToRHeadset_RT.gainList, LRSEMusicToRHeadset_gainList, sizeof(LRSEMusicToRHeadset_gainList));
    mRoutingMap[LRSEMusicToRHeadset_RT_ci] = &LRSEMusicToRHeadset_RT;

    //create L-RSE Video source to R-headset routing map
    LRSEVideoToRHeadset_RT.connectionCntOfPath = 2;
    LRSEVideoToRHeadset_RT.codecConnectStartIndex = 1;
    LRSEVideoToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pl");
    LRSEVideoToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_RSE_R");
    ama_connectInfo_t LRSEVideoToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_VIDEO_AUDIO,E_RSE_R_HEADSET);
    //gain info
    LRSEVideoToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    LRSEVideoToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEVideoToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEVideoToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(LRSEVideoToRHeadset_RT.gainList, LRSEVideoToRHeadset_gainList, sizeof(LRSEVideoToRHeadset_gainList));
    mRoutingMap[LRSEVideoToRHeadset_RT_ci] = &LRSEVideoToRHeadset_RT;

    //create L-RSE BtMedia source to R-headset routing map
    LRSEBTMediaToRHeadset_RT.connectionCntOfPath = 2;
    LRSEBTMediaToRHeadset_RT.codecConnectStartIndex = 1;
    LRSEBTMediaToRHeadset_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D4pl");
    LRSEBTMediaToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_RSE_R");
    ama_connectInfo_t LRSEBTMediaToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_BT_AUDIO,E_RSE_R_HEADSET);
    //gain info
    LRSEBTMediaToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    LRSEBTMediaToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    LRSEBTMediaToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t LRSEBTMediaToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(LRSEBTMediaToRHeadset_RT.gainList, LRSEBTMediaToRHeadset_gainList, sizeof(LRSEBTMediaToRHeadset_gainList));
    mRoutingMap[LRSEBTMediaToRHeadset_RT_ci] = &LRSEBTMediaToRHeadset_RT;

    /******************************************************************************/
    /*                    L-RSE ?sources to speaker                               */
    /******************************************************************************/
    //create L-RSE music source to speaker routing map
    LRSEMusicToSpeaker_RT.connectionCntOfPath = 2;
    LRSEMusicToSpeaker_RT.codecConnectStartIndex = 1;
    LRSEMusicToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pl");
    LRSEMusicToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_PRIMARY");
    ama_connectInfo_t LRSEMusicToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_MUSIC,E_IVI_SPEAKER);
    //gain info
    LRSEMusicToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    LRSEMusicToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    LRSEMusicToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t LRSEMusicToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(LRSEMusicToSpeaker_RT.gainList, LRSEMusicToSpeaker_gainList, sizeof(LRSEMusicToSpeaker_gainList));
    mRoutingMap[LRSEMusicToSpeaker_RT_ci] = &LRSEMusicToSpeaker_RT;

    //create L-RSE Video source to speaker routing map
    LRSEVideoToSpeaker_RT.connectionCntOfPath = 2;
    LRSEVideoToSpeaker_RT.codecConnectStartIndex = 1;
    LRSEVideoToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pl");
    LRSEVideoToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_PRIMARY");
    ama_connectInfo_t LRSEVideoToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_VIDEO_AUDIO,E_IVI_SPEAKER);
    //gain info
    LRSEVideoToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    LRSEVideoToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    LRSEVideoToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t LRSEVideoToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(LRSEVideoToSpeaker_RT.gainList, LRSEVideoToSpeaker_gainList, sizeof(LRSEVideoToSpeaker_gainList));
    mRoutingMap[LRSEVideoToSpeaker_RT_ci] = &LRSEVideoToSpeaker_RT;

    //create L-RSE BtMedia source to speaker routing map
    LRSEBTMediaToSpeaker_RT.connectionCntOfPath = 2;
    LRSEBTMediaToSpeaker_RT.codecConnectStartIndex = 1;
    LRSEBTMediaToSpeaker_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D4pl");
    LRSEBTMediaToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_1","SINK_PRIMARY");
    ama_connectInfo_t LRSEBTMediaToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_BT_AUDIO,E_IVI_SPEAKER);
    //gain info
    LRSEBTMediaToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    LRSEBTMediaToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    LRSEBTMediaToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t LRSEBTMediaToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(LRSEBTMediaToSpeaker_RT.gainList, LRSEBTMediaToSpeaker_gainList, sizeof(LRSEBTMediaToSpeaker_gainList));
    mRoutingMap[LRSEBTMediaToSpeaker_RT_ci] = &LRSEBTMediaToSpeaker_RT;

    /******************************************************************************/
    /*                    R-RSE ?sources to R-headset                             */
    /******************************************************************************/
    //create R-RSE music source to R-headset routing map-------->tested ok
    RRSEMusicToRHeadset_RT.connectionCntOfPath = 2;
    RRSEMusicToRHeadset_RT.codecConnectStartIndex = 1;
    RRSEMusicToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pr");
    RRSEMusicToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_RSE_R");
    ama_connectInfo_t RRSEMusicToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_MUSIC,E_RSE_R_HEADSET);
    //gain info
    RRSEMusicToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    RRSEMusicToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEMusicToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEMusicToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(RRSEMusicToRHeadset_RT.gainList, RRSEMusicToRHeadset_gainList, sizeof(RRSEMusicToRHeadset_gainList));
    mRoutingMap[RRSEMusicToRHeadset_RT_ci] = &RRSEMusicToRHeadset_RT;

    //create R-RSE video source to R-headset routing map-------->tested ok
    RRSEVideoToRHeadset_RT.connectionCntOfPath = 2;
    RRSEVideoToRHeadset_RT.codecConnectStartIndex = 1;
    RRSEVideoToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pr");
    RRSEVideoToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_RSE_R");
    ama_connectInfo_t RRSEVideoToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_VIDEO_AUDIO,E_RSE_R_HEADSET);
    //gain info
    RRSEVideoToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    RRSEVideoToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEVideoToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEVideoToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(RRSEVideoToRHeadset_RT.gainList, RRSEVideoToRHeadset_gainList, sizeof(RRSEVideoToRHeadset_gainList));
    mRoutingMap[RRSEVideoToRHeadset_RT_ci] = &RRSEVideoToRHeadset_RT;

    //create R-RSE BtMedia source to R-headset routing map------->tested ok
    RRSEBTMediaToRHeadset_RT.connectionCntOfPath = 2;
    RRSEBTMediaToRHeadset_RT.codecConnectStartIndex = 1;
    RRSEBTMediaToRHeadset_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D4pr");
    RRSEBTMediaToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_RSE_R");
    ama_connectInfo_t RRSEBTMediaToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_BT_AUDIO,E_RSE_R_HEADSET);
    //gain info
    RRSEBTMediaToRHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY2;
    RRSEBTMediaToRHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEBTMediaToRHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEBTMediaToRHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(RRSEBTMediaToRHeadset_RT.gainList, RRSEBTMediaToRHeadset_gainList, sizeof(RRSEBTMediaToRHeadset_gainList));
    mRoutingMap[RRSEBTMediaToRHeadset_RT_ci] = &RRSEBTMediaToRHeadset_RT;
    /******************************************************************************/
    /*                    R-RSE ?sources to L-headset                             */
    /******************************************************************************/
    //create R-RSE music source to L-headset routing map-------->tested ok
    RRSEMusicToLHeadset_RT.connectionCntOfPath = 2;
    RRSEMusicToLHeadset_RT.codecConnectStartIndex = 1;
    RRSEMusicToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pr");
    RRSEMusicToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_RSE_L");
    ama_connectInfo_t RRSEMusicToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_MUSIC,E_RSE_L_HEADSET);
    //gain info
    RRSEMusicToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    RRSEMusicToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEMusicToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEMusicToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(RRSEMusicToLHeadset_RT.gainList, RRSEMusicToLHeadset_gainList, sizeof(RRSEMusicToLHeadset_gainList));
    mRoutingMap[RRSEMusicToLHeadset_RT_ci] = &RRSEMusicToLHeadset_RT;

    //create R-RSE video source to L-headset routing map-------->tested ok
    RRSEVideoToLHeadset_RT.connectionCntOfPath = 2;
    RRSEVideoToLHeadset_RT.codecConnectStartIndex = 1;
    RRSEVideoToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pr");
    RRSEVideoToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_RSE_L");
    ama_connectInfo_t RRSEVideoToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_VIDEO_AUDIO,E_RSE_L_HEADSET);
    //gain info
    RRSEVideoToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    RRSEVideoToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEVideoToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEVideoToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(RRSEVideoToLHeadset_RT.gainList, RRSEVideoToLHeadset_gainList, sizeof(RRSEVideoToLHeadset_gainList));
    mRoutingMap[RRSEVideoToLHeadset_RT_ci] = &RRSEVideoToLHeadset_RT;

    //create R-RSE BtMusic source to L-headset routing map------->tested ok
    RRSEBTMediaToLHeadset_RT.connectionCntOfPath = 2;
    RRSEBTMediaToLHeadset_RT.codecConnectStartIndex = 1;
    RRSEBTMediaToLHeadset_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D4pr");
    RRSEBTMediaToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_RSE_L");
    ama_connectInfo_t RRSEBTMediaToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_BT_AUDIO,E_RSE_L_HEADSET);
    //gain info
    RRSEBTMediaToLHeadset_RT.amacChannel = AMAC_CHANNEL_SECONDARY;
    RRSEBTMediaToLHeadset_RT.gainNum = SHORT_GAIN_NUM;
    RRSEBTMediaToLHeadset_RT.pVolGainTable = VolSetToGain_SS;
    AMAC_Gain_t RRSEBTMediaToLHeadset_gainList[SHORT_GAIN_NUM] = {0x00CC,0x0503,0x0503};
    memcpy(RRSEBTMediaToLHeadset_RT.gainList, RRSEBTMediaToLHeadset_gainList, sizeof(RRSEBTMediaToLHeadset_gainList));
    mRoutingMap[RRSEBTMediaToLHeadset_RT_ci] = &RRSEBTMediaToLHeadset_RT;

    /******************************************************************************/
    /*                    R-RSE ?sources to speaker                               */
    /******************************************************************************/
    //create R-RSE music source to speaker routing map-------->tested ok
    RRSEMusicToSpeaker_RT.connectionCntOfPath = 2;
    RRSEMusicToSpeaker_RT.codecConnectStartIndex = 1;
    RRSEMusicToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pr");
    RRSEMusicToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_PRIMARY");
    ama_connectInfo_t RRSEMusicToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_MUSIC,E_IVI_SPEAKER);
    //gain info
    RRSEMusicToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    RRSEMusicToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    RRSEMusicToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t RRSEMusicToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(RRSEMusicToSpeaker_RT.gainList, RRSEMusicToSpeaker_gainList, sizeof(RRSEMusicToSpeaker_gainList));
    mRoutingMap[RRSEMusicToSpeaker_RT_ci] = &RRSEMusicToSpeaker_RT;

    //create R-RSE tuner source to speaker routing map-------->tested ok
    RRSEVideoToSpeaker_RT.connectionCntOfPath = 2;
    RRSEVideoToSpeaker_RT.codecConnectStartIndex = 1;
    RRSEVideoToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D4pr");
    RRSEVideoToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_PRIMARY");
    ama_connectInfo_t RRSEVideoToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_VIDEO_AUDIO,E_IVI_SPEAKER);
    //gain info
    RRSEVideoToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    RRSEVideoToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    RRSEVideoToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t RRSEVideoToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(RRSEVideoToSpeaker_RT.gainList, RRSEVideoToSpeaker_gainList, sizeof(RRSEVideoToSpeaker_gainList));
    mRoutingMap[RRSEVideoToSpeaker_RT_ci] = &RRSEVideoToSpeaker_RT;

    //create R-RSE BtCall source to speaker routing map------->tested ok
    RRSEBTMediaToSpeaker_RT.connectionCntOfPath = 2;
    RRSEBTMediaToSpeaker_RT.codecConnectStartIndex = 1;
    RRSEBTMediaToSpeaker_RT.conNameArray[0] = make_pair("bt_audio_stream","pa_pcmC0D4pr");
    RRSEBTMediaToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_TDM3_IN_2","SINK_PRIMARY");
    ama_connectInfo_t RRSEBTMediaToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_BT_AUDIO,E_IVI_SPEAKER);
    //gain info
    RRSEBTMediaToSpeaker_RT.amacChannel = AMAC_CHANNEL_PRIMARY;
    RRSEBTMediaToSpeaker_RT.gainNum = LONG_GAIN_NUM;
    RRSEBTMediaToSpeaker_RT.pVolGainTable = VolSetToGain_PRI;
    AMAC_Gain_t RRSEBTMediaToSpeaker_gainList[LONG_GAIN_NUM] = {0x00CC,0x0503,0x0503,0x0503,0x0503};
    memcpy(RRSEBTMediaToSpeaker_RT.gainList, RRSEBTMediaToSpeaker_gainList, sizeof(RRSEBTMediaToSpeaker_gainList));
    mRoutingMap[RRSEBTMediaToSpeaker_RT_ci] = &RRSEBTMediaToSpeaker_RT;

    /******************************************************************************/
    /*                    L-RSE softKey to L-headset                              */
    /******************************************************************************/
    //create L-RSE softKey source to L-headset routing map
    LRSESoftKeyToLHeadset_RT.connectionCntOfPath = 2;
    LRSESoftKeyToLHeadset_RT.codecConnectStartIndex = 1;
    LRSESoftKeyToLHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D8pl");
    LRSESoftKeyToLHeadset_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN4_L","SINK_RSE_L");
    ama_connectInfo_t LRSESoftKeyToLHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_SOFTKEY,E_RSE_L_HEADSET);
    //gain info
    LRSESoftKeyToLHeadset_RT.amacChannel = AMAC_CHANNEL_NULL;
    LRSESoftKeyToLHeadset_RT.gainNum = NO_GAIN_SET;
    LRSESoftKeyToLHeadset_RT.pVolGainTable = VolSetToGain_LRSE;
    mRoutingMap[LRSESoftKeyToLHeadset_RT_ci] = &LRSESoftKeyToLHeadset_RT;

    /******************************************************************************/
    /*                    L-RSE softKey to speaker                                */
    /******************************************************************************/
    //create L-RSE softKey source to speaker routing map
    LRSESoftKeyToSpeaker_RT.connectionCntOfPath = 2;
    LRSESoftKeyToSpeaker_RT.codecConnectStartIndex = 1;
    LRSESoftKeyToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D8pl");
    LRSESoftKeyToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN4_L","SINK_PRIMARY");
    ama_connectInfo_t LRSESoftKeyToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE1,E_STREAM_SOFTKEY,E_IVI_SPEAKER);
    //gain info
    LRSESoftKeyToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    LRSESoftKeyToSpeaker_RT.gainNum = NO_GAIN_SET;
    LRSESoftKeyToSpeaker_RT.pVolGainTable = VolSetToGain_LRSE;
    mRoutingMap[LRSESoftKeyToSpeaker_RT_ci] = &LRSESoftKeyToSpeaker_RT;

    /******************************************************************************/
    /*                    R-RSE softKey to R-headset                              */
    /******************************************************************************/
    //create R-RSE softKey source to R-headset routing map
    RRSESoftKeyToRHeadset_RT.connectionCntOfPath = 2;
    RRSESoftKeyToRHeadset_RT.codecConnectStartIndex = 1;
    RRSESoftKeyToRHeadset_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D8pr");
    RRSESoftKeyToRHeadset_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN4_R","SINK_RSE_R");
    ama_connectInfo_t RRSESoftKeyToRHeadset_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_SOFTKEY,E_RSE_R_HEADSET);
    //gain info
    RRSESoftKeyToRHeadset_RT.amacChannel = AMAC_CHANNEL_NULL;
    RRSESoftKeyToRHeadset_RT.gainNum = NO_GAIN_SET;
    RRSESoftKeyToRHeadset_RT.pVolGainTable = VolSetToGain_RRSE;
    mRoutingMap[RRSESoftKeyToRHeadset_RT_ci] = &RRSESoftKeyToRHeadset_RT;

    /******************************************************************************/
    /*                    R-RSE softKey to speaker                                */
    /******************************************************************************/
    //create R-RSE softKey source to speaker routing map
    RRSESoftKeyToSpeaker_RT.connectionCntOfPath = 2;
    RRSESoftKeyToSpeaker_RT.codecConnectStartIndex = 1;
    RRSESoftKeyToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D8pr");
    RRSESoftKeyToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN4_R","SINK_PRIMARY");
    ama_connectInfo_t RRSESoftKeyToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_RSE2,E_STREAM_SOFTKEY,E_IVI_SPEAKER);
    //gain info
    RRSESoftKeyToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    RRSESoftKeyToSpeaker_RT.gainNum = NO_GAIN_SET;
    RRSESoftKeyToSpeaker_RT.pVolGainTable = VolSetToGain_RRSE;
    mRoutingMap[RRSESoftKeyToSpeaker_RT_ci] = &RRSESoftKeyToSpeaker_RT;

    /******************************************************************************/
    /*                    ivi naviTTS to speaker FL                               */
    /******************************************************************************/
    //create ivi naviTTS source to speaker routing map
    iviNaviToSpeaker_RT.connectionCntOfPath = 2;
    iviNaviToSpeaker_RT.codecConnectStartIndex = 1;
    iviNaviToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pl");
    iviNaviToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_L","SINK_FL");
    //gain info
    iviNaviToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviNaviToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviNaviToSpeaker_RT.pVolGainTable = VolSetToGain_NAVI;
    ama_connectInfo_t iviNaviToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_NAVI_TTS,E_IVI_SPEAKER);
    mRoutingMap[iviNaviToSpeaker_RT_ci] = &iviNaviToSpeaker_RT;
    ama_connectInfo_t serviceNaviToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_NONE,E_STREAM_NAVI_TTS,E_IVI_SPEAKER);
    mRoutingMap[serviceNaviToSpeaker_RT_ci] = &iviNaviToSpeaker_RT;

    /******************************************************************************/
    /*                    ivi FL-radar to speaker FL                              */
    /******************************************************************************/
    //create ivi FL-radar source to speaker routing map
    iviFLRadarToSpeaker_RT.connectionCntOfPath = 2;
    iviFLRadarToSpeaker_RT.codecConnectStartIndex = 1;
    iviFLRadarToSpeaker_RT.conNameArray[0] = make_pair("radarStream","pa_pcmC0D10pr");
    iviFLRadarToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FL");
    //gain info
    iviFLRadarToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviFLRadarToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviFLRadarToSpeaker_RT.pVolGainTable = VolSetToGain_RADAR;
    ama_connectInfo_t iviFLRadarToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_FL_RADAR,E_IVI_SPEAKER);
    mRoutingMap[iviFLRadarToSpeaker_RT_ci] = &iviFLRadarToSpeaker_RT;

    /******************************************************************************/
    /*                    ivi FR-radar to speaker FR                              */
    /******************************************************************************/
    //create ivi FR-radar source to speaker routing map
    iviFRRadarToSpeaker_RT.connectionCntOfPath = 2;
    iviFRRadarToSpeaker_RT.codecConnectStartIndex = 1;
    iviFRRadarToSpeaker_RT.conNameArray[0] = make_pair("radarStream","pa_pcmC0D10pr");
    iviFRRadarToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FR");
    //gain info
    iviFRRadarToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviFRRadarToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviFRRadarToSpeaker_RT.pVolGainTable = VolSetToGain_RADAR;
    ama_connectInfo_t iviFRRadarToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_FR_RADAR,E_IVI_SPEAKER);
    mRoutingMap[iviFRRadarToSpeaker_RT_ci] = &iviFRRadarToSpeaker_RT;

    /******************************************************************************/
    /*                    ivi RL-radar to speaker RL                              */
    /******************************************************************************/
    //create ivi RL-radar source to speaker routing map
    iviRLRadarToSpeaker_RT.connectionCntOfPath = 2;
    iviRLRadarToSpeaker_RT.codecConnectStartIndex = 1;
    iviRLRadarToSpeaker_RT.conNameArray[0] = make_pair("radarStream","pa_pcmC0D10pr");
    iviRLRadarToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_RL");
    //gain info
    iviRLRadarToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviRLRadarToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviRLRadarToSpeaker_RT.pVolGainTable = VolSetToGain_RADAR;
    ama_connectInfo_t iviRLRadarToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_RL_RADAR,E_IVI_SPEAKER);
    mRoutingMap[iviRLRadarToSpeaker_RT_ci] = &iviRLRadarToSpeaker_RT;

    /******************************************************************************/
    /*                    ivi RR-radar to speaker RR                              */
    /******************************************************************************/
    //create ivi RR-radar source to speaker routing map
    iviRRRadarToSpeaker_RT.connectionCntOfPath = 2;
    iviRRRadarToSpeaker_RT.codecConnectStartIndex = 1;
    iviRRRadarToSpeaker_RT.conNameArray[0] = make_pair("radarStream","pa_pcmC0D10pr");
    iviRRRadarToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_RR");
    //gain info
    iviRRRadarToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviRRRadarToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviRRRadarToSpeaker_RT.pVolGainTable = VolSetToGain_RADAR;
    ama_connectInfo_t iviRRRadarToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_RR_RADAR,E_IVI_SPEAKER);
    mRoutingMap[iviRRRadarToSpeaker_RT_ci] = &iviRRRadarToSpeaker_RT;

    /******************************************************************************/
    /*                    ivi radarAlert to speaker FL                            */
    /******************************************************************************/
    //create ivi radarAlert source to speaker routing map
    iviRadarAlertToSpeaker_RT.connectionCntOfPath = 2;
    iviRadarAlertToSpeaker_RT.codecConnectStartIndex = 1;
    iviRadarAlertToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pr");
    iviRadarAlertToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FL");
    ama_connectInfo_t iviRadarAlertToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_RADAR_ALERT,E_IVI_SPEAKER);
    //gain info
    iviRadarAlertToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviRadarAlertToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviRadarAlertToSpeaker_RT.pVolGainTable = VolSetToGain_RADAR;
    mRoutingMap[iviRadarAlertToSpeaker_RT_ci] = &iviRadarAlertToSpeaker_RT;


    /******************************************************************************/
    /*                      ivi prompt to speaker FL                              */
    /******************************************************************************/
    //create ivi prompt source to speaker routing map
    iviPromptToSpeaker_RT.connectionCntOfPath = 2;
    iviPromptToSpeaker_RT.codecConnectStartIndex = 1;
    iviPromptToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pr");
    iviPromptToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FL");
    ama_connectInfo_t iviPromptToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_NOTIFICATION,E_IVI_SPEAKER);
    //gain info
    iviPromptToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviPromptToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviPromptToSpeaker_RT.pVolGainTable = VolSetToGain_IS;
    mRoutingMap[iviPromptToSpeaker_RT_ci] = &iviPromptToSpeaker_RT;

    //create ivi prompt source to speaker routing map
    iviCarplayAltToSpeaker_RT.connectionCntOfPath = 2;
    iviCarplayAltToSpeaker_RT.codecConnectStartIndex = 1;
    iviCarplayAltToSpeaker_RT.conNameArray[0] = make_pair("carplayAlt","pa_pcmC0D10pr");
    iviCarplayAltToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FL");
    //gain info
    iviCarplayAltToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviCarplayAltToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviCarplayAltToSpeaker_RT.pVolGainTable = VolSetToGain_IS;
    ama_connectInfo_t iviCarplayAltToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_CARPLAY_ALT,E_IVI_SPEAKER);
    mRoutingMap[iviCarplayAltToSpeaker_RT_ci] = &iviCarplayAltToSpeaker_RT;

    //create ivi carplay alt source to RR&RL speaker routing map
    static AMRoutingNameTable_s iviCarplayAltToSpeaker_RT;

    /******************************************************************************/
    /*                      ivi AVMALert to speaker FL                              */
    /******************************************************************************/
    //create ivi AVMAlert source to speaker routing map
    iviAVMAlertToSpeaker_RT.connectionCntOfPath = 2;
    iviAVMAlertToSpeaker_RT.codecConnectStartIndex = 1;
    iviAVMAlertToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pr");
    iviAVMAlertToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FL");
    ama_connectInfo_t iviAVMAlertToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_AVM_ALERT,E_IVI_SPEAKER);
    //gain info
    iviAVMAlertToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviAVMAlertToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviAVMAlertToSpeaker_RT.pVolGainTable = VolSetToGain_IS;
    mRoutingMap[iviAVMAlertToSpeaker_RT_ci] = &iviAVMAlertToSpeaker_RT;


    /******************************************************************************/
    /*                      ivi backdoorPrompt to speaker RR&RL                   */
    /******************************************************************************/
    //create ivi backdoorPrompt source to RR&RL speaker routing map
    static AMRoutingNameTable_s iviBackdoorPromptToSpeaker_RT;
    iviBackdoorPromptToSpeaker_RT.connectionCntOfPath = 3;
    iviBackdoorPromptToSpeaker_RT.codecConnectStartIndex = 1;
    iviBackdoorPromptToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pr");
    iviBackdoorPromptToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_RR");
    iviBackdoorPromptToSpeaker_RT.conNameArray[2] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_RL");
    ama_connectInfo_t iviBackdoorPromptToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_BACKDOOR_NOTI,E_IVI_SPEAKER);
    //gain info
    iviBackdoorPromptToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviBackdoorPromptToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviBackdoorPromptToSpeaker_RT.pVolGainTable = VolSetToGain_IS;
    mRoutingMap[iviBackdoorPromptToSpeaker_RT_ci] = &iviBackdoorPromptToSpeaker_RT;

    /******************************************************************************/
    /*                      ivi feedbackSound to speaker FL                              */
    /******************************************************************************/
    //create ivi feedbackSound source to speaker routing map
    static AMRoutingNameTable_s iviFeedbackToSpeaker_RT;
    iviFeedbackToSpeaker_RT.connectionCntOfPath = 2;
    iviFeedbackToSpeaker_RT.codecConnectStartIndex = 1;
    iviFeedbackToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pr");
    iviFeedbackToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_FL");
    ama_connectInfo_t iviFeedbackToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_FEEDBACK_NOTI,E_IVI_SPEAKER);
    //gain info
    iviFeedbackToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviFeedbackToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviFeedbackToSpeaker_RT.pVolGainTable = VolSetToGain_IS;
    mRoutingMap[iviFeedbackToSpeaker_RT_ci] = &iviFeedbackToSpeaker_RT;

    /******************************************************************************/
    /*                        ivi softKey to speaker                              */
    /******************************************************************************/
    //create ivi softKey source to speaker routing map
    iviSoftKeyToSpeaker_RT.connectionCntOfPath = 2;
    iviSoftKeyToSpeaker_RT.codecConnectStartIndex = 1;
    iviSoftKeyToSpeaker_RT.conNameArray[0] = make_pair(PULSEAUDIO_STREAM,"pa_pcmC0D10pr");
    iviSoftKeyToSpeaker_RT.conNameArray[1] = make_pair("SOURCE_HOST_IIS_IN5_R","SINK_PRIMARY");
    ama_connectInfo_t iviSoftKeyToSpeaker_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_SOFTKEY,E_IVI_SPEAKER);
    //gain info
    iviSoftKeyToSpeaker_RT.amacChannel = AMAC_CHANNEL_NULL;
    iviSoftKeyToSpeaker_RT.gainNum = NO_GAIN_SET;
    iviSoftKeyToSpeaker_RT.pVolGainTable = VolSetToGain_SOFTKEY;
    mRoutingMap[iviSoftKeyToSpeaker_RT_ci] = &iviSoftKeyToSpeaker_RT;

    /******************************************************************************/
    /*                        mic to btPhone paSink                               */
    /******************************************************************************/
    //create mic source to btPhone routing map
    micToBTPhone_RT.connectionCntOfPath = 1;
    micToBTPhone_RT.codecConnectStartIndex = 1;
    micToBTPhone_RT.conNameArray[0] = make_pair("pa_pcmC0D3c",PULSEAUDIO_STREAM);
    ama_connectInfo_t micToBTPhone_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_PHONE_MIC,E_IVI_MIC);
    //gain info
    micToBTPhone_RT.amacChannel = AMAC_CHANNEL_NULL;
    micToBTPhone_RT.gainNum = NO_GAIN_SET;
    micToBTPhone_RT.pVolGainTable = VolSetToGain_PRI;
    mRoutingMap[micToBTPhone_RT_ci] = &micToBTPhone_RT;

    /******************************************************************************/
    /*                            mic to VR paSink                                */
    /******************************************************************************/
    //create mic source to VR routing map
    micToVR_RT.connectionCntOfPath = 1;
    micToVR_RT.codecConnectStartIndex = 1;
    micToVR_RT.conNameArray[0] = make_pair("pa_pcmC0D3c",PULSEAUDIO_STREAM);
    ama_connectInfo_t micToVR_RT_ci = CREATE_CONNECTINFO_BY_SEAT_STREAMTYPE_DEVICETYPE(E_SEAT_IVI,E_STREAM_VR_MIC,E_IVI_MIC);
    //gain info
    micToVR_RT.amacChannel = AMAC_CHANNEL_NULL;
    micToVR_RT.gainNum = NO_GAIN_SET;
    micToVR_RT.pVolGainTable = VolSetToGain_PRI;
    mRoutingMap[micToVR_RT_ci] = &micToVR_RT;

    logVerbose(AC,"AMClient::createRoutingMap()-->OUT");
}
