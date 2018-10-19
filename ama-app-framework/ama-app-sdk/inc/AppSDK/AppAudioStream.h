/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppAudioStream.h
/// @brief contains API of AppAudioStream
///
/// Created by wq on 2016/8/3.
/// this file contains API of AppAudioStream
///

#ifndef APPAUDIOSTREAM_H
#define APPAUDIOSTREAM_H

#include <string>
#include <memory>

#include "ama_audioTypes.h"
#include "ama_types.h"

using namespace std;

class AppAudioStream : public enable_shared_from_this<AppAudioStream>
{
public:
    AppAudioStream(ama_streamType_t streamType);
    AppAudioStream(ama_seatID_t seatID,ama_appID_t appID,ama_streamType_t streamType);
    virtual ~AppAudioStream();

    ama_streamID_t streamID(void);
    string streamIDStr(void);

    //fullname:requestAudioFocusForDefaultDevice
    ama_Error_e requestAudioFocusForDefDev(void);
    //fullname:requestAudioFocusForDevice
    ama_Error_e requestAudioFocusForDev(ama_audioDeviceType_t audioDevice);
    //fullname:abandonAudioFocusForDefaultDevice
    ama_Error_e abandonAudioFocusForDefDev(void);
    //fullname:abandonAudioFocusForDevice
    ama_Error_e abandonAudioFocusForDev(ama_audioDeviceType_t audioDevice);

    ama_Error_e setStreamMuteState(bool isMute);//for all device

    ama_Error_e duckAudio(int durationsms,int volume);
    ama_Error_e unduckAudio(int durationsms);
protected:
    friend class AppAudioServiceImpl;

    //fullname:releaseAudioFocusForDevice
    virtual void OnAudioFocusChangedForDefDev(ama_focusSta_t newFocusSta);
    virtual void OnAudioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta);
    virtual void OnAudioFocusChangedEx(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);

private:
    void* pAudioServiceImpl;
    ama_streamID_t mStreamID;
};



#endif //APPAUDIOSTREAM_H
