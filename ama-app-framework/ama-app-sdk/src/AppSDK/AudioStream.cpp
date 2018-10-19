/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "AudioStream.h"
#include "AudioStreamImpl.h"
#include "AppAudioServiceImpl.h"
#include "LogContext.h"

AudioStream::AudioStream(void* pAudioStreamImpl)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    mpAudioStreamImpl = pAudioStreamImpl;
}

AudioStream::~AudioStream()
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);
    //DomainLog chatter(SDK_AC, LOG_VBOS,"AudioStream::"+string(__FUNCTION__)+"()"));
    delete(mpAudioStreamImpl);
}

ama_streamID_t AudioStream::streamID(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    auto pAudioStream = static_cast<AudioStreamImpl*>(mpAudioStreamImpl);
    if(NULL != pAudioStream){
        return pAudioStream->streamID();
    }else{
        return 0x0;
    }
}

string AudioStream::streamIDStr(void)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    auto pAudioStream = static_cast<AudioStreamImpl*>(mpAudioStreamImpl);
    if(NULL != pAudioStream){
        return pAudioStream->streamIDStr();
    }else{
        return "0x0";
    }
}

ama_focusSta_t AudioStream::requestAudioFocus(audioFocusChangedCB func,ama_audioDeviceType_t audioDevice)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    auto pAudioStream = static_cast<AudioStreamImpl*>(mpAudioStreamImpl);
    if(NULL != pAudioStream){
        return pAudioStream->requestAudioFocus(audioDevice,func);
    }else{
        return E_FOUCS_LOSE;
    }
}

ama_focusSta_t AudioStream::abandonAudioFocus(ama_audioDeviceType_t audioDevice)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    auto pAudioStream = static_cast<AudioStreamImpl*>(mpAudioStreamImpl);
    if(NULL != pAudioStream){
        return pAudioStream->abandonAudioFocus(audioDevice);
    }else{
        return E_FOUCS_LOSE;
    }
}

ama_focusSta_t AudioStream::getFocusState(ama_audioDeviceType_t audioDevice)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    auto pAudioStream = static_cast<AudioStreamImpl*>(mpAudioStreamImpl);
    if(NULL != pAudioStream){
        return pAudioStream->getFocusState(audioDevice);
    }else{
        return E_FOUCS_LOSE;
    }
}

ama_Error_e AudioStream::setStreamMuteState(bool isMute)//for all device
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    auto pAudioStream = static_cast<AudioStreamImpl*>(mpAudioStreamImpl);
    if(NULL != pAudioStream){
        pAudioStream->setStreamMuteStateSync(isMute);
        return E_OK;
    }else{
        return E_NULL_PTR;
    }
}

AudioStream* createAudioStream(ama_streamType_t streamType,ama_seatID_t seatIDOfSource)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    //seatIDOfSource wasn't be used rightnow!but it's very useful when we request audioFocus in services.
    ama_streamID_t streamID = AppAudioServiceImpl::GetInstance()->createNewStream(streamType);
    AudioStreamImpl* pAudioStreamImpl = new AudioStreamImpl(streamID);
    AudioStream* pAudioSteam = new AudioStream(pAudioStreamImpl);
    return pAudioSteam;
}

void deleteAudioStream(AudioStream * pAudioStream)
{
    FUNCTION_SCOPE_LOG_C(SDK_AC);

    if(pAudioStream){
        delete(pAudioStream);
    }
}
