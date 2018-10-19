/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef INFORMINGSOUNDIMPL_H
#define INFORMINGSOUNDIMPL_H

#include "log.h"
#include "ama_audioTypes.h"
#include <mutex>

extern LOG_DECLARE_CONTEXT(SDK_IS);

class InformingSoundImpl
{
public:
    static InformingSoundImpl* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static InformingSoundImpl instance;
        return &instance;
    }
    ~InformingSoundImpl();

    void init(int tryTimeMs=1);
    void playSoftKeySound(ama_audioDeviceType_t deviceType);
    void playPromptSound(ama_audioDeviceType_t deviceType);
    void playBackdoorPromptSound(void);
    void playFeedbackSound(int volumeType,int volume);
    void playRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel);
    void playAVMAlertSound(bool isPlay);
protected:
private:
    InformingSoundImpl();

private:
    bool mIsInited;
};

#endif//INFORMINGSOUNDIMPL_H
