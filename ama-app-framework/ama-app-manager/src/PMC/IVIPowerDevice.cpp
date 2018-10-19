/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include "IVIPowerDevice.h"
#include "AMGRLogContext.h"
#include "ama_enum.h"
#include "PowerState.h"
#include "USBManager.h"
#include "HMICTask.h"
#include "BootListener.h"
#include "LineTool.h"
#include "AppAwakeMgr.h"
#include "rpc_proxy.h"
#include "CAbnormalMgr.h"

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

extern RpcProxy rp;

IVIPowerDevice::IVIPowerDevice()
:PowerDevice(E_SEAT_IVI)
,mPowerStateFileByRAM("/tmp/IVI-PowerState")
{
    logVerbose(PMC,"IVIPowerDevice::IVIPowerDevice()-->IN");
    usbCurrentNormal=true;
    usbfaultPath=(char *) hw_get_system_property_string(HW__USB__FAULT__DEV_NODE);

    if(isPowerStateRecordByRAM()){
        logInfo(PMC,"we need restore PowerState From RAM");
        restorePowerStateByRAM();
        updateFinalPowerState(mpNormalPowerState);
        refreshDeviceByPowerState(mpFinalPowerState->GetPowerStateFlag());
    }else{
        logInfo(PMC,"this is first start");
        bool isInWelcomeMode = BootListener::GetInstance()->checkIsWelcomePlaying();
        bool isInOpeningMode = BootListener::GetInstance()->checkIsOpeningPlaying();
        logInfo(PMC,"this is first start  isInWelcomeMode = ",isInWelcomeMode,"isInOpeningMode = ",isInOpeningMode);
        if(isInWelcomeMode){
            //if first app already ready now,this will be a wrong state!!!
            mpNormalPowerState = std::make_shared<WelcomeState>();
        }else if(isInOpeningMode){
            mpNormalPowerState = std::make_shared<OpeningState>();
        }else{
            mpNormalPowerState = std::make_shared<AccOffState>();
        }
        updateFinalPowerState(mpNormalPowerState);
        refreshDeviceByPowerState(mpFinalPowerState->GetPowerStateFlag());
    }

    logVerbose(PMC,"IVIPowerDevice::IVIPowerDevice()-->OUT");
}

IVIPowerDevice::~IVIPowerDevice()
{
    logVerbose(PMC,"IVIPowerDevice::~IVIPowerDevice()-->IN");
    logVerbose(PMC,"IVIPowerDevice::~IVIPowerDevice()-->OUT");
}

