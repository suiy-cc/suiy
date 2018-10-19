/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ConnectController.h
/// @brief contains class ConnectController
///
/// Created by wangqi on 2017/03/20.
/// this file contains the definination of class ConnectController
///

#ifndef CONCTRL_H_
#define CONCTRL_H_

#include <mutex>
#include <vector>
#include <map>
#include <assert.h>

#include "AMConnection.h"
#include "AMChannel.h"
#include "ConReq.h"
#include "Speaker.h"
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "ama_audioTypes.h"

using std::vector;
using std::map;
using std::shared_ptr;

/// @class ConnectController
/// @brief this class is the abstraction of ConnectController
///
class ConnectController
{
public:
    static ConnectController* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static ConnectController instance;
        return &instance;
    }
    ~ConnectController();

    void requestConnect(ama_connectID_t ACConnectID);
    void requestDisconnect(ama_connectID_t ACConnectID);
    void requestSetVolGain(ama_connectID_t ACConnectID, int gain);
    void requestDuckAudio(ama_connectID_t ACConnectID, int durations_ms,int duckVolume);
    void requestUnduckAudio(ama_connectID_t ACConnectID, int durations_ms);

    void tryToConnectAndSetVolWhenAMSourceCreated(ama_connectID_t ACConnectID);

    void setActiveVolGain(ama_connectID_t ACConnectID,int volGain);
    void setAudioDeviceMuteState(ama_audioDeviceType_t audioDeviceID,bool isEnableMute);

    ama_connectID_t getAMConnectIDBySpeakerMode(ama_connectID_t ACConnectID);
    void refreshAMConnectionWhenSpeakerChanged(ama_seatID_t oldSeatID,ama_seatID_t newSeatID);
private:
    ConnectController();
    void makeRightAMConnectionWhenSpeakerChanged(ama_seatID_t seatID);

    void saveRequest(ama_connectID_t ACConnectID,bool isConnectIt);
    void saveRequest(ama_connectID_t ACConnectID,int volGain);
    void saveDuckRequest(ama_connectID_t ACConnectID,int duckPercentage);
    void tryToConnectByRequest(ama_connectID_t ACConnectID);
    void tryToSetVolByRequest(ama_connectID_t ACConnectID);
    void tryToSetDuckAudio(ama_connectID_t ACConnectID, int durations_ms ,bool isDuck);

    std::map<AMAC_Channel_t,shared_ptr<AMChannel>> mAMChannelMap;

    std::map<shared_ptr<ConnectRequest>,shared_ptr<AMConnection>> mRequestToConnectMap;
};

#endif //CONCTRL_H_
