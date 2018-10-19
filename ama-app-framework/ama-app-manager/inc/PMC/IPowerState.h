/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef IPOWERSTATE_H
#define IPOWERSTATE_H

#include "ama_enum.h"
#include "AMGRLogContext.h"

#include <memory>
using std::shared_ptr;

class PowerDevice;
class CAbnormalMgr;

class IPowerState
{
public:

    virtual void handlePM_AccOn_PowerOn(PowerDevice* pPowerDevice);
    virtual void handlePM_AccOn_PowerOff(PowerDevice* pPowerDevice);
    virtual void handlePM_AccOff(PowerDevice* pPowerDevice);
    virtual void handlePM_PowerOn(PowerDevice* pPowerDevice);
    virtual void handlePM_PowerOff(PowerDevice* pPowerDevice);
    virtual void handlePM_PowerOff_Awake(PowerDevice* pPowerDevice);
    virtual void handlePM_ScreenOn(PowerDevice* pPowerDevice);
    virtual void handlePM_ScreenOff(PowerDevice* pPowerDevice);
    virtual void handlePM_ScreenOff_Awake(PowerDevice* pPowerDevice);
    virtual void handlePM_Ign_Not_Come(PowerDevice* pPowerDevice);
    virtual void handlePM_Ign_Come(PowerDevice* pPowerDevice);
    virtual void handlePM_Soc_Temp_High(CAbnormalMgr* pPowerDevice);
  //  virtual void handlePM_Soc_Temp_Normal(PowerDevice* pPowerDevice);
    virtual void handlePM_Soc_Voltage_High(CAbnormalMgr* pPowerDevice);
  //  virtual void handlePM_Soc_Voltage_Normal(PowerDevice* pPowerDevice);
    virtual void handlePM_Soc_Voltage_Low(CAbnormalMgr* pPowerDevice);
 //   virtual void handleSCR_TEMP_HIGH_90(PowerDevice* pPowerDevice);
    virtual void handleSCR_TEMP_HIGH_90_95(CAbnormalMgr* pPowerDevice);
    virtual void handleSCR_TEMP_HIGH_95_100(CAbnormalMgr* pPowerDevice);
    virtual void handleSCR_TEMP_HIGH_100(CAbnormalMgr* pPowerDevice);
    virtual void handleABN_To_Normal(CAbnormalMgr* pPowerDevice);
    virtual void handleRSE_POWER_ON(PowerDevice* pPowerDevice);
    virtual void handleRSE_POWER_OFF(PowerDevice* pPowerDevice);
    virtual void handleRSE_SCREEN_ON(PowerDevice* pPowerDevice);
    virtual void handleRSE_SCREEN_OFF(PowerDevice* pPowerDevice);
    virtual void handleAPP_SCREEN_AWAKE_ON(PowerDevice* pPowerDevice);
    virtual void handleAPP_SCREEN_AWAKE_OFF(PowerDevice* pPowerDevice);
    virtual void handleAPP_POWER_AWAKE_ON(PowerDevice* pPowerDevice);
    virtual void handleAPP_POWER_AWAKE_OFF(PowerDevice* pPowerDevice);
    virtual void handleAPP_ABNORMAL_AWAKE_ON(CAbnormalMgr* pPowerDevice);
    virtual void handleAPP_ABNORMAL_AWAKE_OFF(CAbnormalMgr* pPowerDevice);
    virtual void handleAPP_SET_GOODBYE_MODE(PowerDevice* pPowerDevice);

