/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <AppSDK.h>
#include <log.h>
#include <thread>

#include "appSDKTestCase.h"
#include "tools.h"
#include "TestAPI.h"
#include "HomePLC.h"
#include "AppIME.h"

LogContext SDKTEST;
int s_appMgrPid = 0;
static std::vector<ama_AMPID_t> sAllAMPidVec;

TestCase::TestCase(){

}

TestCase::~TestCase(){

}

void TestCase::addTestCase(testCaseFuncDef pFunc){
    mTestCaseVec.push_back(pFunc);
}

void TestCase::clearTestCase(void){
    mTestCaseVec.clear();
}

int TestCase::getCurCaseNum(void){
    return mTestCaseVec.size();
}
int TestCase::execTestCase(int caseIndex){
    if((caseIndex>=0)&&(caseIndex<mTestCaseVec.size())){
        testCaseFuncDef func = mTestCaseVec.at(caseIndex);
        return func();
    }else{
        return 0;
    }
}

void appSDKTestInit(void){

    SDKProperty property;
    property.appLogID = "STA";
    property.appDescription = "stress test auto process";
    // initialize SDK
    AppSDKInit(property);
    RegisterContext(SDKTEST,"SDKT", "sdk test");

    // imeInit();
    // AppSDKInit();
    // RegisterContext(SDKTEST,"SDKT", "sdk test");

    Mock(0x1001);

    logVerbose(SDKTEST,"appSDKTestInit()-->IN");

    getAllAMPID();

    logVerbose(SDKTEST,"appSDKTestInit()-->OUT");
}


bool isAppMgrExist(void){
    if(s_appMgrPid){
        return isProcOfPidExist(s_appMgrPid);
    }else{
        printf("can't find pid of ama-app-manager \n");
        return false;
    }
}

void getAllAMPID(void){
    //get installed app packages
	std::vector<AppPKGEx> pkgList = GetAppListEx();
    ama_AMPID_t ampid = 0;

	for (std::vector<AppPKGEx>::iterator it = pkgList.begin(); it < pkgList.end(); it++){
		logInfo(SDKTEST,"ampid=", it->GetAmpid(), " itemID=", it->GetItemID(), "name=",it->GetAppName());
        if(it->GetItemID().substr(0,12)=="com.hs7.sync"){
            continue;
        }
		ampid = it->GetAmpid();
        sAllAMPidVec.push_back(ampid);
    }
}

void startAllApp(void){
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
    }
}

uint16_t getRandomAMPID(void){
    int index=getRand(0,(sAllAMPidVec.size()-1));
    return sAllAMPidVec.at(index);
}
