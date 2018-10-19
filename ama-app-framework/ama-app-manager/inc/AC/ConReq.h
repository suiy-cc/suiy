/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ConnectRequest.h
/// @brief contains class ConnectRequest
///
/// Created by wangqi on 2016/11/30.
/// this file contains the definination of class ConnectRequest
///

#ifndef CONNECTREQUEST_H_
#define CONNECTREQUEST_H_

#include "ama_enum.h"
#include "ama_audioTypes.h"
#include <math.h>

/// @class Connection
/// @brief this class is the abstraction of Connection
///
class ConnectRequest
{
public:
    ConnectRequest(ama_connectID_t ACConnectID){
        mACConnectID = ACConnectID;
        mVolDuckPercentage = 100;
    }
    ~ConnectRequest(){}

    void setConnectReq(bool isNeedToConnect){
        mIsNeedToConnect = isNeedToConnect;
    }
    void setVolGain(int gain){
        mVolGain = gain;
    }
    void setDuckPercentage(int duckPercentage){
        mVolDuckPercentage = duckPercentage;
    }
    ama_connectID_t ACConnectID(){
        return mACConnectID;
    }

    bool isNeedToConnect(void){
        return mIsNeedToConnect;
    }

    int getVolGainWithoutDuck(){
        return mVolGain;
    }

    int getVolGainWithDuck(){
        double changeDB,duckPer;
        int changeDB_int;
        duckPer = static_cast<double>(mVolDuckPercentage);
        // logInfo(AC,"duckPer:",duckPer);
        changeDB = (log10(duckPer/100))*20;
        // logInfo(AC,"changeDB:",changeDB);
        changeDB_int = static_cast<int>(changeDB*10);
        // logInfo(AC,"changeDB_int:",changeDB_int);
        return (mVolGain+changeDB_int);
    }
private:
    ama_connectID_t mACConnectID;

    bool mIsNeedToConnect;
    int mVolGain;
    int mVolDuckPercentage;//(0~100)
};

#endif //CONNECTREQUEST_H_
