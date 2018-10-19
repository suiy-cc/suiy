/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "PowerSDK.h"
#include "log.h"

#include <v0_1/org/neusoft/AppMgrProxy.hpp>

extern std::shared_ptr<v0_1::org::neusoft::AppMgrProxy<>> appManager;
LOG_DECLARE_CONTEXT(SDK_PWR);

PowerSDK::PowerSDK()
:mIsInited(false)
,powerStateChangeNotify(nullptr)
,accStateChangeNotify(nullptr)
,powerStateChangeExNotify(nullptr)
,changeOfPowerStateNotify(nullptr)
,mChangeOfPowerStaChangedCBWithIndex(nullptr)
,mChangeIndex(0)
{
    RegisterContext(SDK_PWR, "SPWR");
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    mCurPowerMap[E_SEAT_IVI]=E_PS_POWER_INIT;
    mCurPowerMap[E_SEAT_RSE1]=E_PS_POWER_INIT;
    mCurPowerMap[E_SEAT_RSE2]=E_PS_POWER_INIT;

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

PowerSDK::~PowerSDK()
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::initAsync(void)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(appManager){
        appManager->GetAllPowerStateWhenSDKInitReqAsync(allPowerAsync_cb);
    }else{
        logError(SDK_PWR, "invalid app-manager pointer!");
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::initSync(void)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);

    uint32_t iviPowerSta;
    uint32_t lRsePowerSta;
    uint32_t rRsePowerSta;

    if(appManager){
        appManager->GetAllPowerStateWhenSDKInitReq(callStatus,iviPowerSta,lRsePowerSta,rRsePowerSta,&callInfo);
        if(callStatus != CommonAPI::CallStatus::SUCCESS){
            logError(SDK_PWR, "failed calling AppManager::GetAllPowerStateWhenSDKInitReq()!");
            return;
        }else{
            logDebugF(SDK_PWR,"iviPowerSta:[%x];lRsePowerSta:[%x];rRsePowerSta:[%x] \n",iviPowerSta,lRsePowerSta,rRsePowerSta);
            mCurPowerMap[E_SEAT_IVI] = static_cast<ama_PowerState_e>(iviPowerSta);
            mCurPowerMap[E_SEAT_RSE1] = static_cast<ama_PowerState_e>(lRsePowerSta);
            mCurPowerMap[E_SEAT_RSE2] = static_cast<ama_PowerState_e>(rRsePowerSta);
            mIsInited = true;
        }
    }else{
        logError(SDK_PWR, "invalid app-manager pointer!");
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::allPowerAsync_cb(const CommonAPI::CallStatus& callStatus,
     const uint32_t& iviPowerSta,const uint32_t& lRsePowerSta,const uint32_t& rRsePowerSta)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(callStatus != CommonAPI::CallStatus::SUCCESS){
        logError(SDK_PWR, "failed calling AppManager::allPowerAsync_cb()!");
        return;
    }else{
        logDebugF(SDK_PWR,"iviPowerSta:[%x];lRsePowerSta:[%x];rRsePowerSta:[%x] \n",iviPowerSta,lRsePowerSta,rRsePowerSta);
        PowerSDK::GetInstance()->initPowerState(iviPowerSta,lRsePowerSta,rRsePowerSta);
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::initPowerState(const uint32_t& iviPowerSta,const uint32_t& lRsePowerSta,const uint32_t& rRsePowerSta)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    logDebugF(SDK_PWR,"iviPowerSta:[%x];lRsePowerSta:[%x];rRsePowerSta:[%x] \n",iviPowerSta,lRsePowerSta,rRsePowerSta);
    mCurPowerMap[E_SEAT_IVI] = static_cast<ama_PowerState_e>(iviPowerSta);
    mCurPowerMap[E_SEAT_RSE1] = static_cast<ama_PowerState_e>(lRsePowerSta);
    mCurPowerMap[E_SEAT_RSE2] = static_cast<ama_PowerState_e>(rRsePowerSta);
    //return init power state for callback
    callPowerStateChangedCB(E_SEAT_IVI,mCurPowerMap[E_SEAT_IVI]);
    callPowerStateChangedCB(E_SEAT_RSE1,mCurPowerMap[E_SEAT_RSE1]);
    callPowerStateChangedCB(E_SEAT_RSE2,mCurPowerMap[E_SEAT_RSE2]);
    //code for old power interface
    if(powerStateChangeExNotify){
        powerStateChangeExNotify(mCurPowerMap[E_SEAT_IVI], E_SEAT_IVI);
        powerStateChangeExNotify(mCurPowerMap[E_SEAT_RSE1], E_SEAT_RSE1);
        powerStateChangeExNotify(mCurPowerMap[E_SEAT_RSE2], E_SEAT_RSE2);
    }
    mIsInited = true;
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

bool PowerSDK::isPowerOn(uint8_t seatID)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    if(!IS_SEATID_VALID(seatID)){
        logError(SDK_PWR,"seatID is invalid!!");
        return false;
    }

    if(!mIsInited){
        initSync();
    }

    return (mCurPowerMap[seatID]&POWER_POWER_FLAG);

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

bool PowerSDK::isACCOn(uint8_t seatID)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    if(!IS_SEATID_VALID(seatID)){
        logError(SDK_PWR,"seatID is invalid!!");
        return false;
    }

    if(!mIsInited){
        initSync();
    }

    return (mCurPowerMap[seatID]&POWER_ACC_FLAG);
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

ama_PowerState_e PowerSDK::getPowerState(uint8_t seatID)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    if(!IS_SEATID_VALID(seatID)){
        logError(SDK_PWR,"seatID is invalid!!");
        return E_PS_POWER_INIT;
    }

    if(!mIsInited){
        initSync();
    }

    return mCurPowerMap[seatID];
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::setPowerStateChangedCB(powerStateChangedCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(!mIsInited){
        initAsync();
    }

    if(f){
        for(std::list<powerStateChangedCB>::iterator it=mPowerChangedCBList.begin();it!=mPowerChangedCBList.end();++it){
            if(*it == f){
                logWarn(SDK_PWR,"callback function  already in mPowerChangedCBList");
                return;
            }
        }

        mPowerChangedCBList.push_back(f);
        logInfoF(SDK_PWR,"setPowerStateChangedCB:%p",f);

        //return current power state when registerCallback
        f(E_SEAT_IVI,mCurPowerMap[E_SEAT_IVI]);
        f(E_SEAT_RSE1,mCurPowerMap[E_SEAT_RSE1]);
        f(E_SEAT_RSE2,mCurPowerMap[E_SEAT_RSE2]);
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::setChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(!mIsInited){
        initAsync();
    }

    if(f){
        for(std::list<changeOfPowerStaChangedCB> ::iterator it = mChangeOfPowerChangedCBList.begin(); it != mChangeOfPowerChangedCBList.end(); ++it){
            if(*it == f){
                logWarn(SDK_PWR,"callback function  already in mPowerChangedCBList");
                return;
            }
        }

        mChangeOfPowerChangedCBList.push_back(f);
        logInfoF(SDK_PWR,"setChangeOfPowerStaChangedCB:%p",f);
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::setChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(!mIsInited){
        initAsync();
    }

    if(f){
        mChangeOfPowerStaChangedCBWithIndex = f;
        logInfoF(SDK_PWR,"setChangeOfPowerStaChangedCBWithIndex:%p",f);
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::removePowerStateChangedCB(powerStateChangedCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(f){
        for(std::list<powerStateChangedCB>::iterator it=mPowerChangedCBList.begin();it!=mPowerChangedCBList.end();++it){
            if(*it == f){
                mPowerChangedCBList.erase(it);
                return;
            }
        }

        logError(SDK_PWR,"callback function is not in list!!");
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::removeChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(f){
        for(std::list<changeOfPowerStaChangedCB>::iterator it=mChangeOfPowerChangedCBList.begin();it!=mChangeOfPowerChangedCBList.end();++it){
            if(*it == f){
                mChangeOfPowerChangedCBList.erase(it);
                return;
            }
        }

        logError(SDK_PWR,"callback function is not in list!!");
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::removeChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(f){
        if(f==mChangeOfPowerStaChangedCBWithIndex){
            mChangeOfPowerStaChangedCBWithIndex = nullptr;
        }else{
            logError(SDK_PWR,"f != mChangeOfPowerStaChangedCBWithIndex!!");
        }
    }else{
        logError(SDK_PWR,"f == nullptr!!");
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::requestPowerAwake(int32_t ampid,bool isPowerAwake)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    CommonAPI::CallStatus callStatus;
    if(appManager){
        appManager->PowerAwakeReq(ampid,isPowerAwake,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        {
            logError(SDK_PWR, "failed calling AppManager::PowerAwakeReq()!");
        }
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::requestScreenAwake(int32_t ampid,bool isScreenAwake)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    CommonAPI::CallStatus callStatus;
    if(appManager){
        appManager->ScreenAwakeReq(ampid,isScreenAwake,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        {
            logError(SDK_PWR, "failed calling AppManager::ScreenAwakeReq()!");
        }
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::requestAbnormalAwake(int32_t ampid,bool isAbnormalAwake)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    CommonAPI::CallStatus callStatus;
    if(appManager){
        appManager->AbnormalAwakeReq(ampid,isAbnormalAwake,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        {
            logError(SDK_PWR, "failed calling AppManager::ScreenAwakeReq()!");
        }
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::requestPowerGoodbye(void)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    CommonAPI::CallStatus callStatus;
    if(appManager){
        appManager->SendPowerSignalReq(E_APP_SET_GOODBYE,callStatus);
        if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        {
            logError(SDK_PWR, "failed calling AppManager::PowerAwakeReq()!");
        }
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::handlePowerStateChanged(uint8_t seatID,ama_PowerState_e powerSta)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");

    if(IS_POWER_EVENT(powerSta)){
        logInfo(SDK_PWR,"is a power event,just callPowerStateChangedCB ");
        callPowerStateChangedCB(seatID,powerSta);
        return;
    }

    ama_PowerState_e oldState = mCurPowerMap[seatID];
    ama_PowerState_e newState = powerSta;

    logVerbose(SDK_PWR,"oldState:",ToHEX(oldState), "newState:",ToHEX(newState));

    mChangeIndex++;
    if(mChangeIndex == 0xffffffff){
        mChangeIndex = 0;
    }

    if(newState!=oldState){
        callPowerStateChangedCB(seatID,newState);
    }

    if((oldState&POWER_ACC_FLAG)^(newState&POWER_ACC_FLAG)){
        if(newState&POWER_ACC_FLAG){
            callChangeOfPowerStaChangedCB(seatID,E_ACC_CHANGE_TO_ON);
        }else{
            callChangeOfPowerStaChangedCB(seatID,E_ACC_CHANGE_TO_OFF);
        }
    }else{
        logDebug(SDK_PWR,"acc state not changed");
    }

    if((oldState&POWER_NORMAL_FLAG)^(newState&POWER_NORMAL_FLAG)){
        if(newState&POWER_NORMAL_FLAG){
            callChangeOfPowerStaChangedCB(seatID,E_STATE_CHANGE_TO_NORMAL);
        }else{
            callChangeOfPowerStaChangedCB(seatID,E_STATE_CHANGE_TO_ABNORMAL);
        }
    }else{
        logDebug(SDK_PWR,"normal state not changed");
    }

    if((oldState&POWER_POWER_FLAG)^(newState&POWER_POWER_FLAG)){
        if(newState&POWER_POWER_FLAG){
            callChangeOfPowerStaChangedCB(seatID,E_POWER_CHANGE_TO_ON);
        }else{
            callChangeOfPowerStaChangedCB(seatID,E_POWER_CHANGE_TO_OFF);
        }
    }else{
        logDebug(SDK_PWR,"power state not changed");
    }

    if((oldState&POWER_SCR_FLAG)^(newState&POWER_SCR_FLAG)){
        if(newState&POWER_SCR_FLAG){
            callChangeOfPowerStaChangedCB(seatID,E_SCR_CHANGE_TO_ON);
        }else{
            callChangeOfPowerStaChangedCB(seatID,E_SCR_CHANGE_TO_OFF);
        }
    }else{
        logDebug(SDK_PWR,"screen state not changed");
    }

    //update Power last
    mCurPowerMap[seatID] = powerSta;

    //code for old power interface
    if(E_SEAT_IVI==seatID){
        if(accStateChangeNotify){
            accStateChangeNotify((powerSta&POWER_ACC_FLAG));
        }

        if(powerStateChangeNotify){
            powerStateChangeNotify((powerSta&POWER_ACC_FLAG));
        }
    }
    if(powerStateChangeExNotify){
        powerStateChangeExNotify(powerSta, seatID);
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::callPowerStateChangedCB(uint8_t seatID,ama_PowerState_e powerSta)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    if(!mPowerChangedCBList.empty()){
        logDebug(SDK_PWR,"mChangeOfPowerChangedCBList is not empty");
        for(std::list<powerStateChangedCB>::iterator it=mPowerChangedCBList.begin();it!=mPowerChangedCBList.end();++it){
            powerStateChangedCB powerStateChangedFun = *it;
            logInfoF(SDK_PWR,"call powerStateChangedCB:%p",powerStateChangedFun);
            powerStateChangedFun(seatID,powerSta);
        }
    }else{
        logInfo(SDK_PWR,"mChangeOfPowerChangedCBList is empty");
    }
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::callChangeOfPowerStaChangedCB(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    if(!mChangeOfPowerChangedCBList.empty()){
        logDebug(SDK_PWR,"mChangeOfPowerChangedCBList is not empty");
        for(std::list<changeOfPowerStaChangedCB>::iterator it=mChangeOfPowerChangedCBList.begin();it!=mChangeOfPowerChangedCBList.end();++it){
            changeOfPowerStaChangedCB changeOfPowerStaChangedFun = *it;
            logInfoF(SDK_PWR,"call changeOfPowerStaChangedCB:%p",changeOfPowerStaChangedFun);
            changeOfPowerStaChangedFun(seatID,changeOfPowerSta);
        }
    }else{
        logInfo(SDK_PWR,"mChangeOfPowerChangedCBList is empty");
    }

    if(mChangeOfPowerStaChangedCBWithIndex){
        mChangeOfPowerStaChangedCBWithIndex(seatID,changeOfPowerSta,mChangeIndex);
    }

    //code for old power interface
    if(changeOfPowerStateNotify){
        logInfo(SDK_PWR,"changeOfPowerStateNotify is not empty");
        changeOfPowerStateNotify(changeOfPowerSta, seatID);
    }

    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

//old power interface code
void PowerSDK::SetPowerStateChangeNotifyCB(PowerStateChangeNotifyCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    powerStateChangeNotify = f;
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::SetACCStateChangeNotifyCB(ACCStateChangeNotifyCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    accStateChangeNotify = f;
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::SetPowerStateChangeExNotifyCB(PowerStateChangeExNotifyCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    powerStateChangeExNotify = f;
    if(powerStateChangeExNotify){
        powerStateChangeExNotify(mCurPowerMap[E_SEAT_IVI], E_SEAT_IVI);
        powerStateChangeExNotify(mCurPowerMap[E_SEAT_RSE1], E_SEAT_RSE1);
        powerStateChangeExNotify(mCurPowerMap[E_SEAT_RSE2], E_SEAT_RSE2);
    }
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}

void PowerSDK::SetChangeOfPowerStateCB(changeOfPowerStateCB f)
{
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>IN");
    changeOfPowerStateNotify = f;
    logVerbose(SDK_PWR,"PowerSDK::",__FUNCTION__,"-->>OUT");
}
