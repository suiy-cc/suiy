/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppAudio.h
/// @brief contains API of audio
///
/// Created by wq on 2016/8/3.
/// this file contains API of audio
///

#ifndef APPAUDIO_H
#define APPAUDIO_H

#include "ama_types.h"
#include "ama_audioTypes.h"

/**
* @brief request softKey sound
* @param [in]  void
* @return void.
*/
void reqSoftKeySound(void);

/**
* @brief request prompt sound
* @param [in]  void
* @return void.
*/
void reqPromptSound(void);

/**
* @brief request backdoor remember successed prompt sound
* @param [in]  void
* @return void.
*/
void reqBackdoorPromptSound(void);

/**
* @brief request radar sound for FrontLeft FrontRight RearLeft RearRight
* @param [in]  void
* @return void.
* @exsample: reqToPlayRadarSound(E_HIGH_FREQ_TONE,E_FL_CHANNEL | E_FR_CHANNEL);
*/
void reqToPlayRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel);

/**
* @brief request Speaker user
* @param [in]  void
* @return void.
*/
void requestToUseSpeaker(ama_seatID_t seatID);
void abandonToUseSpeaker(ama_seatID_t seatID);

/**
* @brief get Last disconnect Media App
* @param [in]  deviceID
* @return ama_AMPID_t.
*/
ama_AMPID_t getLastConnectMediaApp(ama_audioDeviceType_t deviceID);

/**
* @brief register all connect focus changed callback
* @param [in]  changeOfAudioFocusCB
* @return void.
*/
typedef void (*changeOfAudioFocusCB)(ama_connectID_t, ama_focusSta_t);
void ListenTochangeOfAudioFocus(changeOfAudioFocusCB f);

typedef void (*changeOfAudioFocusCBEx)(ama_connectID_t connectID,ama_focusSta_t oldFocusSta,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
void ListenTochangeOfAudioFocusEx(changeOfAudioFocusCBEx f);

bool isVolumeTypeActive(int volumeType);

//they are interface for carplay app
void duckAudio(int durationsms,int volume);
void unduckAudio(int durationsms);

#endif //APPAUDIO_H
