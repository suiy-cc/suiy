/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef POWERDEVICE_H_
#define POWERDEVICE_H_

#include <mutex>
#include <memory>

#include "ama_enum.h"
#include "ama_types.h"
#include "ama_audioTypes.h"
#include "IPowerState.h"
#include "USBManager.h"

#define     OPEN        (true)
#define     CLOSE       (false)
#define     ENABLE      (true)
#define     DISABLE     (false)
#define     MUTE        (true)
#define     UNMUTE      (false)

using std::shared_ptr;
using std::mutex;

class CAbnormalMgr;

class PowerDevice
{
public:
    PowerDevice(E_SEAT seatID);
    virtual ~PowerDevice();

    virtual void handlePowerSignal(ama_PowerSignal_e powerSignal)=0;
    bool isIVIDevice(void);
    // bool getIsSocVoltageNormal() const;
    // void setIsSocVoltageNormal(bool value);
    // bool getIsSocTempNormal() const;
    // void setIsSocTempNormal(bool value);
    void makeScreenShot();
protected:
    friend class IPowerState;
   // void recordAbnormalFlag(ama_PowerSignal_e powerSignal);
    virtual void recordLastNormalState(ama_PowerState_e powerStateFlag)=0;
    void changePowerStateTo(shared_ptr<IPowerState> pPowerState);
    void changeAbnormalPowerStateTo(shared_ptr<IPowerState> pPowerState);
    void changeNormalPowerStateTo(shared_ptr<IPowerState> pPowerState);
    void updateFinalPowerState(shared_ptr<IPowerState> pPowerState);
    ama_changeOfPowerSta_e getPowerChange(ama_PowerState_e oldState,ama_PowerState_e newState);
    ama_changeOfPowerSta_e getPowerChange(int powerTypeFlag,ama_PowerState_e oldState,ama_PowerState_e newState);
    int getPowerLevel(ama_PowerState_e state);
    void tellAppNewPowerState(ama_PowerState_e powerState);
    void tellChangeOfPowerStateToAC(ama_changeOfPowerSta_e changeOfPowerSta);
   // virtual void changePowerStateByRecord(void)=0;
    virtual void refreshDeviceByPowerState(ama_PowerState_e powerStateFlag)=0;
    void setLowestBrightness();
    void setNormalBrightness();

    void setLCDState(bool isOpen);
    void setTouchState(bool isEnable);
    void setAudioState(bool isEnable);
    void setTouchForbiden(bool isForbiden);
    void setMediaMute(bool isMediaMute);
    //we set allApps visible when powerState change to Normal,and user can't see apps when welcome and opening playing
    void setAllAppsVisible(bool isVisible);

    
protected:
    shared_ptr<IPowerState> mpNormalPowerState;
    shared_ptr<IPowerState> mpAbnormalPowerState;
    shared_ptr<IPowerState> mpFinalPowerState;

    shared_ptr<CAbnormalMgr> mpcabnormalmgr;
    // bool isSocVoltageNormal;
    // bool isSocTempNormal;
    // int mLastScreenTemp;
    bool mIsIVIDeviceNotRSE;
    ama_PowerState_e mLastNormalPowerState;
    E_SEAT mSeatID;
};

#endif//POWERDEVICE_H_
