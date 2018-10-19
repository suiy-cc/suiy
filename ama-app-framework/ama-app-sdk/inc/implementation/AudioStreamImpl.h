/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef AUDIOSTREAMIMPL_H
#define AUDIOSTREAMIMPL_H

#include <string>
#include <memory>
#include <mutex>

#include "ama_audioTypes.h"
#include "ama_types.h"
#include "AudioStream.h"
#include "AppAudioServiceImpl.h"

using namespace std;

class AudioFocus
{
public:
    AudioFocus(ama_connectID_t connectID);
    virtual ~AudioFocus();

    ama_focusSta_t requestAudioFocus(audioFocusChangedCB pCallbackFunc);
    ama_focusSta_t abandonAudioFocus(void);
    ama_focusSta_t getFocusSta(void);
    void OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
    void OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible);
protected:
    ama_connectID_t mConnectID;
    ama_focusSta_t mFocusSta;
    ama_audioDeviceType_t mAudioDevice;
    audioFocusChangedCB mpCallbackFunc;
    unsigned long long mFocusWaiterID;
    std::mutex mMutexOfAction;
};

class AudioFocusOfVedioApp : public AudioFocus
{
public:
    AudioFocusOfVedioApp(ama_connectID_t connectID);
    virtual ~AudioFocusOfVedioApp();

    ama_focusSta_t requestAudioFocus(audioFocusChangedCB pCallbackFunc);
    ama_focusSta_t abandonAudioFocus(void);
    ama_focusSta_t getFocusSta(void);
    void OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
    void OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible);
private:
    ama_focusSta_t getNotifyFocusStaByCurSta(void);

    bool mIsVideoAppShowing;
    ama_focusSta_t mNotifyFocusSta;
};

class AudioStreamImpl : public AudioFocusObserver
{
public:
    AudioStreamImpl(ama_streamID_t streamID);
    virtual ~AudioStreamImpl();

    ama_streamID_t streamID(void);
    string streamIDStr(void);

    ama_focusSta_t requestAudioFocus(ama_audioDeviceType_t audioDevice,audioFocusChangedCB func);
    ama_focusSta_t abandonAudioFocus(ama_audioDeviceType_t audioDevice);

    ama_focusSta_t getFocusState(ama_audioDeviceType_t audioDevice);

    ama_Error_e setStreamMuteStateSync(bool isMute);//for all device

    void OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
    void OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible);
private:
    ama_streamID_t mStreamID;
    AudioFocus* mpSpeakerFocus;
    AudioFocus* mpLHeadsetFocus;
    AudioFocus* mpRHeadsetFocus;
    AudioFocus* mpMicFocus;
};

#endif //AUDIOSTREAMIMPL_H
