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
/// Created by wangqi on 2016/11/30.
/// this file contains the definination of class Connection
///

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <mutex>
#include <vector>
#include <map>

#include "AudioSetting.h"

using std::vector;
using std::map;

/// @class Connection
/// @brief this class is the abstraction of Connection
///
class Connection:
    public SettingObserver
{
public:
    Connection(ama_connectID_t connectID);
    ~Connection();

    void OnAudioSettingChanged();

    void requestAudioDeviceFocus();
    void releaseAudioDeviceFocus();
    void setStreamMute(bool isMute);

    void handleNewFocusState(ama_focusSta_t newFocusSta);

    void execConnectWhenSourceCreated(void);
    void execDisconnectWhenSourceRemoved(void);

    void duckAudio(int durations_ms,int duckVolume);
    void unduckAudio(int durations_ms);

    ama_focusSta_t focusSta(void){
        return mFocusSta;
    }

    bool isStreamMute(void){
        return mIsStreamMute;
    }
private://function
    // we need set volume:
    // 1)when foucs state changed
    // 2)when connect state changed
    // 3)when audioSetting changed
    // 4)when speed changed
    void setVolume(void);

    void requestConnect(void);
    void requestDisconnect(void);
    void requestSetVolGain(int gain);

    ama_focusSta_t convert2AppFoucsSta(ama_focusSta_t foucsSta);
private://variable
    ama_connectID_t mConnectID;
    ama_focusSta_t mFocusSta;

    bool mIsStreamMute;
};

#endif //CONNECTION_H_
