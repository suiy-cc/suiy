/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Speaker.h
/// @brief contains class Speaker
///
/// Created by wangqi on 2017/03/20.
/// this file contains the definination of class Speaker
///

#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <mutex>
#include <vector>
#include <map>

#include "AMConnection.h"
#include "ama_types.h"

using std::vector;
using std::map;

/// @class ConnectController
/// @brief this class is the abstraction of ConnectController
///
class Speaker
{
public:
    static Speaker* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static Speaker instance;
        return &instance;
    }
    ~Speaker();

    ama_connectID_t getAMConnectIDFrom(ama_connectID_t ACConnectID);
    void setWhichSeatUsingSpeaker(ama_seatID_t seatID);
    void abandonToUseSpeaker(ama_seatID_t seatID);
    void isEnableRSEUseSpeakerStaChanged(void);
private:
    Speaker();
    void tellSMSpeakerOwnerChanged(void);
    ama_seatID_t mWhichSeatUsingSpeaker;
};

#endif //SPEAKER_H_
