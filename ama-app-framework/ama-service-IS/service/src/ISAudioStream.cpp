/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "ISAudioStream.h"
#include "logDef.h"
#include "ISTask.h"
#include "ISCtrl.h"

ISAudioStream::ISAudioStream(ama_streamType_t streamType,ama_audioDeviceType_t deviceType)
:AppAudioStream(deviceType,IS_APPID,streamType)
,mAudioDeviceType(deviceType)
{
    logVerbose(IS,"ISAudioStream::ISAudioStream()-->>in");
    logVerbose(IS,"ISAudioStream::ISAudioStream()-->>out");
}

ISAudioStream::~ISAudioStream()
{
    logVerbose(IS,"ISAudioStream::ISAudioStream()-->>in");

    logVerbose(IS,"ISAudioStream::ISAudioStream()-->>out");
}

ama_connectID_t ISAudioStream::getConnectID(void)
{
    logVerbose(IS,"ISAudioStream::OnAudioFocusChangedForDefDev()-->>in");

    mConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID(),mAudioDeviceType);

    logVerbose(IS,"ISAudioStream::OnAudioFocusChangedForDefDev()-->>out");
    return mConnectID;
}

void ISAudioStream::OnAudioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta)
{
    logVerbose(IS,"ISAudioStream::OnAudioFocusChanged()-->>in");

    mConnectID = CREATE_CONNECTID_BY_STREAMID_DEVICETYPE(streamID(),mAudioDeviceType);

    auto pTask = std::make_shared<ISTask>();
    if(E_FOUCS_GAIN==newFocusSta){
        pTask->SetType(GET_AUDIO_FOCUS);
    }else if(E_FOUCS_LOSE==newFocusSta){
        pTask->SetType(LOSS_AUDIO_FOCUS);
    }
    pTask->setConnectID(mConnectID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(IS,"ISAudioStream::OnAudioFocusChanged()-->>out");
}
