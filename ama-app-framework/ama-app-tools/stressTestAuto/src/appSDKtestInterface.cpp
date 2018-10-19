/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "appSDKtestInterface.h"


int getAppMgrPid_t(void){
    s_appMgrPid = getPidOfProc("ama-app-manager");
    return s_appMgrPid;
}

void loadXXXModuleTestCase_t(int moduleDef){
    switch(moduleDef){
    case E_UNLOAD_ALL_MODULE_TEST_CASE:
        TestCase::GetInstance()->clearTestCase();
        break;
    case E_LOAD_PLC_MODULE_TEST_CASE:
        loadPLCAPITestCase();
        break;
    case E_LOAD_AUDIO_MODULE_TEST_CASE:
        loadAudioAPITestCase();
        break;
    case E_LOAD_OTHER_MODULE_TEST_CASE:
        loadOtherAPITestCase();
        break;
    case E_LOAD_POWER_MODULE_TEST_CASE:
        loadPowerAPITestCase();
        break;
    case E_LOAD_ALL_MODULE_TEST_CASE:
        loadAudioAPITestCase();
        loadPLCAPITestCase();
        loadOtherAPITestCase();
        loadPowerAPITestCase();
        break;
    default:
        printf("moduleDef is not defined!\n");
        break;
    }
}

int getAppSDKTestCaseNum_t(void){
    return TestCase::GetInstance()->getCurCaseNum();
}

bool runAppSDKTestCaseByIndex_t(int index){
    return TestCase::GetInstance()->execTestCase(index);
}
