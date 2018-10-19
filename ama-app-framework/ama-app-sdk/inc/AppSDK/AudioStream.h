/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <string>
#include <memory>

#include "ama_audioTypes.h"
#include "ama_types.h"

using namespace std;

typedef void (*audioFocusChangedCB)(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);

class AudioStream
{
public:
    AudioStream(void* pAudioStreamImpl);
    virtual ~AudioStream();

    ama_streamID_t streamID(void);
    string streamIDStr(void);

    ama_focusSta_t requestAudioFocus(audioFocusChangedCB func,ama_audioDeviceType_t audioDevice=E_IVI_SPEAKER);
    ama_focusSta_t abandonAudioFocus(ama_audioDeviceType_t audioDevice=E_IVI_SPEAKER);

    ama_focusSta_t getFocusState(ama_audioDeviceType_t audioDevice);

    ama_Error_e setStreamMuteState(bool isMute);//for all device
protected:
    friend class AudioStreamImpl;
private:
    void* mpAudioStreamImpl;
};

AudioStream* createAudioStream(ama_streamType_t streamType,ama_seatID_t seatIDOfSource=E_SEAT_IVI);
void deleteAudioStream(AudioStream * pAudioStream);

#endif //AUDIOSTREAM_H