void IVIPowerDevice::handlePowerSignal(ama_PowerSignal_e powerSignal)
{
    logVerbose(PMC,"IVIPowerDevice::handlePowerSignal()-->IN powerSignal:",powerSignal);

  //  recordAbnormalFlag(powerSignal);

    switch(powerSignal){
    case E_PM_ACC_ON_POWER_ON:          mpNormalPowerState->handlePM_AccOn_PowerOn(this);      break;
    case E_PM_ACC_ON_POWER_OFF:         mpNormalPowerState->handlePM_AccOn_PowerOff(this);     break;
    case E_PM_ACC_OFF:                  mpNormalPowerState->handlePM_AccOff(this);             break;
    case E_PM_POWER_ON:                 mpNormalPowerState->handlePM_PowerOn(this);            break;
    case E_PM_POWER_OFF:                mpNormalPowerState->handlePM_PowerOff(this);           break;
    case E_PM_POWER_OFF_AWAKE:          mpNormalPowerState->handlePM_PowerOff_Awake(this);     break;
    case E_PM_SCREEN_ON:                mpNormalPowerState->handlePM_ScreenOn(this);           break;
    case E_PM_SCREEN_OFF:               mpNormalPowerState->handlePM_ScreenOff(this);          break;
    case E_PM_SCREEN_OFF_AWAKE:         mpNormalPowerState->handlePM_ScreenOff_Awake(this);    break;
    case E_PM_IGN_NOT_COME:             mpNormalPowerState->handlePM_Ign_Not_Come(this);       break;
    case E_PM_IGN_COME:                 mpNormalPowerState->handlePM_Ign_Come(this);           break;
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
    case E_APP_ABNORMAL_AWAKE_ON:
    case E_APP_ABNORMAL_AWAKE_OFF:
    {
        mpcabnormalmgr->recordAbnormalFlag(powerSignal);
        mpcabnormalmgr->updateCurAbnormalState();
        break;
    }
    case E_APP_SCREEN_AWAKE_ON:         mpNormalPowerState->handleAPP_SCREEN_AWAKE_ON(this);      break;
    case E_APP_SCREEN_AWAKE_OFF:        mpNormalPowerState->handleAPP_SCREEN_AWAKE_OFF(this);     break;
    case E_APP_POWER_AWAKE_ON:          mpNormalPowerState->handleAPP_POWER_AWAKE_ON(this);      break;
    case E_APP_POWER_AWAKE_OFF:         mpNormalPowerState->handleAPP_POWER_AWAKE_OFF(this);     break;
    case E_APP_SET_GOODBYE:             mpNormalPowerState->handleAPP_SET_GOODBYE_MODE(this);     break;
    case E_BOOT_WELCOME_PLAY_START:     mpNormalPowerState->handleBOOT_WELCOME_PLAY_START(this);  break;
    case E_BOOT_WELCOME_PLAY_OVER:      mpNormalPowerState->handleBOOT_WELCOME_PLAY_OVER(this);   break;
    case E_BOOT_FIRST_APP_READY:        mpNormalPowerState->handleBOOT_FIRST_APP_READY(this);     break;


    default: logInfo(PMC,"we don't need to handle this powerSignal");    break;
    }

    if(getPowerLevel(mpNormalPowerState->GetPowerStateFlag()) < getPowerLevel(mpcabnormalmgr->getCurAbnormalStateFlag()))
    {
        logDebug(PMC,"IVIPowerDevice::handlePowerSignal()-->mpAbnormalPowerState");
        changePowerStateTo(mpcabnormalmgr->getCurAbnormalState());
    }else
    {
        logDebug(PMC,"IVIPowerDevice::handlePowerSignal()-->mpNormalPowerState");
        changePowerStateTo(mpNormalPowerState);
    }

   // changeUsbStateByPowerSingal( powerSignal);

    logVerbose(PMC,"IVIPowerDevice::handlePowerSignal()-->OUT");
}
void IVIPowerDevice::changeUsbStateByPowerSingal(ama_PowerSignal_e powerSignal)
{
    // logVerbose(PMC,"IVIPowerDevice::changeUsbStateByPowerSingal()-->IN");
    // switch(powerSignal){
    //
    // case E_PM_SOC_TEMP_HIGH:
    // case E_SCR_TEMP_HIGH_90:
    // case E_PM_SOC_VOLTAGE_NORMAL:
    //     USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
    //     USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
    //     break;
    //
    // case E_PM_SOC_TEMP_NORMAL:
    // case E_PM_SOC_VOLTAGE_HIGH:
    // case E_PM_SOC_VOLTAGE_LOW:
    // case E_SCR_TEMP_HIGH_90_95:
    // case E_SCR_TEMP_HIGH_95_100:
    // case E_SCR_TEMP_HIGH_100:
    //     USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_OFF);
    //     USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHARGE_OFF);
    //     break;
    //
    // default: logInfo(PMC,"we don't need to handle this powerSignal");    break;
    // }
    // logVerbose(PMC,"IVIPowerDevice::changeUsbStateByPowerSingal()-->OUT");
}

void IVIPowerDevice::recordLastNormalState(ama_PowerState_e powerStateFlag)
{
    logVerbose(PMC,"IVIPowerDevice::recordLastNormalState()-->IN");
    switch(powerStateFlag){
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
    case E_PS_ACCON_NORMAL_POWEROFF:
    case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
        mLastNormalPowerState = powerStateFlag;
        break;
    default:
        logInfo(PMC,"don't record other powerState");
        break;
    }
    logVerbose(PMC,"IVIPowerDevice::recordLastNormalState()-->OUT");
}


// void IVIPowerDevice::changePowerStateByRecord(void)
// {
//     logVerbose(PMC,"IVIPowerDevice::changePowerStateByRecord()-->IN");

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
//         case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
//             if(AppAwakeMgr::GetInstance()->isScreenAwake()){
//                 changePowerStateTo(std::make_shared<AccOn_Normal_PowerOn_ScrOff_AwakeState>());
//             }else{
//                 changePowerStateTo(std::make_shared<AccOn_Normal_PowerOn_ScrOffState>());
//             }
//             break;
//         case E_PS_ACCON_NORMAL_POWEROFF:
//         case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
//             if(AppAwakeMgr::GetInstance()->isPowerAwake()){
//                 changePowerStateTo(std::make_shared<AccOn_Normal_PowerOff_AwakeState>());
//             }else{
//                 changePowerStateTo(std::make_shared<AccOn_Normal_PowerOffState>());
//             }
//             break;
//         default:
//             logError(PMC,"Wrong powerState");
//             break;
//         }
//     }

