/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "PowerState.h"
#include "PowerDevice.h"
#include "AppAwakeMgr.h"
#include "BootListener.h"
#include "PowerManager.h"

/*************************************PowerInitState*****************************************/
PowerInitState::PowerInitState()
{
    logVerbose(PMC,"PowerInitState::PowerInitState()-->IN");
    logVerbose(PMC,"PowerInitState::PowerInitState()-->OUT");
}
PowerInitState::~PowerInitState()
{
    logVerbose(PMC,"PowerInitState::~PowerInitState()-->IN");
    logVerbose(PMC,"PowerInitState::~PowerInitState()-->OUT");
}
ama_PowerState_e PowerInitState::GetPowerStateFlag()
{
    return E_PS_POWER_INIT;
}
/*************************************PowerInitState*****************************************/

/***********************************WelcomeState***********************************/
WelcomeState::WelcomeState()
{
    logVerbose(PMC,"WelcomeState::WelcomeState()-->IN");
    logVerbose(PMC,"WelcomeState::WelcomeState()-->OUT");
}
WelcomeState::~WelcomeState()
{
    logVerbose(PMC,"WelcomeState::~WelcomeState()-->IN");
    logVerbose(PMC,"WelcomeState::~WelcomeState()-->OUT");
}
ama_PowerState_e WelcomeState::GetPowerStateFlag()
{
    return E_PS_WELCOME_MODE;
}

void WelcomeState::handlePM_AccOn_PowerOn(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //only IVIPowerDevice have this power state
        bool isWelcomeOver = BootListener::GetInstance()->isWelcomeOver();
        bool isFirstAppReady = BootListener::GetInstance()->isFirstAppReady();
        if(isWelcomeOver&&isFirstAppReady){
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
            BootListener::GetInstance()->stopListen();
        }
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>OUT");
}
void WelcomeState::handlePM_AccOn_PowerOff(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //only IVIPowerDevice have this power state
        bool isWelcomeOver = BootListener::GetInstance()->isWelcomeOver();
        bool isFirstAppReady = BootListener::GetInstance()->isFirstAppReady();
        if(isWelcomeOver&&isFirstAppReady){
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
            BootListener::GetInstance()->stopListen();
        }
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>OUT");
}

void WelcomeState::handleBOOT_WELCOME_PLAY_OVER(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //only IVIPowerDevice have this power state
        bool isFirstAppReady = BootListener::GetInstance()->isFirstAppReady();
        bool isAccOn = PowerManager::GetInstance()->isAccOn();
        bool isPowerOn = PowerManager::GetInstance()->isPowerOn();
        if(isAccOn&&isFirstAppReady){
            if(isPowerOn){
                auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
                changeNormalStateTo(pPowerDevice,pPowerState);
                BootListener::GetInstance()->stopListen();
            }else{
                auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
                changeNormalStateTo(pPowerDevice,pPowerState);
                BootListener::GetInstance()->stopListen();
            }
        }else if(isAccOn){
            auto pPowerState = std::make_shared<OpeningState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
        }else if(!isAccOn){
            auto pPowerState = std::make_shared<AccOffState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
        }
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>OUT");
}

void WelcomeState::handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //only IVIPowerDevice have this power state
        bool isWelcomeOver = BootListener::GetInstance()->isWelcomeOver();
        bool isAccOn = PowerManager::GetInstance()->isAccOn();
        bool isPowerOn = PowerManager::GetInstance()->isPowerOn();
        if(isWelcomeOver&&isAccOn){
            if(isPowerOn){
                auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
                changeNormalStateTo(pPowerDevice,pPowerState);
                BootListener::GetInstance()->stopListen();
            }else{
                auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
                changeNormalStateTo(pPowerDevice,pPowerState);
                BootListener::GetInstance()->stopListen();
            }
        }
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"WelcomeState::",__FUNCTION__,"-->>OUT");
}
/***********************************WelcomeState***********************************/

/***********************************OpeningState***********************************/
OpeningState::OpeningState()
{
    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>OUT");
}
OpeningState::~OpeningState()
{
    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>IN");
  //  BootListener::GetInstance()->stopOpeningMode();
    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>OUT");
}
ama_PowerState_e OpeningState::GetPowerStateFlag()
{
    return E_PS_OPENING_MODE;
}

