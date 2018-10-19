/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AUDIOAPITESTCASE_H
#define AUDIOAPITESTCASE_H

void loadAudioAPITestCase(void);

bool testCase_reqSoftKeySound(void);
bool testCase_reqPromptSound(void);
bool testCase_reqBackdoorPromptSound(void);
bool testCase_reqToPlayRadarSound(void);
bool testCase_requestToUseSpeaker(void);
bool testCase_abandonToUseSpeaker(void);
bool testCase_getLastConnectMediaApp(void);
bool testCase_isVolumeTypeActive(void);
bool testCase_duckAudio(void);
bool testCase_unduckAudio(void);

void createAllAppStream(void);

bool testCase_streamID(void);
bool testCase_streamIDStr(void);
bool testCase_requestAudioFocusForDefDev(void);
bool testCase_requestAudioFocusForDev(void);
bool testCase_abandonAudioFocusForDefDev(void);
bool testCase_abandonAudioFocusForDev(void);
bool testCase_setStreamMuteState(void);
bool testCase_duckAudio(void);
bool testCase_unduckAudio(void);


#endif//AUDIOAPITESTCASE_H
