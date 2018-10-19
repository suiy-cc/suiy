/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef POWERSDK_H_
#define POWERSDK_H_

#include <mutex>
#include <map>
#include <list>

#include "AppPower.h"
#include "ama_types.h"
#include <CommonAPI/CommonAPI.hpp>

class PowerSDK
{
public:
    static PowerSDK* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static PowerSDK instance;
        return &instance;
    }
    virtual ~PowerSDK();

    //called by powerSDK API
    void initAsync(void);
    void initSync(void);
    bool isPowerOn(uint8_t seatID);
    bool isACCOn(uint8_t seatID);
    ama_PowerState_e getPowerState(uint8_t seatID);
    void setPowerStateChangedCB(powerStateChangedCB f);
    void setChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f);
    void setChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f);
    void removePowerStateChangedCB(powerStateChangedCB f);
    void removeChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f);
    void removeChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f);
    void requestPowerAwake(int32_t ampid,bool isPowerAwake);
    void requestScreenAwake(int32_t ampid,bool isScreenAwake);
    void requestAbnormalAwake(int32_t ampid,bool isAbnormalAwake);
    void requestPowerGoodbye(void);
    void initPowerState(const uint32_t& iviPowerSta,const uint32_t& lRsePowerSta,const uint32_t& rRsePowerSta);

    //called by commonAPI
    void handlePowerStateChanged(uint8_t seatID,ama_PowerState_e powerSta);

    //old power interface
    void SetPowerStateChangeNotifyCB(PowerStateChangeNotifyCB f);
    void SetACCStateChangeNotifyCB(ACCStateChangeNotifyCB f);
    void SetPowerStateChangeExNotifyCB(PowerStateChangeExNotifyCB f);
    void SetChangeOfPowerStateCB(changeOfPowerStateCB f);

protected:
    static void allPowerAsync_cb(const CommonAPI::CallStatus& callStatus,
         const uint32_t& iviPowerSta,const uint32_t& lRsePowerSta,const uint32_t& rRsePowerSta);

private:
    PowerSDK();
    void callPowerStateChangedCB(uint8_t seatID,ama_PowerState_e powerSta);
    void callChangeOfPowerStaChangedCB(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta);

    bool mIsInited;
    uint32_t mChangeIndex;

    std::list<powerStateChangedCB> mPowerChangedCBList;
    std::list<changeOfPowerStaChangedCB> mChangeOfPowerChangedCBList;
    changeOfPowerStaChangedCBWithIndex mChangeOfPowerStaChangedCBWithIndex;
    std::map<uint8_t,ama_PowerState_e> mCurPowerMap;

    PowerStateChangeNotifyCB powerStateChangeNotify;
    ACCStateChangeNotifyCB accStateChangeNotify;
    PowerStateChangeExNotifyCB powerStateChangeExNotify;
    changeOfPowerStateCB changeOfPowerStateNotify;
};

#endif//POWERSDK_H_