void OpeningState::handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //only IVIPowerDevice have this power state
        bool isPowerOn = PowerManager::GetInstance()->isPowerOn();
        if(isPowerOn){
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
            BootListener::GetInstance()->stopListen();
        }else{
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
            BootListener::GetInstance()->stopListen();
        }
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>OUT");
}

void OpeningState::handlePM_AccOff(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOffState>();
    changeNormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"OpeningState::",__FUNCTION__,"-->>OUT");
}
/***********************************WelcomeState***********************************/

/*************************************AccOffState*****************************************/
AccOffState::AccOffState()
{
    logVerbose(PMC,"AccOffState::AccOffState()-->IN");
    logVerbose(PMC,"AccOffState::AccOffState()-->OUT");
}

AccOffState::~AccOffState()
{
    logVerbose(PMC,"AccOffState::~AccOffState()-->IN");
    logVerbose(PMC,"AccOffState::~AccOffState()-->OUT");
}

ama_PowerState_e AccOffState::GetPowerStateFlag()
{
    return E_PS_ACCOFF;
}

void AccOffState::handlePM_AccOn_PowerOn(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        bool isFirstAppReady = BootListener::GetInstance()->isFirstAppReady();
        if(isFirstAppReady){
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
            BootListener::GetInstance()->stopListen();
        }else{
            auto pPowerState = std::make_shared<OpeningState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
        }
    }else{
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }

    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>OUT");
}

void AccOffState::handlePM_AccOn_PowerOff(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        bool isFirstAppReady = BootListener::GetInstance()->isFirstAppReady();
        if(isFirstAppReady){
            auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
            BootListener::GetInstance()->stopListen();
        }else{
            auto pPowerState = std::make_shared<OpeningState>();
            changeNormalStateTo(pPowerDevice,pPowerState);
        }
    }else{
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }

    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>OUT");
}

void AccOffState::handleBOOT_WELCOME_PLAY_START(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<WelcomeState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>OUT");
}

