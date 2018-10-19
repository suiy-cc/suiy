/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "IPowerState.h"
#include "PowerState.h"
#include "PowerDevice.h"
#include "PMCtrl.h"
#include "AppAwakeMgr.h"
#include "Capi.h"
#include "CAbnormalMgr.h"



void IPowerState::handlePM_AccOn_PowerOn(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_AccOn_PowerOn signal");
}

void IPowerState::handlePM_AccOn_PowerOff(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_AccOn_PowerOff signal");
}

void IPowerState::handlePM_AccOff(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_AccOff signal");
}

void IPowerState::handlePM_PowerOn(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_PowerOn signal");
}

void IPowerState::handlePM_PowerOff(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_PowerOff signal");
}

void IPowerState::handlePM_PowerOff_Awake(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_PowerOn_Awake signal");
}

void IPowerState::handlePM_ScreenOn(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_ScreenOn signal");
}

void IPowerState::handlePM_ScreenOff(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_ScreenOff signal");
}

void IPowerState::handlePM_ScreenOff_Awake(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_AccOn_PowerOn signal");
}

void IPowerState::handlePM_Ign_Not_Come(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_Ign_Not_Come signal");
}

void IPowerState::handlePM_Ign_Come(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_Ign_Come signal");
}

void IPowerState::handlePM_Soc_Temp_High(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_Soc_Temp_High signal");
}

// void IPowerState::handlePM_Soc_Temp_Normal(PowerDevice* pPowerDevice)
// {
//     logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
//     logDebug(PMC,"do nothing for this handlePM_Soc_Temp_Normal signal");
// }

void IPowerState::handlePM_Soc_Voltage_High(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_Soc_Voltage_High signal");
}

// void IPowerState::handlePM_Soc_Voltage_Normal(PowerDevice* pPowerDevice)
// {
//     logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
//     logDebug(PMC,"do nothing for this handlePM_AccOn_PowerOn signal");
// }

void IPowerState::handlePM_Soc_Voltage_Low(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handlePM_Soc_Voltage_Low signal");
}

// void IPowerState::handleSCR_TEMP_HIGH_90(PowerDevice* pPowerDevice)
// {
//     logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
//     logDebug(PMC,"do nothing for this handleSCR_TEMP_HIGH_90 signal");
// }

void IPowerState::handleSCR_TEMP_HIGH_90_95(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleSCR_TEMP_HIGH_90_95 signal");
}

void IPowerState::handleSCR_TEMP_HIGH_95_100(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleSCR_TEMP_HIGH_95_100 signal");
}

void IPowerState::handleSCR_TEMP_HIGH_100(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleSCR_TEMP_HIGH_100 signal");
}

void IPowerState::handleABN_To_Normal(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleABN_To_Normal signal");
}

void IPowerState::handleAPP_SCREEN_AWAKE_ON(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_SCREEN_AWAKE_ON signal");
}

void IPowerState::handleAPP_SCREEN_AWAKE_OFF(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_SCREEN_AWAKE_OFF signal");
}

void IPowerState::handleAPP_POWER_AWAKE_ON(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_POWER_AWAKE_ON signal");
}

void IPowerState::handleAPP_POWER_AWAKE_OFF(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_POWER_AWAKE_OFF signal");
}

void IPowerState::handleAPP_ABNORMAL_AWAKE_ON(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_ABNORMAL_AWAKE_ON signal");
}

void IPowerState::handleAPP_ABNORMAL_AWAKE_OFF(CAbnormalMgr* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_ABNORMAL_AWAKE_OFF signal");
}

void IPowerState::handleAPP_SET_GOODBYE_MODE(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleAPP_SET_GOODBYE_MODE signal");
}

void IPowerState::handleRSE_POWER_ON(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleRSE_POWER_ON signal");
}
void IPowerState::handleRSE_POWER_OFF(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleRSE_POWER_OFF signal");
}
void IPowerState::handleRSE_SCREEN_ON(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleRSE_SCREEN_ON signal");
}
void IPowerState::handleRSE_SCREEN_OFF(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleRSE_SCREEN_OFF signal");
}

void IPowerState::handleBOOT_WELCOME_PLAY_START(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleBOOT_WELCOME_PLAY_START signal");
}

void IPowerState::handleBOOT_WELCOME_PLAY_OVER(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleBOOT_WELCOME_PLAY_OVER signal");
}

