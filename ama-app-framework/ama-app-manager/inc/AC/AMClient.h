/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AMClient.h
/// @brief contains class AMClient
///
/// Created by wangqi on 2016/6/16.
/// this file contains the definination of class AMClient
///

#ifndef AMCLIENT_H_
#define AMCLIENT_H_

#include <mutex>
#include <vector>
#include <list>

//sdk->AMACInterface
#include <AMACInterface.h>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"
#include "AMdefine.h"

using std::shared_ptr;
using std::pair;
using std::make_pair;
using std::vector;

class ConnectStaChangedObserver;

/// @class AMClient
/// @brief this class is the abstraction of Audio module
///
/// this class is a client of AudioManager. it's contains
/// AudioManagerCommandClient and AudioManagerControllerClient.
/// AudioController use this class to do actual work
class AMClient
{
public:
    static AMClient* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static AMClient instance;
        return &instance;
    }
    virtual ~AMClient();

    ama_Error_e connectByAMClient(ama_connectID_t connectID);
    ama_Error_e disconnectByAMClient(ama_connectID_t connectID);
    ama_Error_e setConnectionVolGain(ama_connectID_t connectID, int gain);

    ama_Error_e setBassGain(AMAC_Channel_t amacChannel,int bassGain);
    ama_Error_e setMidGain(AMAC_Channel_t amacChannel,int bassGain);
    ama_Error_e setTrebleGain(AMAC_Channel_t amacChannel,int bassGain);

    ama_Error_e setChannelMuteState(AMAC_Channel_t amacChannel,bool isEnableMute);

    AMAC_Channel_t getAMACChannelByConnectID(ama_connectID_t connectID);

    bool isVolInfoActive(ama_volumeInfo_t volInfo);

    ama_volumeType_t getActiveStreamTypeWhenHardKeySet();
    void updateActiveStreamTypeForHardKey();

    int setStreamMute(ama_connectID_t connectID);

    int getVolGainByStep(ama_connectID_t connectID,int volStep);

    int getAMSourceIDFromMap(string sourceName);
    int getAMSinkIDFromMap(string sinkName);

    void addAMSourceIntoMap(string sourceName,AMAC_SourceID_t sourceID);
    void delAMSourceFromMap(AMAC_SourceID_t sourceID);

    void addAMSinkIntoMap(string sinkName,AMAC_SinkID_t sinkID);
    void delAMSinkFromMap(AMAC_SinkID_t sinkID);

    string getSourceNameByID(AMAC_SourceID_t sourceID);

    void registerCommandAndControllerClient(void);

    int getBassGain(int BassSet,int volGain);
    int getMidGain(int MidSet,int volGain);
    int getTrebleGain(int TrebleSet,int volGain);

    int setBalance(int balanceSet);
    int setFader(int faderSet);

    ama_AMPID_t getLastConnectMediaApp(ama_audioDeviceType_t deviceID);

    void attach(ConnectStaChangedObserver *pConnectStaChangedObserver);
    void detach(ConnectStaChangedObserver *pConnectStaChangedObserver);
    void notifyConnectStaChanged(ama_connectID_t connectID,bool isConnect);

    void debugShowAMSources(void);
    void debugShowAMSinks(void);
    void debugShowConRecord(void);
    void debugShowRoutingMap(void);
private:
    AMClient();
    void createRoutingMap(void);
    string convertSourceNameIfPASource(string sourceName,ama_connectID_t connectID);
    string convertSinkNameIfPASink(string sourceName,ama_connectID_t connectID);
    int getRoutingNameTable(ama_connectInfo_t connectInfo, AMRoutingNameTable_s** ppRoutingTable);
    int getRoutingIDTable(AMRoutingNameTable_s* pRoutingNameTable, AMRoutingIDTable_s *pRoutingIdTable,ama_connectID_t connectID);
    int getConFinalRoutingIDTableByConnectionSta(AMRoutingIDTable_s *pRoutingIdTable,AMRoutingIDTable_s *pFinalRoutingIdTable);
    int getDisconFinalRoutingIDTableByConnectionSta(AMRoutingIDTable_s *pRoutingIdTable,AMRoutingIDTable_s *pFinalRoutingIdTable);
    bool isConnectionNeedExec(AMConnectionIDPair connection);
    bool isConnectionCanExecByAMAC(AMConnectionIDPair connection);
    bool isDisconnectNeedExec(AMConnectionIDPair disconnection);
    bool isDisconnectCanExecByAMAC(AMConnectionIDPair disconnection);
    void recordConnectState(AMConnectionIDPair connection,bool isConnect);
    void recordConStaOfRoutingTable(AMRoutingIDTable_s *pRoutingIdTable,bool isConnect);
    void logPrintAMRoutingNameTable(AMRoutingNameTable_s* pRoutingNameTable);

    int connectStepByStep(ama_connectID_t connectID);
    int disconnectStepByStep(ama_connectID_t connectID);

    void recordActiveConnection(ama_connectID_t connectID, bool isConnect);

    void recordLastConnectMediaApp(ama_connectID_t connectID, bool isConnect);

    bool isManagerStartFirstTime(void);

    std::map<ama_connectInfo_t,AMRoutingNameTable_s*> mRoutingMap;
    std::map<AMConnectionIDPair,int> mConnectionState;//for AudioManager

    std::vector<ama_connectID_t> mActiveConnectionVec;//for AudioController

    std::map<string,shared_ptr<AMSource>> mAMSourceMap;
    std::map<string,shared_ptr<AMSink>> mAMSinkMap;

    ama_AMPID_t mLastMediaAppIVI;
    ama_AMPID_t mLastMediaAppLRSE;
    ama_AMPID_t mLastMediaAppRRSE;

    std::list<ConnectStaChangedObserver *> mObserverList;
};

class ConnectStaChangedObserver
{
public:
    ConnectStaChangedObserver(){
        AMClient::GetInstance()->attach(this);
    }
    ~ConnectStaChangedObserver(){
        AMClient::GetInstance()->detach(this);
    }

    virtual void OnConnectStaChanged(ama_connectID_t connectID,bool isConnect)=0;
};

#endif //AMCLIENT_H_
