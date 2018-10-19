/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppAudioService.h
/// @brief contains API of audioService
///
/// Created by wq on 2016/8/3.
/// this file contains API of audio
///

#ifndef APPAUDIOSERVICEIMPL_H
#define APPAUDIOSERVICEIMPL_H

#include <string>

#include "ama_audioTypes.h"
#include "AppManager.h"
#include "ama_types.h"
#include "AppAudioStream.h"

using std::shared_ptr;

class AudioFocusObserver;

class AppAudioServiceImpl
{
public:
    static AppAudioServiceImpl* GetInstance(void);
    virtual ~AppAudioServiceImpl();
    ama_AMPID_t getAMpid(void);

    ama_streamID_t createNewStream(ama_streamType_t streamType);
    ama_streamID_t createNewStream(ama_streamType_t streamType,AppAudioStream* pAppAudioStream);
    ama_streamID_t createNewStream(ama_seatID_t seatID,ama_appID_t appID,ama_streamType_t streamType,AppAudioStream* pAppAudioStream);
    void deleteStream(ama_streamID_t streamID);
    ama_Error_e requestAudioDeviceFocus(const ama_connectID_t connectID);
    ama_Error_e releaseAudioDeviceFocus(const ama_connectID_t connectID);
    ama_Error_e setStreamMuteState(const ama_streamID_t streamID,bool isMute);

    void requestToUseSpeaker(const ama_seatID_t seatID);
    void abandonToUseSpeaker(const ama_seatID_t seatID);

    void OnAudioDeviceFocusChangedNotify(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);

    void printAppAudioStreamMap(void);

    void updateVisibleSta(ama_AMPID_t ampid,bool isVisible);

    void attach(AudioFocusObserver *pObserver);
    void detach(AudioFocusObserver *pObserver);
private:
    AppAudioServiceImpl();
    static AppAudioServiceImpl* mpInstance;
    bool generateNewStream(ama_streamID_t streamIDWithNoIndex,ama_streamID_t& newStreamID);
    ama_AMPID_t mAMPID;
    AppManager* pAppManager;
    std::map<ama_streamID_t,AppAudioStream*> mAppAudioStreamMap;

    void notifyFocusStaChangedByCurSta(ama_streamID_t applyStreamID);
    void notifyFocusStaToObserver(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
    void notifyAppVisibleStaToObserver(ama_AMPID_t ampid,bool isVisible);
    AppAudioStream* mpVideoStream;
    ama_focusSta_t mVideoFocusSta;
    bool mIsVisible;

    std::list<AudioFocusObserver *> mObserverList;
};

class AudioFocusObserver
{
public:
    AudioFocusObserver(){
        AppAudioServiceImpl::GetInstance()->attach(this);
    }
    ~AudioFocusObserver(){
        AppAudioServiceImpl::GetInstance()->detach(this);
    }

    virtual void OnAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)=0;
    virtual void OnAppVisibleStaChanged(ama_AMPID_t ampid,bool isVisible)=0;
};

#endif //APPAUDIOSERVICEIMPL_H
