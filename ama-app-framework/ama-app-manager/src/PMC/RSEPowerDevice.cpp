/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "RSEPowerDevice.h"
#include "AMGRLogContext.h"
#include "ama_enum.h"
#include "PowerState.h"
#include "LCDManager.h"
#include "Configuration.h"//for AMGRConf.txt
#include "CAbnormalMgr.h"

RSEPowerDevice::RSEPowerDevice(E_SEAT seatID)
    :PowerDevice(seatID)
{
    logVerbose(PMC,"RSEPowerDevice::RSEPowerDevice()-->IN");
    if (IsRearAlwaysON()){
        logInfo(PMC,"AMGRConfig.txt config Rear is always ON");
        mpNormalPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
    }
    else{
        mpNormalPowerState = std::make_shared<AccOffState>();
    }
    updateFinalPowerState(mpNormalPowerState);
    refreshDeviceByPowerState(mpFinalPowerState->GetPowerStateFlag());
    logVerbose(PMC,"RSEPowerDevice::RSEPowerDevice()-->OUT");
}

RSEPowerDevice::~RSEPowerDevice()
{
    logVerbose(PMC,"RSEPowerDevice::~RSEPowerDevice()-->IN");
    logVerbose(PMC,"RSEPowerDevice::~RSEPowerDevice()-->OUT");
}

void RSEPowerDevice::handlePowerSignal(ama_PowerSignal_e powerSignal)
{
    logVerbose(PMC,"RSEPowerDevice::handlePowerSignal()-->IN powerSignal:",powerSignal);

  //  recordAbnormalFlag(powerSignal);

    switch(powerSignal){
    case E_PM_ACC_ON_POWER_ON:          mpNormalPowerState->handlePM_AccOn_PowerOn(this);      break;
    case E_PM_ACC_ON_POWER_OFF:         mpNormalPowerState->handlePM_AccOn_PowerOff(this);     break;
    case E_PM_ACC_OFF:                  mpNormalPowerState->handlePM_AccOff(this);             break;
    case E_PM_POWER_ON:                 mpNormalPowerState->handlePM_PowerOn(this);            break;
    case E_PM_POWER_OFF:                mpNormalPowerState->handlePM_PowerOff(this);           break;
    // case E_PM_SOC_TEMP_HIGH:            mpAbnormalPowerState->handlePM_Soc_Temp_High(this);      break;
    // case E_PM_SOC_TEMP_NORMAL:          mpAbnormalPowerState->handlePM_Soc_Temp_Normal(this);    break;
    // case E_PM_SOC_VOLTAGE_HIGH:         mpAbnormalPowerState->handlePM_Soc_Voltage_High(this);   break;
    // case E_PM_SOC_VOLTAGE_NORMAL:       mpAbnormalPowerState->handlePM_Soc_Voltage_Normal(this); break;
    // case E_PM_SOC_VOLTAGE_LOW:          mpAbnormalPowerState->handlePM_Soc_Voltage_Low(this);    break;
    // case E_SCR_TEMP_HIGH_90:            mpAbnormalPowerState->handleSCR_TEMP_HIGH_90(this);      break;
    // case E_SCR_TEMP_HIGH_90_95:         mpAbnormalPowerState->handleSCR_TEMP_HIGH_90_95(this);   break;
    // case E_SCR_TEMP_HIGH_95_100:        mpAbnormalPowerState->handleSCR_TEMP_HIGH_95_100(this);  break;
    // case E_SCR_TEMP_HIGH_100:           mpAbnormalPowerState->handleSCR_TEMP_HIGH_100(this);     break;
    case E_PM_SOC_TEMP_HIGH:            
    case E_PM_SOC_TEMP_NORMAL:          
    case E_PM_SOC_VOLTAGE_HIGH:         
    case E_PM_SOC_VOLTAGE_NORMAL:       
    case E_PM_SOC_VOLTAGE_LOW:         
    case E_SCR_TEMP_HIGH_90:           
    case E_SCR_TEMP_HIGH_90_95:         
    case E_SCR_TEMP_HIGH_95_100:        
    case E_SCR_TEMP_HIGH_100:      
    {
        mpcabnormalmgr->recordAbnormalFlag(powerSignal);
        mpcabnormalmgr->updateCurAbnormalState();
        break;
    } 
    case E_RSE_POWER_ON:                mpNormalPowerState->handleRSE_POWER_ON(this);          break;
    case E_RSE_POWER_OFF:               mpNormalPowerState->handleRSE_POWER_OFF(this);         break;
    case E_RSE_SCREEN_ON:               mpNormalPowerState->handleRSE_SCREEN_ON(this);         break;
    case E_RSE_SCREEN_OFF:              mpNormalPowerState->handleRSE_SCREEN_OFF(this);        break;
    case E_RSE_TIMER_ENABLE:            this->setTimerState(true);                             break;
    case E_RSE_TIMER_DISABLE:           this->setTimerState(false);                            break;
    default: logInfo(PMC,"RSE powerDevice  don't need to handle this powerSignal");    break;
    }

    if(getPowerLevel(mpNormalPowerState->GetPowerStateFlag()) < getPowerLevel(mpcabnormalmgr->getCurAbnormalStateFlag()))
    {
        logDebug(PMC,"RSEPowerDevice::handlePowerSignal()-->mpAbnormalPowerState");
        changePowerStateTo(mpcabnormalmgr->getCurAbnormalState());
    }else
    {
        logDebug(PMC,"RSEPowerDevice::handlePowerSignal()-->mpNormalPowerState");
        changePowerStateTo(mpNormalPowerState);
    }

    logVerbose(PMC,"RSEPowerDevice::handlePowerSignal()-->OUT");
}
void RSEPowerDevice::setTimerState(bool isEnable)
{
    logVerbose(PMC,"RSEPowerDevice::setTimerState()-->IN");
    LCDManager::GetInstance()->SetRearTimerState(isEnable,mSeatID);
    logVerbose(PMC,"RSEPowerDevice::setTimerState()-->OUT");
}
void RSEPowerDevice::recordLastNormalState(ama_PowerState_e powerStateFlag)
{
    logVerbose(PMC,"RSEPowerDevice::recordLastNormalState()-->IN");
    switch(powerStateFlag){
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
    case E_PS_ACCON_NORMAL_POWEROFF:
        mLastNormalPowerState = powerStateFlag;
        break;
    default:
        logInfo(PMC,"don't record other powerState");
        break;
    }
    logVerbose(PMC,"RSEPowerDevice::recordLastNormalState()-->OUT");
}


