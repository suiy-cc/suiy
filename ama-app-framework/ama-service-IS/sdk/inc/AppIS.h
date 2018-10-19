/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APPIS_H
#define APPIS_H

#include "ama_types.h"
#include "ama_audioTypes.h"

void informingSoundInit();

void playSoftKeySound(ama_audioDeviceType_t deviceType);
void playPromptSound(ama_audioDeviceType_t deviceType);
void playBackdoorPromptSound(void);
void playFeedbackSound(int volumeType,int volume);
void playRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel);

void startPlayAVMAlertSound(void);
void stopPlayAVMAlertSound(void);

#endif //APPIS_H