//     logVerbose(PMC,"IVIPowerDevice::changePowerStateByRecord()-->OUT");
// }

void IVIPowerDevice::refreshDeviceByPowerState(ama_PowerState_e powerStateFlag)
{
    logVerbose(PMC,"IVIPowerDevice::IVIPowerDevice()-->IN");

    recordPowerStateByRAM(powerStateFlag);

    switch(powerStateFlag){
    case E_PS_WELCOME_MODE:
    case E_PS_OPENING_MODE:
        setAllAppsVisible(false);
    #ifndef CODE_FOR_GAIA
        setLCDState(OPEN);
    #endif
        setTouchState(DISABLE);
        setTouchForbiden(DISABLE);
        setMediaMute(MUTE);
        setAudioState(ENABLE);
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(false);
        break;
    case E_PS_ACCOFF:
        setAllAppsVisible(false);
        setLCDState(CLOSE);
        setTouchState(DISABLE);
        setTouchForbiden(DISABLE);
        setMediaMute(MUTE);
        setAudioState(DISABLE);
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(false);
        break;
    case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_OFF);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHARGE_OFF);
        setAllAppsVisible(false);
        setLCDState(CLOSE);
        setTouchState(DISABLE);
        setTouchForbiden(DISABLE);
        setMediaMute(MUTE);
        setAudioState(DISABLE);
        setNormalBrightness();
        setLinKeyIsWork(false);
        break;
    case E_PS_ACCON_ABNORMAL_SOCTEMP:
    case E_PS_ACCON_NORMAL_POWEROFF:
        setAllAppsVisible(false);
        setLCDState(CLOSE);
        setTouchState(DISABLE);
        setTouchForbiden(DISABLE);
        setMediaMute(MUTE);
        setAudioState(DISABLE);
        setNormalBrightness();
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(false);
        break;
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
        setAllAppsVisible(true);
        setLCDState(OPEN);
        setAudioState(ENABLE);
        setMediaMute(UNMUTE);
        setTouchState(ENABLE);
        setTouchForbiden(DISABLE);
        setLowestBrightness();
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(true);
        break;
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
        setAllAppsVisible(false);
        setLCDState(CLOSE);
        setMediaMute(MUTE);
        setAudioState(DISABLE);
        setTouchState(ENABLE);
        setTouchForbiden(ENABLE);
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(false);
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
    case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
        setAllAppsVisible(true);
        setNormalBrightness();
        setLCDState(OPEN);
        setTouchState(ENABLE);
        setTouchForbiden(DISABLE);
        setAudioState(ENABLE);
        setMediaMute(UNMUTE);
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(true);
        break;
    case E_PS_ACCON_ABNORMAL_AWAKE:
        setAllAppsVisible(true);
        setNormalBrightness();
        setLCDState(OPEN);
        setTouchState(ENABLE);
        setTouchForbiden(DISABLE);
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
        //makeScreenShot();
        setAllAppsVisible(true);
        setNormalBrightness();
        setLCDState(CLOSE);
        setTouchState(DISABLE);
        setTouchForbiden(DISABLE);
        setAudioState(ENABLE);
        setMediaMute(UNMUTE);
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(true);
        break;
    case E_PS_GOODBYE_MODE:
        setAllAppsVisible(true);
        setLowestBrightness();
        setLCDState(OPEN);
        setTouchState(DISABLE);
        setTouchForbiden(DISABLE);
        setMediaMute(MUTE);
        setAudioState(DISABLE);
        USBManager::GetInstance()->UsbOperate(E_USB_HOST_CHARGE_ON);
        USBManager::GetInstance()->UsbOperate(E_USB_OTG_CHAGRE_ON);
        setLinKeyIsWork(false);
        break;
    default:
        logError(PMC,"powerStateFlag is not defined");
        break;
    }
    logVerbose(PMC,"IVIPowerDevice::IVIPowerDevice()-->OUT");
}

bool IVIPowerDevice::isPowerOn(void)
{
    logVerbose(PMC,"IVIPowerDevice::isPowerOn()-->IN");

    ama_PowerState_e powerStateFlag = mpFinalPowerState->GetPowerStateFlag();
    if(powerStateFlag&POWER_POWER_FLAG){
        return true;
    }else{
        return false;
    }

    logVerbose(PMC,"IVIPowerDevice::isPowerOn()-->OUT");
}

