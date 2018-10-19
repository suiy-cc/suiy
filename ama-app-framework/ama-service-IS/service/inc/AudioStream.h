/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef AUDIOSTREAM_H_
#define AUDIOSTREAM_H_

#include "AppAudioStream.h"

class ISAudioStream
: public AppAudioStream
{
public:
    ISAudioStream(ama_streamType_t streamType,ama_audioDeviceType_t deviceType);
    ~ISAudioStream();
    ama_connectID_t getConnectID(void);
private:
    void OnAudioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta);

    ama_audioDeviceType_t mAudioDeviceType;
    ama_connectID_t mConnectID;
};

#endif //AUDIOSTREAM_H_
