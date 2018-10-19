/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef POWERSTATE_H
#define POWERSTATE_H

#include "ama_enum.h"
#include "AMGRLogContext.h"
#include "IPowerState.h"
/*************************************PowerInitState*****************************************/
class PowerInitState : public IPowerState
{
public:
    PowerInitState();
    ~PowerInitState();
    ama_PowerState_e GetPowerStateFlag();
};
/*************************************PowerInitState*****************************************/

/***********************************WelcomeState***********************************/
class WelcomeState : public IPowerState
{
public:
    WelcomeState();
    ~WelcomeState();
    ama_PowerState_e GetPowerStateFlag();
    void handlePM_AccOn_PowerOn(PowerDevice* pPowerDevice);
    void handlePM_AccOn_PowerOff(PowerDevice* pPowerDevice);
    void handleBOOT_WELCOME_PLAY_OVER(PowerDevice* pPowerDevice);
    void handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice);
};
/***********************************WelcomeState***********************************/

/***********************************OpeningState***********************************/
class OpeningState : public IPowerState
{
public:
    OpeningState();
    ~OpeningState();
    ama_PowerState_e GetPowerStateFlag();
    void handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice);
    void handlePM_AccOff(PowerDevice* pPowerDevice);
};
/***********************************WelcomeState***********************************/

/*************************************AccOffState*****************************************/
class AccOffState : public IPowerState
{
public:
    AccOffState();
    ~AccOffState();
    ama_PowerState_e GetPowerStateFlag();
    void handlePM_AccOn_PowerOn(PowerDevice* pPowerDevice);
    void handlePM_AccOn_PowerOff(PowerDevice* pPowerDevice);
    void handleBOOT_WELCOME_PLAY_START(PowerDevice* pPowerDevice);
    void handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice);
};
/*************************************AccOffState*****************************************/

/*************************************AccOn_Goodbye_State*****************************************/
class AccOn_Goodbye_State : public IPowerState
{
public:
    AccOn_Goodbye_State();
    ~AccOn_Goodbye_State();
    ama_PowerState_e GetPowerStateFlag();
};
/*************************************AccOn_Goodbye_State*****************************************/


/*************************************AccOn_Abnormal_SocVoltageState*****************************************/
class AccOn_Abnormal_SocVoltageState : public IAccOn_AbormalState
{
public:
    AccOn_Abnormal_SocVoltageState();
    ~AccOn_Abnormal_SocVoltageState();
    ama_PowerState_e GetPowerStateFlag();
   // void handlePM_Soc_Voltage_Normal(PowerDevice* pPowerDevice);
    void handleABN_To_Normal(CAbnormalMgr* pPowerDevice);
    
};
/*************************************AccOn_Abnormal_SocVoltageState*****************************************/

/*************************************AccOn_Abnormal_SocTempState*****************************************/
class AccOn_Abnormal_SocTempState : public IAccOn_AbormalState
{
public:
    AccOn_Abnormal_SocTempState();
    ~AccOn_Abnormal_SocTempState();
    ama_PowerState_e GetPowerStateFlag();
  //  void handlePM_Soc_Temp_Normal(PowerDevice* pPowerDevice);
    void handleABN_To_Normal(CAbnormalMgr* pPowerDevice);
};
/*************************************AccOn_Abnormal_SocTempState*****************************************/

/*************************************AccOn_Abnormal_SocTempState*****************************************/
class AccOn_Abnormal_ScrTempHighState : public IAccOn_AbormalState
{
public:
    AccOn_Abnormal_ScrTempHighState();
    ~AccOn_Abnormal_ScrTempHighState();
    ama_PowerState_e GetPowerStateFlag();
   // void handleSCR_TEMP_HIGH_90(PowerDevice* pPowerDevice);
     void handleABN_To_Normal(CAbnormalMgr* pPowerDevice);
    // void handleSCR_TEMP_HIGH_100(PowerDevice* pPowerDevice);
    // void handlePM_Soc_Voltage_High(PowerDevice* pPowerDevice);
    // void handlePM_Soc_Voltage_Low(PowerDevice* pPowerDevice);
    // void handlePM_Soc_Temp_High(PowerDevice* pPowerDevice);
};
/*************************************AccOn_Abnormal_SocTempState*****************************************/