    virtual void handleBOOT_WELCOME_PLAY_START(PowerDevice* pPowerDevice);
    virtual void handleBOOT_WELCOME_PLAY_OVER(PowerDevice* pPowerDevice);
    virtual void handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice);

    virtual ama_PowerState_e GetPowerStateFlag(){
        return E_PSTA_DEF_MAX;
    }
    virtual std::string GetPowerStateFlagStr()
    {
        switch(GetPowerStateFlag())
        {
            case E_PS_POWER_INIT: return "E_PS_POWER_INIT";
            case E_PS_WELCOME_MODE: return "E_PS_WELCOME_MODE";
            case E_PS_OPENING_MODE: return "E_PS_OPENING_MODE";
            case E_PS_ACCOFF:return "E_PS_ACCOFF";
            case E_PS_GOODBYE_MODE:return "E_PS_GOODBYE_MODE";
            case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:return "E_PS_ACCON_ABNORMAL_SOCVOLTAGE";
            case E_PS_ACCON_ABNORMAL_SOCTEMP:return "E_PS_ACCON_ABNORMAL_SOCTEMP";
            case E_PS_ACCON_ABNORMAL_AWAKE:return "E_PS_ACCON_ABNORMAL_AWAKE";
            case E_PS_ACCON_NORMAL_POWERON_SCRON:return "E_PS_ACCON_NORMAL_POWERON_SCRON";
            case E_PS_ACCON_NORMAL_POWERON_SCROFF:return "E_PS_ACCON_NORMAL_POWERON_SCROFF";
            case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:return "E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE";
            case E_PS_ACCON_NORMAL_POWEROFF:return "E_PS_ACCON_NORMAL_POWEROFF";
            case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:return "E_PS_ACCON_NORMAL_POWEROFF_AWAKE";
            case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:return "E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH";
            case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:return "E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER";
            default: return "ERROR POWER STATE";
        }
    }
protected:
    void changeStateTo(PowerDevice* pPowerDevice,shared_ptr<IPowerState> pPowerState);
    void changeAbnormalStateTo(CAbnormalMgr* pPowerDevice,shared_ptr<IPowerState> pPowerState);
    void changeNormalStateTo(PowerDevice* pPowerDevice,shared_ptr<IPowerState> pPowerState);
    void changeStateByDeviceRecord(PowerDevice* pPowerDevice);

};
    

class IAccOnState : public IPowerState
{
public:
    void handlePM_AccOff(PowerDevice* pPowerDevice)override final;
    void handlePM_Ign_Not_Come(PowerDevice* pPowerDevice)override final;
    void handlePM_Ign_Come(PowerDevice* pPowerDevice)override final;

protected:
    
};


class IAccOn_NormalState : public IAccOnState
{
public:
    // void handlePM_Soc_Temp_High(PowerDevice* pPowerDevice)override final;
    // void handlePM_Soc_Voltage_High(PowerDevice* pPowerDevice)override final;
    // void handlePM_Soc_Voltage_Low(PowerDevice* pPowerDevice)override final;
    // void handleSCR_TEMP_HIGH_95_100(PowerDevice* pPowerDevice)override final;
    // void handleSCR_TEMP_HIGH_100(PowerDevice* pPowerDevice)override final;
};

class IAccOn_AbormalState : public IPowerState
{
public:
    ama_PowerState_e GetPowerStateFlag();
    virtual void handlePM_Soc_Temp_High(CAbnormalMgr* pPowerDevice) override final;
    virtual void handlePM_Soc_Voltage_High(CAbnormalMgr* pPowerDevice) override final;
    virtual void handlePM_Soc_Voltage_Low(CAbnormalMgr* pPowerDevice) override final;
    virtual void handleSCR_TEMP_HIGH_95_100(CAbnormalMgr* pPowerDevice) override final;
    virtual void handleSCR_TEMP_HIGH_100(CAbnormalMgr* pPowerDevice) override final;
    virtual void handleAPP_ABNORMAL_AWAKE_ON(CAbnormalMgr* pPowerDevice) override final;    

protected:
    

};

class IAccOn_Normal_PowerOnState : public IAccOnState
{
public:
    void handlePM_PowerOff(PowerDevice* pPowerDevice)override final;
    void handleRSE_POWER_OFF(PowerDevice* pPowerDevice)override final;
    void handleAPP_SET_GOODBYE_MODE(PowerDevice* pPowerDevice)override final;



};

class IAccOn_Normal_PowerOffState : public IAccOnState
{
public:
    void handlePM_PowerOn(PowerDevice* pPowerDevice)override final;
    void handleRSE_POWER_ON(PowerDevice* pPowerDevice)override final;
};


#endif // IPOWERSTATE_H
