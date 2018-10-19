/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppAwakeMgr.h"
#include "AppList.h"
#include "PMCTask.h"
#include "ReadXmlConfig.h"
#include "ID.h"

AppAwakeMgr::AppAwakeMgr()
:mIsPowerAwake(false)
,mIsScreenAwake(false)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");

    std::string configKey="AwakeAppList";
    std::vector<std::string> dataVector;
    GetConfigFromVector(configKey, dataVector);
    for (std::vector<std::string>::iterator iter=dataVector.begin();iter!=dataVector.end();++iter)
    {
        AMPID ampid = AppList::GetInstance()->GetAMPID(*iter);
        mActiveAwakeAppList.push_back(ampid);
        logDebugF(PMC,"mActiveAwakeAppList.push_back:[%x] \n",ampid);
    }

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

AppAwakeMgr::~AppAwakeMgr()
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

void AppAwakeMgr::clearAwakeStateWhenAppCrash(AMPID ampid)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"ampid:",ampid);

    handleScreenAwakeRequest(ampid,false);
    handlePowerAwakeRequest(ampid,false);
    handleAbnormalAwakeRequest(ampid,false);

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

void AppAwakeMgr::handlePowerAwakeRequest(AMPID ampid,bool isPowerAwake)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"ampid:",ampid," isPowerAwake:",isPowerAwake);

    if(checkIsAppCanAwake(ampid)){
        mPowerAwakeAppMap[ampid] = isPowerAwake;
        if(isPowerAwake){
            if(!mIsPowerAwake){
                mIsPowerAwake = true;
                logDebug(PMC,"send E_APP_POWER_AWAKE_ON signal to PMC");
                sendSignalToPMC(E_APP_POWER_AWAKE_ON);
            }

            if(!mIsScreenAwake){
                mIsScreenAwake = true;
                logDebug(PMC,"send E_APP_SCREEN_AWAKE_ON signal to PMC");
                sendSignalToPMC(E_APP_SCREEN_AWAKE_ON);
            }
        }else{//power stop awake
            if(!checkIsPowerAwakeInMap()){
                if(mIsPowerAwake){
                    mIsPowerAwake = false;
                    logDebug(PMC,"send E_APP_POWER_AWAKE_OFF signal to PMC");
                    sendSignalToPMC(E_APP_POWER_AWAKE_OFF);
                }
            }

            if(!checkIsScreenAwakeInMap()){
                if(mIsScreenAwake){
                    mIsScreenAwake = false;
                    logDebug(PMC,"send E_APP_SCREEN_AWAKE_OFF signal to PMC");
                    sendSignalToPMC(E_APP_SCREEN_AWAKE_OFF);
                }
            }
        }
    }

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

void AppAwakeMgr::handleScreenAwakeRequest(AMPID ampid,bool isScreenAwake)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"ampid:",ampid," isScreenAwake:",isScreenAwake);

    if(checkIsAppCanAwake(ampid)){
        mScreenAwakeAppMap[ampid] = isScreenAwake;
        if(isScreenAwake){
            if(!mIsScreenAwake){
                mIsScreenAwake = true;
                logDebug(PMC,"send E_APP_SCREEN_AWAKE_ON signal to PMC");
                sendSignalToPMC(E_APP_SCREEN_AWAKE_ON);
            }
        }else{//screen stop awake
            if(!checkIsScreenAwakeInMap()){
                if(mIsScreenAwake){
                    mIsScreenAwake = false;
                    logDebug(PMC,"send E_APP_SCREEN_AWAKE_OFF signal to PMC");
                    sendSignalToPMC(E_APP_SCREEN_AWAKE_OFF);
                }
            }
        }
    }

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

void AppAwakeMgr::handleAbnormalAwakeRequest(AMPID ampid,bool isAbnormalAwake)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"ampid:",ampid," isScreenAwake:",isAbnormalAwake);

    if(checkIsAppCanAwake(ampid)){
        mAbnormalAwakeAppMap[ampid] = isAbnormalAwake;
        if(isAbnormalAwake){
            if(!mIsAbnormalAwake){
                mIsAbnormalAwake = true;
                logDebug(PMC,"send E_APP_SCREEN_AWAKE_ON signal to PMC");
                sendSignalToPMC(E_APP_ABNORMAL_AWAKE_ON);
            }
        }else{//screen stop awake
            if(!checkIsAbnormalAwakeInMap()){
                if(mIsAbnormalAwake){
                    mIsAbnormalAwake = false;
                    logDebug(PMC,"send E_APP_SCREEN_AWAKE_OFF signal to PMC");
                    sendSignalToPMC(E_APP_ABNORMAL_AWAKE_OFF);
                }
            }
        }
    }

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

bool AppAwakeMgr::isPowerAwake()
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
    return mIsPowerAwake;
}

bool AppAwakeMgr::isScreenAwake()
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
    return mIsScreenAwake;
}

bool AppAwakeMgr::checkIsAppCanAwake(AMPID ampid)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");

    // for (std::list<AMPID>::iterator iter=mActiveAwakeAppList.begin();iter!=mActiveAwakeAppList.end();++iter)
    // {
    //     if(ampid==*iter){
    //         logDebug(PMC,"app can awake");
    //         return true;
    //     }
    // }
    //
    // logDebug(PMC,"app can't awake");
    // return false;

    //note:there is a bug in the code ,if appList get slow,mActiveAwakeAppList will null

    return true;

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

bool AppAwakeMgr::checkIsPowerAwakeInMap(void)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");

    for (std::map<AMPID,bool>::iterator iter=mPowerAwakeAppMap.begin();iter!=mPowerAwakeAppMap.end();++iter){
        if(iter->second){
            logDebug(PMC,"power awake");
            return true;
        }
    }

    logDebug(PMC,"power not awake");
    return false;

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

bool AppAwakeMgr::checkIsScreenAwakeInMap(void)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");

    for (std::map<AMPID,bool>::iterator iter=mScreenAwakeAppMap.begin();iter!=mScreenAwakeAppMap.end();++iter){
        if(iter->second){
            logDebug(PMC,"screen awake");
            return true;
        }
    }

    logDebug(PMC,"screen not awake");
    return false;

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}

bool AppAwakeMgr::checkIsAbnormalAwakeInMap(void)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");

    for (std::map<AMPID,bool>::iterator iter=mAbnormalAwakeAppMap.begin();iter!=mAbnormalAwakeAppMap.end();++iter){
        if(iter->second){
            logDebug(PMC,"abnormal awake");
            return true;
        }
    }

    logDebug(PMC,"abnormal not awake");
    return false;

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}


void AppAwakeMgr::sendSignalToPMC(ama_PowerSignal_e powerAwakeSig)
{
    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>IN");

    auto pmcTask = make_shared<PMCTask>();
    pmcTask->SetType(HANDLE_APP_AWAKE_SIGNAL);
    pmcTask->setPowerSignal(powerAwakeSig);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);

    logVerbose(PMC,"AppAwakeMgr::",__FUNCTION__,"-->>OUT");
}