void IPowerState::handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice)
{
    logDebug(PMC,"GetPowerStateFlagStr:",GetPowerStateFlagStr());
    logDebug(PMC,"do nothing for this handleBOOT_FIRST_APP_READY signal");
}

void IPowerState::changeStateTo(PowerDevice* pPowerDevice,shared_ptr<IPowerState> pPowerState)
{
    logVerbose(PMC,"IPowerState::",__FUNCTION__,"-->>IN");
    if(pPowerDevice->isIVIDevice()){
        ama_PowerState_e powerStateFlag = pPowerState->GetPowerStateFlag();
        //we shoud check is awake when we change to E_PS_ACCON_NORMAL_POWERON_SCROFF
        if(E_PS_ACCON_NORMAL_POWERON_SCROFF==powerStateFlag){
            if(AppAwakeMgr::GetInstance()->isScreenAwake()){
                logDebug(PMC,"we should change to AccOn_Normal_PowerOff_AwakeState by AppAwakeMgr");
                auto pScreenAwake = std::make_shared<AccOn_Normal_PowerOn_ScrOff_AwakeState>();
                pPowerDevice->changePowerStateTo(pScreenAwake);
            }else{
                pPowerDevice->changePowerStateTo(pPowerState);
            }
        }else if(E_PS_ACCON_NORMAL_POWEROFF==powerStateFlag){
            if(AppAwakeMgr::GetInstance()->isPowerAwake()){
                logDebug(PMC,"we should change to AccOn_Normal_PowerOff_AwakeState by AppAwakeMgr");
                auto pPowerAwake = std::make_shared<AccOn_Normal_PowerOff_AwakeState>();
                pPowerDevice->changePowerStateTo(pPowerAwake);
            }else{
                pPowerDevice->changePowerStateTo(pPowerState);
            }
        }else{
            pPowerDevice->changePowerStateTo(pPowerState);
        }
    }else{//RSE device
        logDebug(PMC,"RSEPowerDevice don't have awake state");
        pPowerDevice->changePowerStateTo(pPowerState);
    }

    logVerbose(PMC,"IPowerState::",__FUNCTION__,"-->>OUT");
}

void IPowerState::changeAbnormalStateTo(CAbnormalMgr* pPowerDevice,shared_ptr<IPowerState> pPowerState)
{
    logVerbose(PMC,"IPowerState::",__FUNCTION__,"-->>IN");

    pPowerDevice->changeAbnormalPowerStateTo(pPowerState);

    logVerbose(PMC,"IPowerState::",__FUNCTION__,"-->>OUT");
}

void IPowerState::changeNormalStateTo(PowerDevice* pPowerDevice,shared_ptr<IPowerState> pPowerState)
{
    logVerbose(PMC,"IPowerState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        ama_PowerState_e powerStateFlag = pPowerState->GetPowerStateFlag();
        //we shoud check is awake when we change to E_PS_ACCON_NORMAL_POWERON_SCROFF
        if(E_PS_ACCON_NORMAL_POWERON_SCROFF==powerStateFlag){
            if(AppAwakeMgr::GetInstance()->isScreenAwake()){
                logDebug(PMC,"we should change to AccOn_Normal_PowerOff_AwakeState by AppAwakeMgr");
                auto pScreenAwake = std::make_shared<AccOn_Normal_PowerOn_ScrOff_AwakeState>();
                pPowerDevice->changeNormalPowerStateTo(pScreenAwake);
            }else{
                pPowerDevice->changeNormalPowerStateTo(pPowerState);
            }
        }else if(E_PS_ACCON_NORMAL_POWEROFF==powerStateFlag){
            if(AppAwakeMgr::GetInstance()->isPowerAwake()){
                logDebug(PMC,"we should change to AccOn_Normal_PowerOff_AwakeState by AppAwakeMgr");
                auto pPowerAwake = std::make_shared<AccOn_Normal_PowerOff_AwakeState>();
                pPowerDevice->changeNormalPowerStateTo(pPowerAwake);
            }else{
                pPowerDevice->changeNormalPowerStateTo(pPowerState);
            }
        }else{
            pPowerDevice->changeNormalPowerStateTo(pPowerState);
        }
    }else{//RSE device
        logDebug(PMC,"RSEPowerDevice don't have awake state");
        pPowerDevice->changeNormalPowerStateTo(pPowerState);
    }

    logVerbose(PMC,"IPowerState::",__FUNCTION__,"-->>OUT");
}