void AccOffState::handleBOOT_FIRST_APP_READY(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //only IVIPowerDevice have this power state
        logDebug(PMC,"only IVIpowerDevice handle E_BOOT_FIRST_APP_READY signal!");
        bool isAccOn = PowerManager::GetInstance()->isAccOn();
        bool isPowerOn = PowerManager::GetInstance()->isPowerOn();
        if(isAccOn){
            if(isPowerOn){
                auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
                changeNormalStateTo(pPowerDevice,pPowerState);
                BootListener::GetInstance()->stopListen();
            }else{
                auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
                changeNormalStateTo(pPowerDevice,pPowerState);
                BootListener::GetInstance()->stopListen();
            }
        }
    }else{
        logError(PMC,"RSEPowerDevice don't have this power state");
    }

    logVerbose(PMC,"AccOffState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOffState*****************************************/

/*************************************AccOn_Goodbye_State*****************************************/
AccOn_Goodbye_State::AccOn_Goodbye_State()
{
    logVerbose(PMC,"AccOn_Goodbye_State::AccOn_Goodbye_State()-->IN");
    logVerbose(PMC,"AccOn_Goodbye_State::AccOn_Goodbye_State()-->OUT");
}
AccOn_Goodbye_State::~AccOn_Goodbye_State()
{
    logVerbose(PMC,"AccOn_Goodbye_State::AccOn_Goodbye_State()-->IN");
    logVerbose(PMC,"AccOn_Goodbye_State::AccOn_Goodbye_State()-->OUT");
}
ama_PowerState_e AccOn_Goodbye_State::GetPowerStateFlag()
{
    return E_PS_GOODBYE_MODE;
}
/*************************************AccOn_Goodbye_State*****************************************/

/*************************************AccOn_Abnormal_SocVoltageState*****************************************/
AccOn_Abnormal_SocVoltageState::AccOn_Abnormal_SocVoltageState()
{
    logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::AccOn_Abnormal_SocVoltageState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::AccOn_Abnormal_SocVoltageState()-->OUT");
}
AccOn_Abnormal_SocVoltageState::~AccOn_Abnormal_SocVoltageState()
{
    logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::AccOn_Abnormal_SocVoltageState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::AccOn_Abnormal_SocVoltageState()-->OUT");
}
ama_PowerState_e AccOn_Abnormal_SocVoltageState::GetPowerStateFlag()
{
    return E_PS_ACCON_ABNORMAL_SOCVOLTAGE;
}

// void AccOn_Abnormal_SocVoltageState::handlePM_Soc_Voltage_Normal(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::",__FUNCTION__,"-->>IN");

//     auto pPowerState = std::make_shared<IAccOn_AbormalState>();
//     //changeStateByDeviceRecord(pPowerDevice);
//     changeAbnormalStateTo(pPowerDevice, pPowerState);

//     logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::",__FUNCTION__,"-->>OUT");
// }

void AccOn_Abnormal_SocVoltageState::handleABN_To_Normal(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<IAccOn_AbormalState>();
    //changeStateByDeviceRecord(pPowerDevice);
    changeAbnormalStateTo(pPowerDevice, pPowerState);

    logVerbose(PMC,"AccOn_Abnormal_SocVoltageState::",__FUNCTION__,"-->>OUT");
}

/*************************************AccOn_Abnormal_SocVoltageState*****************************************/

/*************************************AccOn_Abnormal_SocTempState*****************************************/
AccOn_Abnormal_SocTempState::AccOn_Abnormal_SocTempState()
{
    logVerbose(PMC,"AccOn_Abnormal_SocTempState::AccOn_Abnormal_SocTempState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_SocTempState::AccOn_Abnormal_SocTempState()-->OUT");
}
AccOn_Abnormal_SocTempState::~AccOn_Abnormal_SocTempState()
{
    logVerbose(PMC,"AccOn_Abnormal_SocTempState::AccOn_Abnormal_SocTempState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_SocTempState::AccOn_Abnormal_SocTempState()-->OUT");
}
ama_PowerState_e AccOn_Abnormal_SocTempState::GetPowerStateFlag()
{
    return E_PS_ACCON_ABNORMAL_SOCTEMP;
}
// void AccOn_Abnormal_SocTempState::handlePM_Soc_Temp_Normal(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_SocTempState::",__FUNCTION__,"-->>IN");
//     auto pPowerState = std::make_shared<IAccOn_AbormalState>();
//     //changeStateByDeviceRecord(pPowerDevice);
//     changeAbnormalStateTo(pPowerDevice, pPowerState);
//     logVerbose(PMC,"AccOn_Abnormal_SocTempState::",__FUNCTION__,"-->>OUT");
// }

void AccOn_Abnormal_SocTempState::handleABN_To_Normal(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_SocTempState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<IAccOn_AbormalState>();
    //changeStateByDeviceRecord(pPowerDevice);
    changeAbnormalStateTo(pPowerDevice, pPowerState);

    logVerbose(PMC,"AccOn_Abnormal_SocTempState::",__FUNCTION__,"-->>OUT");
}

/*************************************AccOn_Abnormal_SocTempState*****************************************/

/*************************************AccOn_Abnormal_ScrTempHighState*****************************************/
AccOn_Abnormal_ScrTempHighState::AccOn_Abnormal_ScrTempHighState()
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::AccOn_Abnormal_ScrTempHighState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::AccOn_Abnormal_ScrTempHighState()-->OUT");
}
AccOn_Abnormal_ScrTempHighState::~AccOn_Abnormal_ScrTempHighState()
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::AccOn_Abnormal_ScrTempHighState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::AccOn_Abnormal_ScrTempHighState()-->OUT");
}
ama_PowerState_e AccOn_Abnormal_ScrTempHighState::GetPowerStateFlag()
{
    return E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH;
}
// void AccOn_Abnormal_ScrTempHighState::handleSCR_TEMP_HIGH_90(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>IN");
//         auto pPowerState = std::make_shared<IAccOn_AbormalState>();
//     //changeStateByDeviceRecord(pPowerDevice);
//     changeAbnormalStateTo(pPowerDevice, pPowerState);
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>OUT");
// }

