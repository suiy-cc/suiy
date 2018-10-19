/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
 extern "C" {
 #include <libpwrmgr.h>
 }
#include "PowerManager.h"
#include "AMGRLogContext.h"
#include "PLCTask.h"
#include "PMCTask.h"
#include "TaskDispatcher.h"
#include "LCDManager.h"
#include "USBManager.h"
#include "BootListener.h"
#include "Capi.h"

static const char * pmcUnitName={"PMController"};

ama_PowerSignal_e pwrSignal2amaPMSignal(int powerMode, int detail)
{
    switch(powerMode){
    case PWRMGR_ACC_ON:
        BootListener::GetInstance()->reqWelcomeFlg();
        if(POWER_MODE_OFF==detail){
            return E_PM_ACC_ON_POWER_OFF;
        }else{
            return E_PM_ACC_ON_POWER_ON;
        }
    case PWRMGR_ACC_OFF:    return E_PM_ACC_OFF;
    case PWRMGR_POWER_ON:    return E_PM_POWER_ON;
    case PWRMGR_POWER_OFF:    return E_PM_POWER_OFF;
    case PWRMGR_POWER_OFF_AWAKE:    return E_PM_POWER_OFF_AWAKE;
    case PWRMGR_SCREEN_ON:    return E_PM_SCREEN_ON;
    case PWRMGR_SCREEN_OFF:    return E_PM_SCREEN_OFF;
    case PWRMGR_SCREEN_OFF_AWAKE:    return E_PM_SCREEN_OFF_AWAKE;
    case PWRMGR_IGN_NOT_COME:
        if(POWER_MODE_IGN_NOT_REV==detail){
            return E_PM_IGN_NOT_COME;
        }else{
            return E_PM_IGN_COME;
        }
    case PWRMGR_HITEMP_CHG:
        if(POWER_HIGH_TEMP==detail){
            return E_PM_SOC_TEMP_HIGH;
        }else{
            return E_PM_SOC_TEMP_NORMAL;
        }
    case PWRMGR_HIVOLTAGE_CHG:
        if(POWER_HIGH_VOLTAGE==detail){
            return E_PM_SOC_VOLTAGE_HIGH;
        }else if(POWER_NORMAL_VOLTAGE==detail){
            return E_PM_SOC_VOLTAGE_NORMAL;
        }else{
            return E_PM_SOC_VOLTAGE_LOW;
        }
    default:
        logError(PMC,"pwrSignal is not defined");
        break;
    }
}

///////////////////////callback function///////////////////////////
static void powerManager_callback(int powerMode, int detail)
{
    logVerbose(PMC,"powerManager_callback-->IN (powerMode= ",powerMode, " detail= " ,detail, ")");

    PowerManager::GetInstance()->sendSignalToPMC(powerMode,detail);

    notifyPowerManager (pmcUnitName,powerMode);//NOTIFY SINGNAL RECEIVED

    logVerbose(PMC,"powerManager_callback-->OUT");
}

void usbStatusCB(ama_USBSignal signal)
{
    logVerboseF(PMC,"usbStatusCB--> IN! signal:0x%04x",signal);
    BroadcastAMGRSetNotify(E_USB_SIGNAL_CHANGED,to_string(signal));
    logVerbose(PMC,"usbStatusCB--> OUT! signal:0x%04x",signal);
}
void lcdmRearTouchStatusCB (E_SEAT seatID,ama_PowerSignal_e signal)
{
    logVerbose(PMC,"rearTouchStatusCB--> IN! seatID:",seatID,"signal:",signal);
    auto pmcTask = make_shared<PMCTask>();
    pmcTask->setSeatID(seatID);
    pmcTask->SetType(HANDLE_LCD_SIGNAL);
    pmcTask->setPowerSignal(signal);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);
    logVerbose(PMC,"rearTouchStatusCB-->OUT! seatID:",seatID,"signal:",signal);
}
///////////////////////callback function///////////////////////////
PowerManager::PowerManager()
{
    logVerbose(PMC,"PowerManager::PowerManager()-->IN");
    logVerbose(PMC,"PowerManager::PowerManager()-->OUT");
}