/*************************************AccOn_Abnormal_SocTempState*****************************************/
class AccOn_Abnormal_ScrTempHigherState : public IAccOn_AbormalState
{
public:
    AccOn_Abnormal_ScrTempHigherState();
    ~AccOn_Abnormal_ScrTempHigherState();
    ama_PowerState_e GetPowerStateFlag();
 //   void handleSCR_TEMP_HIGH_90(PowerDevice* pPowerDevice);
    void handleABN_To_Normal(CAbnormalMgr* pPowerDevice);
   void handleSCR_TEMP_HIGH_90_95(CAbnormalMgr* pPowerDevice);
};
/*************************************AccOn_Abnormal_SocTempState*****************************************/

/*************************************AccOn_Abnormal_SocTempState*****************************************/
class AccOn_Abnormal_AwakeState : public IAccOn_AbormalState
{
public:
    AccOn_Abnormal_AwakeState();
    ~AccOn_Abnormal_AwakeState();
    ama_PowerState_e GetPowerStateFlag();
    virtual void handleABN_To_Normal(CAbnormalMgr* pPowerDevice);
    virtual void handleAPP_ABNORMAL_AWAKE_OFF(CAbnormalMgr* pPowerDevice) override final;
    
};
/*************************************AccOn_Abnormal_SocTempState*****************************************/



/*************************************AccOn_Normal_PowerOn_ScrOnState*****************************************/
class AccOn_Normal_PowerOn_ScrOnState : public IAccOn_Normal_PowerOnState
{
public:
    AccOn_Normal_PowerOn_ScrOnState();
    ~AccOn_Normal_PowerOn_ScrOnState();
    ama_PowerState_e GetPowerStateFlag();
    void handlePM_ScreenOff(PowerDevice* pPowerDevice);
    void handleRSE_SCREEN_OFF(PowerDevice* pPowerDevice);
};
/*************************************AccOn_Normal_PowerOn_ScrOnState*****************************************/

/*************************************AccOn_Normal_PowerOn_ScrOffState*****************************************/
class AccOn_Normal_PowerOn_ScrOffState : public IAccOn_Normal_PowerOnState
{
public:
    AccOn_Normal_PowerOn_ScrOffState();
    ~AccOn_Normal_PowerOn_ScrOffState();
    ama_PowerState_e GetPowerStateFlag();
    void handlePM_ScreenOn(PowerDevice* pPowerDevice);
    void handleRSE_SCREEN_ON(PowerDevice* pPowerDevice);
    void handleAPP_SCREEN_AWAKE_ON(PowerDevice* pPowerDevice);
};
/*************************************AccOn_Normal_PowerOn_ScrOffState*****************************************/

/*************************************AccOn_Normal_PowerOn_ScrOff_AwakeState*****************************************/
class AccOn_Normal_PowerOn_ScrOff_AwakeState : public IAccOn_Normal_PowerOnState
{
public:
    AccOn_Normal_PowerOn_ScrOff_AwakeState();
    ~AccOn_Normal_PowerOn_ScrOff_AwakeState();
    ama_PowerState_e GetPowerStateFlag();
    void handlePM_ScreenOn(PowerDevice* pPowerDevice);
    void handleAPP_SCREEN_AWAKE_OFF(PowerDevice* pPowerDevice);
};
/*************************************AccOn_Normal_PowerOn_ScrOff_AwakeState*****************************************/

/*************************************AccOn_Normal_PowerOffState*****************************************/
class AccOn_Normal_PowerOffState : public IAccOn_Normal_PowerOffState
{
public:
    AccOn_Normal_PowerOffState();
    ~AccOn_Normal_PowerOffState();
    ama_PowerState_e GetPowerStateFlag();
    void handleAPP_POWER_AWAKE_ON(PowerDevice* pPowerDevice);
};
/*************************************AccOn_Normal_PowerOffState*****************************************/

/*************************************AccOn_Normal_PowerOff_AwakeState*****************************************/
class AccOn_Normal_PowerOff_AwakeState : public IAccOn_Normal_PowerOffState
{
public:
    AccOn_Normal_PowerOff_AwakeState();
    ~AccOn_Normal_PowerOff_AwakeState();
    ama_PowerState_e GetPowerStateFlag();
    void handleAPP_POWER_AWAKE_OFF(PowerDevice* pPowerDevice);
};
/*************************************AccOn_Normal_PowerOff_AwakeState*****************************************/


#endif//POWERSTATE_H
