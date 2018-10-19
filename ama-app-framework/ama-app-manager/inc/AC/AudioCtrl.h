/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioCtrl.h
/// @brief contains class AudioController
///
/// Created by wangqi on 2016/6/15.
/// this file contains the definination of class AudioController
///

#ifndef AUDIOCTRL_H_
#define AUDIOCTRL_H_

//sdk->AMACInterface
#include <AMACInterface.h>

#include <mutex>

#include "AMGRLogContext.h"
#include "AudioTask.h"
#include "AMClient.h"
#include "FocusCtrl.h"

#include "ama_audioTypes.h"
#include "ama_types.h"
#include "Stream.h"
#include "Connection.h"
#include "AudioSetting.h"
#include "Amplifier.h"

using std::shared_ptr;
using std::string;
using std::map;

/// @class AudioController
/// @brief this class is the abstraction of Audio module
///
/// this class receive the task from TaskDispatcher. and
/// handle the actuall work about audio moudule
class AudioController
{
public:
    static AudioController* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static AudioController instance;
        return &instance;
    }
    ~AudioController();

    void doTask(shared_ptr<Task> task);
    void earlyInit(void);

    void handleRequestAudioFocus(ama_connectID_t connectID);
    void handleAbandonAudioFocus(ama_connectID_t connectID);

    shared_ptr<Stream> findStreamFromMapByStreamID(ama_streamID_t streamID);
    shared_ptr<Stream> createStreamByStreamID(ama_streamID_t streamID);
    shared_ptr<Connection> findConnectionFromMapByConnectID(ama_connectID_t connectID);
    shared_ptr<Connection> createConnectionIntoMap(ama_connectID_t connectID);
    void delConnectionFromMap(ama_connectID_t connectID);
    ama_streamID_t getApplyStreamID(void){
        return mApplyStreamID;
    }
private:
    AudioController();

    void audioControllerInit(void);

    void handleAMSourceCreated(AMAC_SourceType_s AMSource);
    void handleAMSourceDestroyed(AMAC_SourceID_t AMSourceID);
    void handleAMSinkCreated(AMAC_SinkType_s AMSink);
    void handleAMSinkDestroyed(AMAC_SinkID_t AMSinkID);

    void debugShowConnections(void);
    void handleDebugCmd(int debugCmd);

    void delAppStreamAndCon(ama_AMPID_t AMPID);

    void handleSetReq(int key,int value);

    void handleDuckReq(int durations_ms,int duckVolume);
    void handleUnduckReq(int durations_ms);

    void recordApplyStreamID(ama_streamID_t applyStreamID){
        mApplyStreamID = applyStreamID;
    }

    void execConnectWhenSourceCreatedByStreamType(ama_streamType_t streamType);
    void execDisconnectWhenSourceDestroyByStreamType(ama_streamType_t streamType);

    void findAllRadarStreamAndexecConnectWhenSourceCreated(void);
    void findAllRadarStreamAndexecDisconnectWhenSourceDestroy(void);
private:
    std::map<ama_streamID_t,shared_ptr<Stream>> mAllStreamMap;
    std::map<ama_connectID_t,shared_ptr<Connection>> mAllConnectionMap;
    ama_streamID_t mApplyStreamID;//record the stream that request/abandon audio focus
};

#endif //AUDIOCTRL_H_
