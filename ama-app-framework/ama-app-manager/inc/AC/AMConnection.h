/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Connection.h
/// @brief contains class Connection
///
/// Created by wangqi on 2017/03/20.
/// this file contains the definination of class Connection
///

#ifndef AMCONNECTION_H_
#define AMCONNECTION_H_

#include <mutex>
#include <vector>
#include <map>
#include "ama_enum.h"
#include "ama_audioTypes.h"
#include "FadeInThread.h"

using std::vector;
using std::map;

/// @class Connection
/// @brief this class is the abstraction of Connection
///
class AMConnection
{
public:
    AMConnection(ama_connectID_t AMConnectID,ama_connectID_t ACConnectID);
    ~AMConnection();

    void connect(void);
    void disconnect(void);
    void setVolGain(int gain);
    void duckAudio(int durations_ms,int formGain,int toGain);

    ama_connectID_t AMConnectID(){
        return mAMConnectID;
    }

    ama_connectID_t ACConnectID(){
        return mACConnectID;
    }

    bool isConnected(void){
        return mIsConnect;
    }

    int getVolGain(){
        return mVolGain;
    }
private:
    void setDefProperties(void);
    void recordVolGain(int volGain);
    void setVolGainFadeIN(int gain);
    bool setVolGainByAMClient(int gain);

    ama_connectID_t mAMConnectID;
    ama_connectID_t mACConnectID;

    bool mIsActiveChannelConnection;
    bool mIsValidConnection;
    bool mIsConnect;
    int mVolGain;

    shared_ptr<FadeInThread> mpFadeInThread;
};

#endif //AMCONNECTION_H_