void AccOn_Abnormal_ScrTempHighState::handleABN_To_Normal(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<IAccOn_AbormalState>();
    //changeStateByDeviceRecord(pPowerDevice);
    changeAbnormalStateTo(pPowerDevice, pPowerState);

    logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>OUT");
}

// void AccOn_Abnormal_ScrTempHighState::handleSCR_TEMP_HIGH_100(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>IN");
//     auto pPowerState = std::make_shared<AccOn_Abnormal_ScrTempHigherState>();
//     changeAbnormalStateTo(pPowerDevice, pPowerState);
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>OUT");
// }

// void AccOn_Abnormal_ScrTempHighState::handlePM_Soc_Voltage_High(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>IN");
//     auto pPowerState = std::make_shared<AccOn_Abnormal_SocVoltageState>();
//     changeAbnormalStateTo(pPowerDevice, pPowerState);;
//     logVerbose(PMC,"WelcoAccOn_Abnormal_ScrTempHighStatemeState::",__FUNCTION__,"-->>OUT");
// }

// void AccOn_Abnormal_ScrTempHighState::handlePM_Soc_Voltage_Low(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>IN");
//     auto pPowerState = std::make_shared<AccOn_Abnormal_SocVoltageState>();
//     changeAbnormalStateTo(pPowerDevice, pPowerState);
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>OUT");
// }

// void AccOn_Abnormal_ScrTempHighState::handlePM_Soc_Temp_High(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>IN");
//     auto pPowerState = std::make_shared<AccOn_Abnormal_SocTempState>();
//     changeAbnormalStateTo(pPowerDevice, pPowerState);
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHighState::",__FUNCTION__,"-->>OUT");
// }

/*************************************AccOn_Abnormal_ScrTempHighState*****************************************/

/*************************************AccOn_Abnormal_ScrTempHigherState*****************************************/
AccOn_Abnormal_ScrTempHigherState::AccOn_Abnormal_ScrTempHigherState()
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::AccOn_Abnormal_ScrTempHigherState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::AccOn_Abnormal_ScrTempHigherState()-->OUT");
}
AccOn_Abnormal_ScrTempHigherState::~AccOn_Abnormal_ScrTempHigherState()
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::AccOn_Abnormal_ScrTempHigherState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::AccOn_Abnormal_ScrTempHigherState()-->OUT");
}
ama_PowerState_e AccOn_Abnormal_ScrTempHigherState::GetPowerStateFlag()
{
    return E_PS_ACCON_ABNORMAL_SOCTEMP;
}

void AccOn_Abnormal_ScrTempHigherState::handleABN_To_Normal(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<IAccOn_AbormalState>();
    //changeStateByDeviceRecord(pPowerDevice);
    changeAbnormalStateTo(pPowerDevice, pPowerState);

    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::",__FUNCTION__,"-->>OUT");
}

