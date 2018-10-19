/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppIS.h"
#include "InformingSoundImpl.h"

void informingSoundInit()
{
    InformingSoundImpl::GetInstance()->init();
}

void playSoftKeySound(ama_audioDeviceType_t deviceType)
{
    InformingSoundImpl::GetInstance()->playSoftKeySound(deviceType);
}

void playPromptSound(ama_audioDeviceType_t deviceType)
{
    InformingSoundImpl::GetInstance()->playPromptSound(deviceType);
}

void playBackdoorPromptSound(void)
{
    InformingSoundImpl::GetInstance()->playBackdoorPromptSound();
}

void playFeedbackSound(int volumeType,int volume)
{
    //InformingSoundImpl::GetInstance()->playFeedbackSound(volumeType,volume);
}

void playRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel)
{
    InformingSoundImpl::GetInstance()->playRadarSound(soundType,highestLevelChannel);
}

void startPlayAVMAlertSound(void)
{
    InformingSoundImpl::GetInstance()->playAVMAlertSound(true);
}

void stopPlayAVMAlertSound(void)
{
    InformingSoundImpl::GetInstance()->playAVMAlertSound(false);
}