PowerManager::~PowerManager()
{
    logVerbose(PMC,"PowerManager::~PowerManager()-->IN");

    DeInit();

    logVerbose(PMC,"PowerManager::~PowerManager()-->OUT");
}

void PowerManager::Init(void)
{
    logVerbose(PMC,"PowerManager::Init()-->IN");

    //init & register PowerManager
    powerManagerInit();
    //register & set callback func
    registerPowerManager(pmcUnitName,*powerManager_callback);

#ifndef CODE_FOR_EV
    //if project-ev do not do this
    StartListenRearTouchStatusCB(lcdmRearTouchStatusCB);
#endif
    StartListenUSBStateCB( usbStatusCB);
    logVerbose(PMC,"PowerManager::Init()-->OUT");
}

void PowerManager::sendSignalToPMC(int powerMode, int detail)
{
    logVerbose(PMC,"PowerManager::sendSignalToPMC()-->IN powerMode:",powerMode," detail:",detail);

    ama_PowerSignal_e powerSignal = pwrSignal2amaPMSignal(powerMode,detail);

    auto pmcTask = make_shared<PMCTask>();
    pmcTask->SetType(HANDLE_PM_SIGNAL);
    pmcTask->setPowerSignal(powerSignal);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);

    logVerbose(PMC,"PowerManager::sendSignalToPMC()-->OUT");
}

void PowerManager::responsePowerMode(int powerMode)
{
    logVerbose(PMC,"PowerManager::responsePowerMode()-->IN powerMode:",powerMode);

    notifyPowerManager(pmcUnitName,powerMode);

    logVerbose(PMC,"PowerManager::responsePowerMode()-->OUT");
}

void PowerManager::activeAppStaChanged()
{
    logVerbose(PMC,"PowerManager::activeAppStaChanged()-->IN");

    logVerbose(PMC,"PowerManager::activeAppStaChanged()-->OUT");
}

void PowerManager::SetScreenOnByApp()
{
    logVerbose(PMC,"PowerManager::SetScreenOnByApp()--> IN");
    reqModeToPowerManager(REQ_POWER_SCREEN_ON);
    logVerbose(PMC,"PowerManager::SetScreenOnByApp()-->OUT");
}

bool PowerManager::isAccOn(void)
{
    /**
    * @brief mode change to power manager
    * @return int
    * @retval 0 : POWER_OFF
    * @retval 1 : POWER_ON
    * @retval 2 : POWER_OFF_ON
    * @retval 3 : SCREEN_OFF
    * @retval 4 : SCREEN_ON
    * @retval 5 : SCREEN_OFF_ON
    * @retval 0xff: standby
    */
    int ret = getPowerManagerMode();
    logDebug(PMC,"getPowerManagerMode return:",ToHEX(ret));
    if(0xff==ret){
        return false;
    }else{
        return true;
    }
}

bool PowerManager::isPowerOn(void)
{
    /**
    * @brief mode change to power manager
    * @return int
    * @retval 0 : POWER_OFF
    * @retval 1 : POWER_ON
    * @retval 2 : POWER_OFF_ON
    * @retval 3 : SCREEN_OFF
    * @retval 4 : SCREEN_ON
    * @retval 5 : SCREEN_OFF_ON
    */
    int ret = getPowerManagerMode();
    switch(ret){
        case 0:
        case 0xff:
            return false;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            return true;
        default:
            logError(PMC,"getPowerManagerMode() return a undefine value!");
            return false;
    }
}

void PowerManager::DeInit()
{
    logVerbose(PMC,"PowerManager::DeInit()-->IN");

    powerManagerDestory();

    StopListenRearTouchStatusCB(lcdmRearTouchStatusCB);

    StopListenUSBStateCB(usbStatusCB);
    logVerbose(PMC,"PowerManager::DeInit()-->OUT");
}