void IVIPowerDevice::recordPowerStateByRAM(ama_PowerState_e powerStateFlag)
{
    logVerbose(PMC,"IVIPowerDevice::recordPowerStateByRAM()-->IN");

    switch(powerStateFlag){
    case E_PS_WELCOME_MODE:
    case E_PS_OPENING_MODE:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
    case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
    case E_PS_GOODBYE_MODE:
        logInfo(PMC,"we don't need record those powerState!");
        break;
    case E_PS_ACCOFF:
    case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
    case E_PS_ACCON_ABNORMAL_SOCTEMP:
    case E_PS_ACCON_NORMAL_POWEROFF:
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
    {
        // std::stringstream ss;
        // std::string str;
        // ss<<powerStateFlag;
        // ss>>str;

        std::ofstream ofs;

        ofs.open(mPowerStateFileByRAM.c_str());
        if(ofs.is_open()){
            ofs << powerStateFlag;
            ofs.close();
        }else{
            logError(PMC,"open mPowerStateFileByRAM failed");
        }
    }
        break;

    default:
        logError(PMC,"powerStateFlag is not defined");
        break;
    }

    logVerbose(PMC,"IVIPowerDevice::recordPowerStateByRAM()-->OUT");
}

ama_PowerState_e IVIPowerDevice::getPowerStateFromRAM()
{
    logVerbose(PMC,"IVIPowerDevice::getPowerStateFromRAM()-->IN");

    if(0 == (access(mPowerStateFileByRAM.c_str(),F_OK))){
        std::ifstream ifs;
        ifs.open(mPowerStateFileByRAM.c_str());
        if(ifs.is_open()){
            std::ostringstream temp;
            temp<<ifs.rdbuf();
            int powerStaFlag = atol(temp.str().c_str());
            return static_cast<ama_PowerState_e>(powerStaFlag);
        }else{
            logError(PMC,"open mPowerStateFileByRAM failed");
            remove(mPowerStateFileByRAM.c_str());
            return E_PS_ACCON_NORMAL_POWERON_SCRON;
        }
    }else{
        logError(PMC,"mPowerStateFileByRAM is not exist!");
        return E_PS_ACCON_NORMAL_POWERON_SCRON;
    }

    logVerbose(PMC,"IVIPowerDevice::getPowerStateFromRAM()-->OUT");
}

void IVIPowerDevice::restorePowerStateByRAM()
{
    logVerbose(PMC,"IVIPowerDevice::restorePowerStateByRAM()-->IN");

    ama_PowerState_e powerStaFlag = getPowerStateFromRAM();
    switch(powerStaFlag){
    case E_PS_WELCOME_MODE:
    case E_PS_OPENING_MODE:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
    case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
    case E_PS_GOODBYE_MODE:
        logError(PMC,"we don't need restore those powerState!");
        break;
    case E_PS_ACCOFF:
        mpNormalPowerState = std::make_shared<AccOffState>();
        break;
    case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
        mpNormalPowerState = std::make_shared<AccOn_Abnormal_SocVoltageState>();
        break;
    case E_PS_ACCON_ABNORMAL_SOCTEMP:
        mpNormalPowerState = std::make_shared<AccOn_Abnormal_SocTempState>();
        break;
    case E_PS_ACCON_NORMAL_POWEROFF:
        mpNormalPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
        break;
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
        mpNormalPowerState = std::make_shared<AccOn_Abnormal_ScrTempHighState>();
        break;
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
        mpNormalPowerState = std::make_shared<AccOn_Abnormal_ScrTempHigherState>();
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
        mpNormalPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
        mpNormalPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOffState>();
        break;
    default:
        logError(PMC,"powerStateFlag is not defined");
        break;
    }

    logVerbose(PMC,"IVIPowerDevice::restorePowerStateByRAM()-->OUT");
}

bool IVIPowerDevice::isPowerStateRecordByRAM()
{
    logVerbose(PMC,"IVIPowerDevice::isPowerStateRecordByRAM()-->IN");

    if(0 == (access(mPowerStateFileByRAM.c_str(),F_OK))){
        return true;
    }else{
        return false;
    }

    logVerbose(PMC,"IVIPowerDevice::isPowerStateRecordByRAM()-->OUT");
}

void IVIPowerDevice::setLinKeyIsWork(bool isWork)
{
    logVerbose(PMC,"IVIPowerDevice::setLinKeyIsWork()-->IN isWork:",isWork);

    rp.setLinKeyWorkSta(isWork);

    logVerbose(PMC,"IVIPowerDevice::setLinKeyIsWork()-->OUT");
}