// void RSEPowerDevice::changePowerStateByRecord(void)
// {
//     logVerbose(PMC,"RSEPowerDevice::changePowerStateByRecord()-->IN");

//     if(!mpcabnormalmgr->getIsSocVoltageNormal()){
//         changePowerStateTo(std::make_shared<AccOn_Abnormal_SocVoltageState>());
//     }else if(!mpcabnormalmgr->getIsSocTempNormal()){
//         changePowerStateTo(std::make_shared<AccOn_Abnormal_SocTempState>());
//     }else if(E_SCR_TEMP_HIGH_100==mpcabnormalmgr->getLastScreenTemp()){
//         changePowerStateTo(std::make_shared<AccOn_Abnormal_ScrTempHigherState>());
//     }else if(E_SCR_TEMP_HIGH_95_100==mpcabnormalmgr->getLastScreenTemp()){
//         changePowerStateTo(std::make_shared<AccOn_Abnormal_ScrTempHighState>());
//     }else{
//         switch(mLastNormalPowerState){
//         case E_PS_ACCON_NORMAL_POWERON_SCRON:
//             changePowerStateTo(std::make_shared<AccOn_Normal_PowerOn_ScrOnState>());
//             break;
//         case E_PS_ACCON_NORMAL_POWERON_SCROFF:
//             changePowerStateTo(std::make_shared<AccOn_Normal_PowerOn_ScrOffState>());
//             break;
//         case E_PS_ACCON_NORMAL_POWEROFF:
//             changePowerStateTo(std::make_shared<AccOn_Normal_PowerOffState>());
//             break;
//         default:
//             logError(PMC,"Wrong powerState");
//             break;
//         }
//     }

//     logVerbose(PMC,"RSEPowerDevice::changePowerStateByRecord()-->OUT");
// }

void RSEPowerDevice::refreshDeviceByPowerState(ama_PowerState_e powerStateFlag)
{
    logVerbose(PMC,"RSEPowerDevice::RSEPowerDevice()-->IN");
    if (IsRearAlwaysON()){
           logInfo(PMC,"powerStateFlag config set RearAlwaysON!!!");
           setLCDState(OPEN);
           setTouchState(ENABLE);//if double click open LCD
           setAudioState(ENABLE);
           return ;
       }
    switch(powerStateFlag){
    case E_PS_ACCOFF:
    case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
    case E_PS_ACCON_ABNORMAL_SOCTEMP:
    case E_PS_ACCON_NORMAL_POWEROFF:
        setLCDState(CLOSE);
        setTouchState(DISABLE);//if double click open LCD
        setAudioState(DISABLE);
        setNormalBrightness();
        break;
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
        setLCDState(OPEN);
        setTouchState(ENABLE);//if double click open LCD
        setAudioState(ENABLE);
        setTouchForbiden(DISABLE);//Discard every Touch Message
        setLowestBrightness();
        break;
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
        setLCDState(CLOSE);
        setTouchState(ENABLE);//if double click open LCD
        setAudioState(ENABLE);
        setTouchForbiden(ENABLE);//Discard every Touch Message
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
        setLCDState(OPEN);
        setTouchState(ENABLE);//if double click open LCD
        setAudioState(ENABLE);
        setNormalBrightness();
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
        setLCDState(CLOSE);
        setTouchState(DISABLE);//if double click open LCD
        setAudioState(ENABLE);
        setNormalBrightness();
        break;
    case E_PS_GOODBYE_MODE:
        setLCDState(OPEN);
        setTouchState(DISABLE);//if double click open LCD
        setAudioState(DISABLE);
        setNormalBrightness();
        break;
    default:
        logError(PMC,"powerStateFlag is not defined");
        break;
    }
    logVerbose(PMC,"RSEPowerDevice::RSEPowerDevice()-->OUT");
}