// void AccOn_Abnormal_ScrTempHigherState::handleSCR_TEMP_HIGH_90(PowerDevice* pPowerDevice)
// {
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::",__FUNCTION__,"-->>IN");
//     auto pPowerState = std::make_shared<IAccOn_AbormalState>();
//     //changeStateByDeviceRecord(pPowerDevice);
//     changeAbnormalStateTo(pPowerDevice, pPowerState);
//     logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::",__FUNCTION__,"-->>OUT");
// }
void AccOn_Abnormal_ScrTempHigherState::handleSCR_TEMP_HIGH_90_95(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::",__FUNCTION__,"-->>IN");
    auto pPowerState = std::make_shared<AccOn_Abnormal_ScrTempHighState>();
    changeAbnormalStateTo(pPowerDevice, pPowerState);
    logVerbose(PMC,"AccOn_Abnormal_ScrTempHigherState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOn_Abnormal_ScrTempHigherState*****************************************/

/*************************************AccOn_Abnormal_AwakeState*****************************************/

AccOn_Abnormal_AwakeState::AccOn_Abnormal_AwakeState()
{
    logVerbose(PMC,"AccOn_Abnormal_AwakeState::AccOn_Abnormal_AwakeState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_AwakeState::AccOn_Abnormal_AwakeState()-->OUT");
}
AccOn_Abnormal_AwakeState::~AccOn_Abnormal_AwakeState()
{
    logVerbose(PMC,"AccOn_Abnormal_AwakeState::AccOn_Abnormal_AwakeState()-->IN");
    logVerbose(PMC,"AccOn_Abnormal_AwakeState::AccOn_Abnormal_AwakeState()-->OUT");
}

ama_PowerState_e AccOn_Abnormal_AwakeState::GetPowerStateFlag()
{
    return E_PS_ACCON_ABNORMAL_AWAKE;
}

void AccOn_Abnormal_AwakeState::handleABN_To_Normal(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_AwakeState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<IAccOn_AbormalState>();
    //changeStateByDeviceRecord(pPowerDevice);
    changeAbnormalStateTo(pPowerDevice, pPowerState);

    logVerbose(PMC,"AccOn_Abnormal_AwakeState::",__FUNCTION__,"-->>OUT");
}

void AccOn_Abnormal_AwakeState::handleAPP_ABNORMAL_AWAKE_OFF(CAbnormalMgr* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Abnormal_AwakeState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<IAccOn_AbormalState>();
    changeAbnormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"AccOn_Abnormal_AwakeState::",__FUNCTION__,"-->>OUT"); 
}


/*************************************AccOn_Abnormal_AwakeState*****************************************/

/*************************************AccOn_Normal_PowerOn_ScrOnState*****************************************/
AccOn_Normal_PowerOn_ScrOnState::AccOn_Normal_PowerOn_ScrOnState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::AccOn_Normal_PowerOn_ScrOnState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::AccOn_Normal_PowerOn_ScrOnState()-->OUT");
}
AccOn_Normal_PowerOn_ScrOnState::~AccOn_Normal_PowerOn_ScrOnState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::AccOn_Normal_PowerOn_ScrOnState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::AccOn_Normal_PowerOn_ScrOnState()-->OUT");
}
ama_PowerState_e AccOn_Normal_PowerOn_ScrOnState::GetPowerStateFlag()
{
    return E_PS_ACCON_NORMAL_POWERON_SCRON;
}
void AccOn_Normal_PowerOn_ScrOnState::handlePM_ScreenOff(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOffState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
        pPowerDevice->makeScreenShot();
    }else{
        logDebug(PMC,"do nothing for RSEPowerDevice");
    }

    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::",__FUNCTION__,"-->>OUT");
}
void AccOn_Normal_PowerOn_ScrOnState::handleRSE_SCREEN_OFF(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        logDebug(PMC,"do nothing for IVIPowerDevice");
    }else{
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOffState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }

    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOnState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOn_Normal_PowerOn_ScrOnState*****************************************/

/*************************************AccOn_Normal_PowerOn_ScrOffState*****************************************/
AccOn_Normal_PowerOn_ScrOffState::AccOn_Normal_PowerOn_ScrOffState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::AccOn_Normal_PowerOn_ScrOffState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::AccOn_Normal_PowerOn_ScrOffState()-->OUT");
}
AccOn_Normal_PowerOn_ScrOffState::~AccOn_Normal_PowerOn_ScrOffState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::AccOn_Normal_PowerOn_ScrOffState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::AccOn_Normal_PowerOn_ScrOffState()-->OUT");
}
ama_PowerState_e AccOn_Normal_PowerOn_ScrOffState::GetPowerStateFlag()
{
    return E_PS_ACCON_NORMAL_POWERON_SCROFF;
}
void AccOn_Normal_PowerOn_ScrOffState::handlePM_ScreenOn(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }else{
        //do nothing for RSEDevice
    }

    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::",__FUNCTION__,"-->>OUT");
}
void AccOn_Normal_PowerOn_ScrOffState::handleRSE_SCREEN_ON(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        //do nothing for RSEDevice
    }else{
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }

    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::",__FUNCTION__,"-->>OUT");
}

void AccOn_Normal_PowerOn_ScrOffState::handleAPP_SCREEN_AWAKE_ON(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOff_AwakeState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }else{
        //do nothing for RSEDevice
    }

    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOffState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOn_Abnormal_SocTempState*****************************************/

