/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AMChannel.h
/// @brief contains class AMChannel
///
/// Created by wangqi on 2017/03/28.
/// this file contains the definination of class AMChannel
///

#ifndef AMCHANNEL_H_
#define AMCHANNEL_H_

#include <mutex>
#include <vector>
#include <map>

#include "ama_enum.h"
#include "ama_audioTypes.h"
#include "AMdefine.h"
#include "AudioSetting.h"
#include "AMClient.h"


using std::vector;
using std::map;

/// @class Connection
/// @brief this class is the abstraction of Connection
///
class AMChannel:
    public SettingObserver,public ConnectStaChangedObserver
{
public:
    AMChannel(AMAC_Channel_t AMChannelID);
    ~AMChannel();

    void OnAudioSettingChanged();
    void OnConnectStaChanged(ama_connectID_t connectID,bool isConnect);

    void setActiveGain(int activeGain);
    void setChannelMuteState(bool isEnableMute);
private:
    void setBassGain(int bassGain);
    void setMidGain(int midGain);
    void setTrebleGain(int trebleGain);

    void updateBMTWhenCurStaChanged(void);
private:
    AMAC_Channel_t mAMChannelID;
    ama_audioDeviceType_t mAudioDeviceID;

    int mActiveGain;
    int mBassSet;
    int mMidSet;
    int mTrebleSet;

    int mBassGain;
    int mMidGain;
    int mTrebleGain;

    bool mIsChannelMute;
    bool mIsMediaActive;
};

#endif //AMCHANNEL_H_
