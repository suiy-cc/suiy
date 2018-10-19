/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLCAPITestCase.cpp
/// @brief contains
///
/// Created by zs on 2017/09/14.
///

#include <AppSDK.h>
#include <tools.h>
#include <algorithm>
#include <thread>

#include "appSDKTestCase.h"
#include "PLCAPITestCase.h"
#include "HomePLC.h"
#include "TestAPI.h"

void PowerStateChangeCallBack(bool isChanged){
    logVerbose(SDKTEST,"PowerStateChangeCallBack()-->IN isChanged:",isChanged);
}

void ACCStateChangeCallback(bool isChanged)
{
    logVerbose(SDKTEST,"ACCStateChangeCallback()-->IN isChanged:",isChanged);
}

void ShutdownCallback(unsigned int){
    logVerbose(SDKTEST,"ShutdownCallback()-->IN");
}

void PowerStateChangeExCallBack(ama_PowerState_e, unsigned int){
    logVerbose(SDKTEST,"PowerStateChangeExCallBack()-->IN");
}

void changeOfPowerStateCallBack(ama_changeOfPowerSta_e, unsigned int){
    logVerbose(SDKTEST,"changeOfPowerStateCallBack()-->IN");
}

void IntentCallBack(const std::string&, const std::string&){
    logVerbose(SDKTEST,"IntentCallBack()-->IN");
}

void loadPLCAPITestCase(void)
{
    TestCase::GetInstance()->addTestCase(testCase_ShowMyself);
    TestCase::GetInstance()->addTestCase(    testCase_HideMyself);
    TestCase::GetInstance()->addTestCase(testCase_StartHome);
    TestCase::GetInstance()->addTestCase(    testCase_StartApp);
    TestCase::GetInstance()->addTestCase(testCase_Intent);
    TestCase::GetInstance()->addTestCase(    testCase_Intent2);
    TestCase::GetInstance()->addTestCase(testCase_Intent3);
    TestCase::GetInstance()->addTestCase(    testCase_Intent4);
    TestCase::GetInstance()->addTestCase(testCase_getScreenShot);
    TestCase::GetInstance()->addTestCase(    testCase_getCurrentApp);
    // TestCase::GetInstance()->addTestCase(testCase_refreshScreenShot);
    TestCase::GetInstance()->addTestCase(    testCase_SetLCDTimerStop);
    TestCase::GetInstance()->addTestCase(testCase_SetLCDPowerOff);
    TestCase::GetInstance()->addTestCase(    testCase_AppPowerAwakeRequest);
    TestCase::GetInstance()->addTestCase(testCase_SetBrightness);
    TestCase::GetInstance()->addTestCase(    testCase_GetAMPID);
    TestCase::GetInstance()->addTestCase(testCase_GetAMPID2);
    TestCase::GetInstance()->addTestCase(    testCase_GetItemID);
    TestCase::GetInstance()->addTestCase(testCase_GetItemID2);
    TestCase::GetInstance()->addTestCase(    testCase_GetSurfaceID);
    TestCase::GetInstance()->addTestCase(testCase_GetUnderLayerSurfaceID);
    TestCase::GetInstance()->addTestCase(    testCase_GetBKGLayerSurfaceID);
    TestCase::GetInstance()->addTestCase(testCase_ListenToPowerStateChangeNotify);
    TestCase::GetInstance()->addTestCase(    testCase_ListenToACCStateChangeNotify);
    TestCase::GetInstance()->addTestCase(testCase_ListenToShutdownNotify);
    TestCase::GetInstance()->addTestCase(    testCase_ListenToPowerStateChangeExNotify);
    TestCase::GetInstance()->addTestCase(testCase_ListenToChangeOfPowerStateNotify);
    TestCase::GetInstance()->addTestCase(    testCase_ListenToIntentNotify);

    TestCase::GetInstance()->addTestCase(    testCase_SetLCDScreenOn);
    TestCase::GetInstance()->addTestCase(    testCase_requestPowerAwakeByApp);
    TestCase::GetInstance()->addTestCase(    testCase_requestScreenAwakeByApp);
    TestCase::GetInstance()->addTestCase(    testCase_USBStateController);
    TestCase::GetInstance()->addTestCase(    testCase_RequestAppAlwaysOn);
    TestCase::GetInstance()->addTestCase(    testCase_RequestLCDTemperature);
    // TestCase::GetInstance()->addTestCase(testCase_StartValidApp);
}