/*************************************AccOn_Normal_PowerOn_ScrOff_AwakeState*****************************************/
AccOn_Normal_PowerOn_ScrOff_AwakeState::AccOn_Normal_PowerOn_ScrOff_AwakeState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::AccOn_Normal_PowerOn_ScrOff_AwakeState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::AccOn_Normal_PowerOn_ScrOff_AwakeState()-->OUT");
}
AccOn_Normal_PowerOn_ScrOff_AwakeState::~AccOn_Normal_PowerOn_ScrOff_AwakeState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::AccOn_Normal_PowerOn_ScrOff_AwakeState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::AccOn_Normal_PowerOn_ScrOff_AwakeState()-->OUT");
}
ama_PowerState_e AccOn_Normal_PowerOn_ScrOff_AwakeState::GetPowerStateFlag()
{
    return E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE;
}
void AccOn_Normal_PowerOn_ScrOff_AwakeState::handlePM_ScreenOn(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::",__FUNCTION__,"-->>IN");

    auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
    changeNormalStateTo(pPowerDevice,pPowerState);

    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::",__FUNCTION__,"-->>OUT");
}
void AccOn_Normal_PowerOn_ScrOff_AwakeState::handleAPP_SCREEN_AWAKE_OFF(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::",__FUNCTION__,"-->>IN");
    auto pPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOffState>();
    changeNormalStateTo(pPowerDevice,pPowerState);
    logVerbose(PMC,"AccOn_Normal_PowerOn_ScrOff_AwakeState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOn_Normal_PowerOn_ScrOff_AwakeState*****************************************/



/*************************************AccOn_Normal_PowerOffState*****************************************/
AccOn_Normal_PowerOffState::AccOn_Normal_PowerOffState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOffState::AccOn_Normal_PowerOffState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOffState::AccOn_Normal_PowerOffState()-->OUT");
}
AccOn_Normal_PowerOffState::~AccOn_Normal_PowerOffState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOffState::AccOn_Normal_PowerOffState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOffState::AccOn_Normal_PowerOffState()-->OUT");
}
ama_PowerState_e AccOn_Normal_PowerOffState::GetPowerStateFlag()
{
    return E_PS_ACCON_NORMAL_POWEROFF;
}
void AccOn_Normal_PowerOffState::handleAPP_POWER_AWAKE_ON(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOffState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOff_AwakeState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }else{
        //do nothing for RSEDevice
    }

    logVerbose(PMC,"AccOn_Normal_PowerOffState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOn_Normal_PowerOffState*****************************************/


/*************************************AccOn_Normal_PowerOff_AwakeState*****************************************/
AccOn_Normal_PowerOff_AwakeState::AccOn_Normal_PowerOff_AwakeState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOff_AwakeState::AccOn_Normal_PowerOff_AwakeState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOff_AwakeState::AccOn_Normal_PowerOff_AwakeState()-->OUT");
}
AccOn_Normal_PowerOff_AwakeState::~AccOn_Normal_PowerOff_AwakeState()
{
    logVerbose(PMC,"AccOn_Normal_PowerOff_AwakeState::AccOn_Normal_PowerOff_AwakeState()-->IN");
    logVerbose(PMC,"AccOn_Normal_PowerOff_AwakeState::AccOn_Normal_PowerOff_AwakeState()-->OUT");
}
ama_PowerState_e AccOn_Normal_PowerOff_AwakeState::GetPowerStateFlag()
{
    return E_PS_ACCON_NORMAL_POWEROFF_AWAKE;
}

void AccOn_Normal_PowerOff_AwakeState::handleAPP_POWER_AWAKE_OFF(PowerDevice* pPowerDevice)
{
    logVerbose(PMC,"AccOn_Normal_PowerOff_AwakeState::",__FUNCTION__,"-->>IN");

    if(pPowerDevice->isIVIDevice()){
        auto pPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
        changeNormalStateTo(pPowerDevice,pPowerState);
    }else{
        //do nothing for RSEDevice
    }

    logVerbose(PMC,"AccOn_Normal_PowerOff_AwakeState::",__FUNCTION__,"-->>OUT");
}
/*************************************AccOn_Normal_PowerOff_AwakeState*****************************************/
