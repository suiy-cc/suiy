/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef PLCAPITESTCASE_H
#define PLCAPITESTCASE_H

void loadPLCAPITestCase(void);

bool testCase_ShowMyself();
bool testCase_HideMyself();
bool testCase_StartHome();
bool testCase_StartApp();
bool testCase_Intent();
bool testCase_Intent2();
bool testCase_Intent3();
bool testCase_Intent4();
bool testCase_getScreenShot();
bool testCase_getCurrentApp();
bool testCase_refreshScreenShot();
bool testCase_SetLCDTimerStop();
bool testCase_SetLCDPowerOff();
bool testCase_SetLCDScreenOn();
bool testCase_AppPowerAwakeRequest();
bool testCase_requestPowerAwakeByApp();
bool testCase_requestScreenAwakeByApp();
bool testCase_SetBrightness();
bool testCase_USBStateController();
bool testCase_RequestAppAlwaysOn();
bool testCase_RequestLCDTemperature();
bool testCase_GetAMPID();
bool testCase_GetAMPID2();
bool testCase_GetItemID();
bool testCase_GetItemID2();
bool testCase_GetSurfaceID();
bool testCase_GetUnderLayerSurfaceID();
bool testCase_GetBKGLayerSurfaceID();
bool testCase_ListenToPowerStateChangeNotify();
bool testCase_ListenToACCStateChangeNotify();
bool testCase_ListenToShutdownNotify();
bool testCase_ListenToPowerStateChangeExNotify();
bool testCase_ListenToChangeOfPowerStateNotify();
bool testCase_ListenToIntentNotify();
bool testCase_StartValidApp();
bool testCase_StartEVApp(int startCnt);
bool testCase_StartStopRadomApp(int testCnt,int time_ms);
bool testCase_StartStopApp(int ampid,int testCnt,int time_ms);

#endif// PLCAPITESTCASE_H