bool testCase_ShowMyself()
{
    logVerbose(SDKTEST,"testCase_ShowMyself()-->IN");
    ShowMyself();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_HideMyself()
{
    logVerbose(SDKTEST,"testCase_HideMyself()-->IN");
    HideMyself();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_StartHome()
{
    logVerbose(SDKTEST,"testCase_StartHome()-->IN");
    StartHome();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_StartApp()
{
    logVerbose(SDKTEST,"testCase_StartApp()-->IN");

    StartApp(getRandomAMPID(), getRand(-100, 100));

    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_Intent()
{
    logVerbose(SDKTEST,"testCase_Intent()-->IN");
    Intent(getRandomAMPID(), "cook", "meal");
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_Intent2()
{
    logVerbose(SDKTEST,"testCase_Intent2()-->IN");
    Intent(GetItemID(getRandomAMPID()), "have", "dinner");
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_Intent3()
{
    logVerbose(SDKTEST,"testCase_Intent3()-->IN");
    Intent(getRandomAMPID(), "say", "hello", getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_Intent4()
{
    logVerbose(SDKTEST,"testCase_Intent4()-->IN");
    Intent(GetItemID(getRandomAMPID()), "watch", "TV", getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_getScreenShot()
{
    logVerbose(SDKTEST,"testCase_getScreenShot()-->IN");
    char path[1000];
    getScreenShot(getRand(1,3), path);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_getCurrentApp()
{
    logVerbose(SDKTEST,"testCase_getCurrentApp()-->IN");
    getCurrentApp(getRand(1,3));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_refreshScreenShot()
{
    logVerbose(SDKTEST,"testCase_refreshScreenShot()-->IN");
    refreshScreenShot(getRand(1,3), "/tmp");
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_SetLCDTimerStop()
{
    logVerbose(SDKTEST,"testCase_SetLCDTimerStop()-->IN");
    SetLCDTimerStop(getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_SetLCDPowerOff()
{
    logVerbose(SDKTEST,"testCase_SetLCDPowerOff()-->IN");
    SetLCDPowerOff();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_AppPowerAwakeRequest()
{
    logVerbose(SDKTEST,"testCase_AppPowerAwakeRequest()-->IN");
    AppPowerAwakeRequest(getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_SetBrightness()
{
    logVerbose(SDKTEST,"testCase_SetBrightness()-->IN");
    SetBrightness(getRand(-1000,1000), static_cast<E_SEAT>(getRand(-1000, 1000)));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetAMPID()
{
    logVerbose(SDKTEST,"testCase_GetAMPID()-->IN");
    GetAMPID();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetAMPID2()
{
    logVerbose(SDKTEST,"testCase_GetAMPID2()-->IN");
    GetAMPID(GetItemID(getRandomAMPID()));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetItemID()
{
    logVerbose(SDKTEST,"testCase_GetItemID()-->IN");
    GetItemID();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetItemID2()
{
    logVerbose(SDKTEST,"testCase_GetItemID2()-->IN");
    GetItemID(getRandomAMPID());
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetSurfaceID()
{
    logVerbose(SDKTEST,"testCase_GetSurfaceID()-->IN");
    GetSurfaceID();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetUnderLayerSurfaceID()
{
    logVerbose(SDKTEST,"testCase_GetUnderLayerSurfaceID()-->IN");
    GetUnderLayerSurfaceID();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_GetBKGLayerSurfaceID()
{
    logVerbose(SDKTEST,"testCase_GetBKGLayerSurfaceID()-->IN");
    GetBKGLayerSurfaceID();
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_ListenToPowerStateChangeNotify()
{
    logVerbose(SDKTEST,"testCase_ListenToPowerStateChangeNotify()-->IN");
    ListenToPowerStateChangeNotify(PowerStateChangeCallBack);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_ListenToACCStateChangeNotify()
{
    logVerbose(SDKTEST,"testCase_ListenToACCStateChangeNotify()-->IN");
    ListenToACCStateChangeNotify(ACCStateChangeCallback);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_ListenToShutdownNotify()
{
    logVerbose(SDKTEST,"testCase_ListenToShutdownNotify()-->IN");
    ListenToShutdownNotify(ShutdownCallback);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_ListenToPowerStateChangeExNotify()
{
    logVerbose(SDKTEST,"testCase_ListenToPowerStateChangeExNotify()-->IN");
    ListenToPowerStateChangeExNotify(PowerStateChangeExCallBack);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_ListenToChangeOfPowerStateNotify()
{
    logVerbose(SDKTEST,"testCase_ListenToChangeOfPowerStateNotify()-->IN");
    ListenToChangeOfPowerStateNotify(changeOfPowerStateCallBack);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_ListenToIntentNotify()
{
    logVerbose(SDKTEST,"testCase_ListenToIntentNotify()-->IN");
    ListenToIntentNotify(IntentCallBack);
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_StartValidApp()
{
    AMPID originalApp = GetAMPID();
    Mock(0x1001);
    auto apps1 = GetAppListEx();
    Mock(0x2001);
    auto apps2 = GetAppListEx();
    Mock(0x3001);
    auto apps3 = GetAppListEx();
    Mock(originalApp);
    std::vector<AppPKGEx> apps;
    apps.insert(apps.begin(), apps1.begin(), apps1.end());
    apps.insert(apps.begin(), apps2.begin(), apps2.end());
    apps.insert(apps.begin(), apps3.begin(), apps3.end());
    for(int i = 0; i < 1000; ++i)
    {
        std::random_shuffle(apps.begin(), apps.end());
        for(auto& app: apps)
        {
            if(app.GetBigIconPath().empty())
                continue;

            app.Start();
            std::cout << "start app 0x" << std::hex << app.GetAmpid() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_StartEVApp(int startCnt)
{
    AMPID originalApp = GetAMPID();
    Mock(0x1001);
    auto apps1 = GetAppListEx();
    Mock(originalApp);
    std::vector<AppPKGEx> apps;
    apps.insert(apps.begin(), apps1.begin(), apps1.end());

    //start all app
    std::cout << "start all apps" << std::endl;
    for(auto& app: apps)
    {
        if(app.GetBigIconPath().empty())
            continue;

        app.Start();
        std::cout << "start app 0x" << std::hex << app.GetAmpid() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5*1000));

        if(!isAppMgrExist()){
            return false;
        }
    }

    std::cout << "start teat all app switch" << std::endl;
    for(int i = 0; i < startCnt; ++i)
    {
        std::random_shuffle(apps.begin(), apps.end());
        for(auto& app: apps)
        {
            if(app.GetBigIconPath().empty())
                continue;

            app.Start();
            std::cout << "start app 0x" << std::hex << app.GetAmpid() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            if(!isAppMgrExist()){
                return false;
            }
        }
    }

    return isAppMgrExist();
}

extern void StopApp(unsigned int app);

bool testCase_StartStopRadomApp(int testCnt,int time_ms)
{
    AMPID originalApp = GetAMPID();
    Mock(0x1001);
    auto apps1 = GetAppListEx();
    Mock(originalApp);
    std::vector<AppPKGEx> apps;
    apps.insert(apps.begin(), apps1.begin(), apps1.end());

    //start all app
    std::cout << "start all apps" << std::endl;
    for(auto& app: apps)
    {
        if(app.GetBigIconPath().empty())
            continue;

        app.Start();
        std::cout << "start app 0x" << std::hex << app.GetAmpid() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5*1000));

        if(!isAppMgrExist()){
            return false;
        }
    }

    std::cout << "start test start or stop radom app" << std::endl;
    for(int i = 0; i < testCnt; ++i)
    {
        int radomIndex = getRand(0, apps.size()-1);
        int radomAmpid = apps.at(radomIndex).GetAmpid();
        if(i%2){
            std::cout << "start rand app 0x" << std::hex << radomAmpid << std::endl;
            StartApp(radomAmpid,0);
            usleep(time_ms*1000);
        }else{
            std::cout << "stop rand app 0x" << std::hex << radomAmpid << std::endl;
            StopApp(radomAmpid);
            usleep(time_ms*1000);
        }
    }

    return isAppMgrExist();
}

bool testCase_StartStopApp(int ampid,int testCnt,int time_ms)
{
    std::cout << "start test start or stop radom app" << std::endl;
    for(int i = 0; i < testCnt; ++i)
    {
        if(i%2){
            std::cout << "start app 0x"<< std::hex  << ampid << std::endl;
            StartApp(ampid,0);
            usleep(time_ms*1000);
        }else{
            std::cout << "stop app 0x"<< std::hex  << ampid << std::endl;
            StopApp(ampid);
            usleep(time_ms*1000);
        }
    }

    return isAppMgrExist();
}

bool testCase_SetLCDScreenOn()
{
    logVerbose(SDKTEST,"testCase_SetLCDScreenOn()-->IN");
    SetLCDScreenOn((E_SEAT)getRand((int)E_SEAT_IVI,(int)E_SEAT_RSE2));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_requestPowerAwakeByApp()
{
    logVerbose(SDKTEST,"testCase_requestPowerAwakeByApp()-->IN");
    requestPowerAwakeByApp((bool)getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_requestScreenAwakeByApp()
{
    logVerbose(SDKTEST,"testCase_requestScreenAwakeByApp()-->IN");
    requestScreenAwakeByApp((bool)getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_USBStateController()
{
    logVerbose(SDKTEST,"testCase_USBStateController()-->IN");
    USBStateController((ama_USBOperate)getRand((int)E_USB_OTG_CHAGRE_ON,(int)E_USB_HOST_CHARGE_OFF));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_RequestAppAlwaysOn()
{
    logVerbose(SDKTEST,"testCase_RequestAppAlwaysOn()-->IN");
    RequestAppAlwaysOn((bool)getRand(0,1));
    delayAfterCallAPI();
    return isAppMgrExist();
}

bool testCase_RequestLCDTemperature()
{
    logVerbose(SDKTEST,"testCase_RequestLCDTemperature()-->IN");
    RequestLCDTemperature();
    delayAfterCallAPI();
    return isAppMgrExist();
}