void IPowerState::changeStateByDeviceRecord(PowerDevice* pPowerDevice)
{
  //  pPowerDevice->changePowerStateByRecord();
}

/*************************************IAccOnState*****************************************/
void IAccOnState::handlePM_AccOff(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOnState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOffState>();
    changeNormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOnState::",__FUNCTION__,"-->>OUT");
}

void IAccOnState::handlePM_Ign_Not_Come(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOnState::",__FUNCTION__,"-->>IN");

    //send a notify to comservice
    //comservice will call a popup to User
    if(pPowerDevice->isIVIDevice()){
        PowerStateChangeNotifyEx(E_SEAT_IVI,E_PS_IG_NOT_COME_NOTIFY);
    }else{
        //do nothing In RSE device
    }

    logVerbose(PMC,"IAccOnState::",__FUNCTION__,"-->>OUT");
}

void IAccOnState::handlePM_Ign_Come(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOnState::",__FUNCTION__,"-->>IN");

    //send a notify to comservice
    //comservice will call a popup to User
    if(pPowerDevice->isIVIDevice()){
        PowerStateChangeNotifyEx(E_SEAT_IVI,E_PS_IG_COME_NOTIFY);
    }else{
        //do nothing In RSE device
    }

    logVerbose(PMC,"IAccOnState::",__FUNCTION__,"-->>OUT");
}

/*************************************IAccOnState*****************************************/

/**********************************IAccOn_AbormalState**************************************/

ama_PowerState_e IAccOn_AbormalState::GetPowerStateFlag()
{
    return E_PS_POWER_INIT;
}

void IAccOn_AbormalState::handlePM_Soc_Temp_High(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Abnormal_SocTempState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>OUT");
}

void IAccOn_AbormalState::handlePM_Soc_Voltage_High(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Abnormal_SocVoltageState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>OUT");
}

void IAccOn_AbormalState::handlePM_Soc_Voltage_Low(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Abnormal_SocVoltageState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>OUT");
}

void IAccOn_AbormalState::handleSCR_TEMP_HIGH_95_100(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Abnormal_ScrTempHighState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>OUT");
}

void IAccOn_AbormalState::handleSCR_TEMP_HIGH_100(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Abnormal_ScrTempHigherState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>OUT");
}

void IAccOn_AbormalState::handleAPP_ABNORMAL_AWAKE_ON(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Abnormal_AwakeState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_AbormalState::",__FUNCTION__,"-->>OUT");
}



/**********************************IAccOn_AbormalState**************************************/

/**********************************IAccOn_Normal_PowerOnState**************************************/

void IAccOn_Normal_PowerOnState::handlePM_PowerOff(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_Normal_PowerOnState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
    changeNormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_Normal_PowerOnState::",__FUNCTION__,"-->>OUT");
}
void IAccOn_Normal_PowerOnState::handleRSE_POWER_OFF(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_Normal_PowerOnState::",__FUNCTION__,"-->>IN");

    if(!(pPowerDevice->isIVIDevice())){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }

    logVerbose(PMC,"IAccOn_Normal_PowerOnState::",__FUNCTION__,"-->>OUT");
}

void IAccOn_Normal_PowerOnState::handleAPP_SET_GOODBYE_MODE(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_Normal_PowerOnState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Goodbye_State>();
    changeNormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"IAccOn_Normal_PowerOnState::",__FUNCTION__,"-->>OUT");
}

/**********************************IAccOn_Normal_PowerOnState**************************************/

/**********************************IAccOn_Normal_PowerOffState**************************************/
void IAccOn_Normal_PowerOffState::handlePM_PowerOn(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_Normal_PowerOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }else{
        //do nothing
    }

    logVerbose(PMC,"IAccOn_Normal_PowerOffState::",__FUNCTION__,"-->>OUT");
}
void IAccOn_Normal_PowerOffState::handleRSE_POWER_ON(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"IAccOn_Normal_PowerOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //do nothing for IVIDevice
        logDebug(PMC,"do nothing for IVIPowerDevice!");
    }else{
        if(PowerManagerController::GetInstance()->isIVIDevicePowerOn()){
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
        }
    }

    logVerbose(PMC,"IAccOn_Normal_PowerOffState::",__FUNCTION__,"-->>OUT");
}
/**********************************IAccOn_Normal_PowerOffState**************************************/
