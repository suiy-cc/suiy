/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <AppSDK.h>
#include <log.h>

#include "tools.h"
#include "appSDKTestCase.h"
#include "audioAPITestCase.h"
#include "audioTools.h"

#include <vector>

LogContext TEST_AUDIO;
static std::vector<AppStream*> AllAppStreamVec;

static void changeOfAudioFocusCallback(ama_connectID_t connectID, ama_focusSta_t focusSta)
{
    logVerbose(SDKTEST,"changeOfAudioFocusCB()-->IN");
}

void loadAudioAPITestCase(void){
    RegisterContext(TEST_AUDIO, "TADO");
    createAllAppStream();
    ListenTochangeOfAudioFocus(changeOfAudioFocusCallback);
    TestCase::GetInstance()->addTestCase(testCase_reqSoftKeySound);
    TestCase::GetInstance()->addTestCase(testCase_reqPromptSound);
    TestCase::GetInstance()->addTestCase(testCase_reqBackdoorPromptSound);
    TestCase::GetInstance()->addTestCase(testCase_reqToPlayRadarSound);
    TestCase::GetInstance()->addTestCase(testCase_requestToUseSpeaker);
    TestCase::GetInstance()->addTestCase(testCase_abandonToUseSpeaker);
    TestCase::GetInstance()->addTestCase(testCase_getLastConnectMediaApp);
    TestCase::GetInstance()->addTestCase(testCase_isVolumeTypeActive);
    TestCase::GetInstance()->addTestCase(testCase_duckAudio);
    TestCase::GetInstance()->addTestCase(testCase_unduckAudio);

    TestCase::GetInstance()->addTestCase(testCase_streamID);
    TestCase::GetInstance()->addTestCase(testCase_streamIDStr);
    TestCase::GetInstance()->addTestCase(testCase_requestAudioFocusForDefDev);
    TestCase::GetInstance()->addTestCase(testCase_requestAudioFocusForDev);
    TestCase::GetInstance()->addTestCase(testCase_abandonAudioFocusForDefDev);
    TestCase::GetInstance()->addTestCase(testCase_abandonAudioFocusForDev);
    TestCase::GetInstance()->addTestCase(testCase_setStreamMuteState);
}

bool testCase_reqSoftKeySound(void){
    logVerbose(SDKTEST,"testCase_reqSoftKeySound()-->IN");
    reqSoftKeySound();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_reqPromptSound(void){
    logVerbose(SDKTEST,"testCase_reqPromptSound()-->IN");
    reqSoftKeySound();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_reqBackdoorPromptSound(void){
    logVerbose(SDKTEST,"testCase_reqBackdoorPromptSound()-->IN");
    reqBackdoorPromptSound();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_reqToPlayRadarSound(void){
    logVerbose(SDKTEST,"testCase_reqToPlayRadarSound()-->IN");
    reqToPlayRadarSound(getRand(E_NO_TONE,E_NORMAL_TONE),getRand(E_NO_CHANNEL,E_RR_CHANNEL)|getRand(E_NO_CHANNEL,E_RR_CHANNEL));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_requestToUseSpeaker(void){
    logVerbose(SDKTEST,"testCase_requestToUseSpeaker()-->IN");
    requestToUseSpeaker(getRand(E_SEAT_IVI,E_SEAT_RSE2));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_abandonToUseSpeaker(void){
    logVerbose(SDKTEST,"testCase_abandonToUseSpeaker()-->IN");
    abandonToUseSpeaker(getRand(E_SEAT_IVI,E_SEAT_RSE2));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_getLastConnectMediaApp(void){
    logVerbose(SDKTEST,"testCase_getLastConnectMediaApp()-->IN");
    getLastConnectMediaApp(getRand(E_IVI_SPEAKER,E_RSE_R_HEADSET));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_isVolumeTypeActive(void){
    logVerbose(SDKTEST,"testCase_isVolumeTypeActive()-->IN");
    isVolumeTypeActive(getRand(E_MediaVolMixWithNavi,E_ECallVol));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_duckAudio(void){
    logVerbose(SDKTEST,"testCase_duckAudio()-->IN");
    duckAudio(500,20);
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_unduckAudio(void){
    logVerbose(SDKTEST,"testCase_unduckAudio()-->IN");
    unduckAudio(500);
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_streamID(void)
{
    logVerbose(SDKTEST,"testCase_streamID()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->streamID();
    delayAfterCallAPI();

    return isAppMgrExist();
}
bool testCase_streamIDStr(void){
    logVerbose(SDKTEST,"testCase_streamIDStr()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->streamIDStr();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_requestAudioFocusForDefDev(void){
    logVerbose(SDKTEST,"testCase_requestAudioFocusForDefDev()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->requestAudioFocusForDefDev();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_requestAudioFocusForDev(void){
    logVerbose(SDKTEST,"testCase_requestAudioFocusForDev()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->requestAudioFocusForDev(getRand(E_IVI_MIC,E_RSE_R_HEADSET));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_abandonAudioFocusForDefDev(void){
    logVerbose(SDKTEST,"testCase_abandonAudioFocusForDefDev()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->abandonAudioFocusForDefDev();
    delayAfterCallAPI();

    return isAppMgrExist();
}


bool testCase_abandonAudioFocusForDev(void){
    logVerbose(SDKTEST,"testCase_abandonAudioFocusForDev()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->abandonAudioFocusForDev(getRand(E_IVI_MIC,E_RSE_R_HEADSET));
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_setStreamMuteState(void){
    logVerbose(SDKTEST,"testCase_setStreamMuteState()-->IN");
    int index = getRand(0,(AllAppStreamVec.size()-1));
    AppStream* pAppStream = AllAppStreamVec.at(index);
    pAppStream->setStreamMuteState(getRand(0,1));
    delayAfterCallAPI();

    return isAppMgrExist();
}



void createAllAppStream(void){
    logVerbose(SDKTEST,"createAllAppStream()-->IN");
    AppStream* pAppStream = new AppStream(E_STREAM_TUNER);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_MUSIC);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_VIDEO_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_BT_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_PHONELINK_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_3THAPP_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_FM);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_AM);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_IPOD_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_CARPLAY_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_CARLIFE_AUDIO);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_SMS_TTS);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_VR_TTS);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_NAVI_TTS);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_CALL_RING);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_B_CALL);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_I_CALL);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_PHONE_MIC);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_VR_MIC);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_B_CALL_MIC);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_I_CALL_MIC);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_E_CALL_MIC);
    AllAppStreamVec.push_back(pAppStream);

    pAppStream = new AppStream(E_STREAM_LOCAL_VR_MIC);
    AllAppStreamVec.push_back(pAppStream);
}
