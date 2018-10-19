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
#include "powerAPITestCase.h"

static void powerStateChangedCallback(uint8_t seatID,ama_PowerState_e powerState)
{
    logVerbose(SDKTEST,"powerStateChangedCB()-->IN");
}

static void changeOfPowerStaChangedCallback(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta)
{
    logVerbose(SDKTEST,"changeOfPowerStaChangedCB()-->IN");
}

static void changeOfPowerStaChangedCallbackWithIndex(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta,uint32_t changeIndex)
{
    logVerbose(SDKTEST,"changeOfPowerStaChangedCBWithIndex()-->IN");
}

void loadPowerAPITestCase(void){
    powerSDKInit();

    registerPowerStateChangedCB(powerStateChangedCallback);
    registerChangeOfPowerStaChangedCB(changeOfPowerStaChangedCallback);
    registerChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCallbackWithIndex);

    TestCase::GetInstance()->addTestCase(testCase_checkIsPowerOn);
    TestCase::GetInstance()->addTestCase(testCase_checkIsACCOn);
    TestCase::GetInstance()->addTestCase(testCase_getPowerState);
    TestCase::GetInstance()->addTestCase(testCase_requestPowerGoodbye);
}

bool testCase_checkIsPowerOn(void){
    logVerbose(SDKTEST,"testCase_reqSoftKeySound()-->IN");
    reqSoftKeySound();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_checkIsACCOn(void){
    logVerbose(SDKTEST,"testCase_reqSoftKeySound()-->IN");
    reqSoftKeySound();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_getPowerState(void){
    logVerbose(SDKTEST,"testCase_reqSoftKeySound()-->IN");
    reqSoftKeySound();
    delayAfterCallAPI();

    return isAppMgrExist();
}

bool testCase_requestPowerGoodbye(void){
    logVerbose(SDKTEST,"testCase_reqSoftKeySound()-->IN");
    reqSoftKeySound();
    delayAfterCallAPI();

    return isAppMgrExist();
}
