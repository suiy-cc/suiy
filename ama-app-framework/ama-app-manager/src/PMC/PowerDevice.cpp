/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "PowerDevice.h"
#include "PMCtrl.h"
#include "AMGRLogContext.h"
#include "ama_audioTypes.h"
#include "HMICTask.h"
#include "TaskDispatcher.h"
#include "ama_enum.h"
#include "PowerState.h"
#include "LCDManager.h"
#include "Capi.h"
#include "AppAwakeMgr.h"
#include "AudioTask.h"
#include "LineTool.h"
#include "CAbnormalMgr.h"
#include "BootListener.h"

PowerDevice::PowerDevice(E_SEAT seatID)
:mSeatID(seatID)
{
    logVerbose(PMC,"PowerDevice::PowerDevice()-->IN");

    if(E_SEAT_IVI == seatID){
        mIsIVIDeviceNotRSE = true;
    }else{
        mIsIVIDeviceNotRSE = false;
    }

    mpAbnormalPowerState = std::make_shared<IAccOn_AbormalState>();
    mpNormalPowerState = std::make_shared<PowerInitState>();
    mpFinalPowerState = std::make_shared<PowerInitState>();
    mpcabnormalmgr = std::make_shared<CAbnormalMgr>();

    logVerbose(PMC,"PowerDevice::PowerDevice()-->OUT");
}

PowerDevice::~PowerDevice()
{
    logVerbose(PMC,"PowerDevice::~PowerDevice()-->IN");
    logVerbose(PMC,"PowerDevice::~PowerDevice()-->OUT");
}

bool PowerDevice::isIVIDevice(void)
{
    logVerbose(PMC,"PowerDevice::isIVIDevice()-->IN");

    return mIsIVIDeviceNotRSE;

    logVerbose(PMC,"PowerDevice::isIVIDevice()-->OUT");
}

// void PowerDevice::recordAbnormalFlag(ama_PowerSignal_e powerSignal)
// {
//     logVerbose(PMC,"PowerDevice::recordAbnormalFlag()-->IN powerSignal:",powerSignal);

//     switch(powerSignal){
//     case E_PM_SOC_TEMP_HIGH:            mpcabnormalmgr->setIsSocTempNormal(false);        break;
//     case E_PM_SOC_TEMP_NORMAL:          mpcabnormalmgr->setIsSocTempNormal(true);         break;
//     case E_PM_SOC_VOLTAGE_HIGH:         mpcabnormalmgr->setIsSocVoltageNormal(false);   break;
//     case E_PM_SOC_VOLTAGE_NORMAL:       mpcabnormalmgr->setIsSocVoltageNormal(true);    break;
//     case E_PM_SOC_VOLTAGE_LOW:          mpcabnormalmgr->setIsSocVoltageNormal(false);   break;
//     case E_SCR_TEMP_HIGH_90:
//     case E_SCR_TEMP_HIGH_90_95:
//     case E_SCR_TEMP_HIGH_95_100:
//     case E_SCR_TEMP_HIGH_100:           mpcabnormalmgr->setLastScreenTemp(powerSignal); break;
//     default: logInfo(PMC,"powerSignal is not about abnormal flag"); break;
//     }

//     logVerbose(PMC,"PowerDevice::recordAbnormalFlag()-->OUT");
// }

void PowerDevice::updateFinalPowerState(shared_ptr<IPowerState> pPowerState)
{
    logVerbose(PMC,"PowerDevice::updateFinalPowerState()-->IN  ",ToHEX(pPowerState->GetPowerStateFlag()));

    switch(pPowerState->GetPowerStateFlag())
    {
        case E_PS_ACCOFF:
            mpFinalPowerState = std::make_shared<AccOffState>();
        break;
        case E_PS_WELCOME_MODE:
            mpFinalPowerState = std::make_shared<WelcomeState>();
        break;
        case E_PS_OPENING_MODE:
            mpFinalPowerState = std::make_shared<OpeningState>();
        break;
        case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
            mpFinalPowerState = std::make_shared<AccOn_Abnormal_SocVoltageState>();
        break;
        case E_PS_ACCON_ABNORMAL_SOCTEMP:
            mpFinalPowerState = std::make_shared<AccOn_Abnormal_SocTempState>();
        break;
        case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
            mpFinalPowerState = std::make_shared<AccOn_Abnormal_ScrTempHighState>();
        break;
        case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
            mpFinalPowerState = std::make_shared<AccOn_Abnormal_ScrTempHigherState>();
        break;
        case E_PS_GOODBYE_MODE:
            mpFinalPowerState = std::make_shared<AccOn_Goodbye_State>();
        break;
        case E_PS_ACCON_NORMAL_POWERON_SCRON:
            mpFinalPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOnState>();
        break;
        case E_PS_ACCON_NORMAL_POWERON_SCROFF:
            mpFinalPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOffState>();
        break;
        case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
            mpFinalPowerState = std::make_shared<AccOn_Normal_PowerOn_ScrOff_AwakeState>();
        break;
        case E_PS_ACCON_NORMAL_POWEROFF:
            mpFinalPowerState = std::make_shared<AccOn_Normal_PowerOffState>();
        break;
        case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
            mpFinalPowerState = std::make_shared<AccOn_Normal_PowerOff_AwakeState>();
        break;
        case E_PS_ACCON_ABNORMAL_AWAKE:
            mpFinalPowerState = std::make_shared<AccOn_Abnormal_AwakeState>();
        break;
    }

    logVerbose(PMC,"PowerDevice::updateFinalPowerState()-->IN");
}

void PowerDevice::changePowerStateTo(shared_ptr<IPowerState> pPowerState)
{
    logVerbose(PMC,"PowerDevice::changePowerStateTo()-->IN");
    if(mpFinalPowerState->GetPowerStateFlag() == pPowerState->GetPowerStateFlag())
    {
        logVerbose(PMC,"PowerDevice::changePowerStateTo()-->flag is same ,OUT!!!");
        return;
    }

    //get change of power State first
    ama_changeOfPowerSta_e onlyPowerChange = getPowerChange(POWER_POWER_FLAG,mpFinalPowerState->GetPowerStateFlag(),pPowerState->GetPowerStateFlag());
    tellChangeOfPowerStateToAC(onlyPowerChange);

    if(mpFinalPowerState->GetPowerStateFlag() == E_PS_OPENING_MODE)
    {
        BootListener::GetInstance()->stopOpeningMode();
    }

    //update powerState
    updateFinalPowerState(pPowerState);
    logInfo(PMC,"==>>change power state to GetPowerStateFlagStr:",mpFinalPowerState->GetPowerStateFlagStr());
    recordLastNormalState(mpFinalPowerState->GetPowerStateFlag());
    refreshDeviceByPowerState(mpFinalPowerState->GetPowerStateFlag());
    tellAppNewPowerState(mpFinalPowerState->GetPowerStateFlag());
    if(E_SEAT_IVI==mSeatID){
        LineTool::GetInstance()->handlePowerStaChanged(mpFinalPowerState->GetPowerStateFlag());
    }

    logVerbose(PMC,"PowerDevice::changePowerStateTo()-->OUT");
}

void PowerDevice::changeAbnormalPowerStateTo(shared_ptr<IPowerState> pPowerState)
{
    mpAbnormalPowerState = pPowerState;
}

void PowerDevice::changeNormalPowerStateTo(shared_ptr<IPowerState> pPowerState)
{
    mpNormalPowerState = pPowerState;
}

ama_changeOfPowerSta_e PowerDevice::getPowerChange(ama_PowerState_e oldState,ama_PowerState_e newState)
{
    logVerbose(PMC," PowerDevice::getPowerChange()-->IN oldState:",ToHEX(oldState), "newState:",ToHEX(newState));

    if((oldState&POWER_ACC_FLAG)^(newState&POWER_ACC_FLAG)){
        if(newState&POWER_ACC_FLAG){
            return E_ACC_CHANGE_TO_ON;
        }else{
            return E_ACC_CHANGE_TO_OFF;
        }
    }else if((oldState&POWER_NORMAL_FLAG)^(newState&POWER_NORMAL_FLAG)){
        if(newState&POWER_NORMAL_FLAG){
            return E_STATE_CHANGE_TO_NORMAL;
        }else{
            return E_STATE_CHANGE_TO_ABNORMAL;
        }
    }else if((oldState&POWER_POWER_FLAG)^(newState&POWER_POWER_FLAG)){
        if(newState&POWER_POWER_FLAG){
            return E_POWER_CHANGE_TO_ON;
        }else{
            return E_POWER_CHANGE_TO_OFF;
        }
    }else if((oldState&POWER_SCR_FLAG)^(newState&POWER_SCR_FLAG)){
        if(newState&POWER_SCR_FLAG){
            return E_SCR_CHANGE_TO_ON;
        }else{
            return E_SCR_CHANGE_TO_OFF;
        }
    }else{
        return E_PS_NO_CHANGE;
    }

    logVerbose(PMC," PowerDevice::getPowerChange()-->OUT");
}

int PowerDevice::getPowerLevel(ama_PowerState_e state)
{
    logVerbose(PMC," PowerDevice::getPowerLevel()-->IN ama_PowerState_e:",ToHEX(state));

    int level = POWER_LEVEL_FLAG & state;
    logVerbose(PMC," PowerDevice::getPowerLevel()-->OUT  ",ToHEX(level));
    return level;
}

ama_changeOfPowerSta_e PowerDevice::getPowerChange(int powerTypeFlag,ama_PowerState_e oldState,ama_PowerState_e newState)
{
    logVerbose(PMC," PowerDevice::getPowerChange()-->IN oldState:",ToHEX(oldState), "newState:",ToHEX(newState));

    switch(powerTypeFlag){
    case POWER_ACC_FLAG:
        if((oldState&POWER_ACC_FLAG)^(newState&POWER_ACC_FLAG)){
            if(newState&POWER_ACC_FLAG){
                return E_ACC_CHANGE_TO_ON;
            }else{
                return E_ACC_CHANGE_TO_OFF;
            }
        }else{
            return E_PS_NO_CHANGE;
        }

    case POWER_NORMAL_FLAG:
        if((oldState&POWER_NORMAL_FLAG)^(newState&POWER_NORMAL_FLAG)){
            if(newState&POWER_NORMAL_FLAG){
                return E_STATE_CHANGE_TO_NORMAL;
            }else{
                return E_STATE_CHANGE_TO_ABNORMAL;
            }
        }else{
            return E_PS_NO_CHANGE;
        }

    case POWER_POWER_FLAG:
        if((oldState&POWER_POWER_FLAG)^(newState&POWER_POWER_FLAG)){
            if(newState&POWER_POWER_FLAG){
                return E_POWER_CHANGE_TO_ON;
            }else{
                return E_POWER_CHANGE_TO_OFF;
            }
        }else{
            return E_PS_NO_CHANGE;
        }

    case POWER_SCR_FLAG:
        if((oldState&POWER_SCR_FLAG)^(newState&POWER_SCR_FLAG)){
            if(newState&POWER_SCR_FLAG){
                return E_SCR_CHANGE_TO_ON;
            }else{
                return E_SCR_CHANGE_TO_OFF;
            }
        }else{
                return E_PS_NO_CHANGE;
        }
    default:
        break;
    }

    logVerbose(PMC," PowerDevice::getPowerChange()-->OUT");
}

void PowerDevice::tellAppNewPowerState(ama_PowerState_e powerState)
{
    logVerbose(PMC,"PowerDevice::tellAppNewPowerState()-->IN mSeatID:",ToHEX(mSeatID),"powerState:",ToHEX(powerState));

    PowerStateChangeNotifyEx(mSeatID, powerState);
    PowerManagerController::GetInstance()->updatePowerStateByDevice(mSeatID, powerState);

    logVerbose(PMC,"PowerDevice::tellAppNewPowerState()-->OUT");
}

void PowerDevice::tellChangeOfPowerStateToAC(ama_changeOfPowerSta_e changeOfPowerSta)
{
    logVerbose(PMC,"PowerDevice::tellChangeOfPowerStateToAC()-->IN mSeatID:",ToHEX(mSeatID),"changeOfPowerSta:",ToHEX(changeOfPowerSta));

    switch(changeOfPowerSta){
    case E_POWER_CHANGE_TO_OFF:
        logDebug(PMC,"E_POWER_CHANGE_TO_OFF");
        notifyPowerStaChangedToAC(mSeatID,false);
        break;
    case E_POWER_CHANGE_TO_ON:
        logDebug(PMC,"E_POWER_CHANGE_TO_ON");
        notifyPowerStaChangedToAC(mSeatID,true);
        break;
    default:
        break;
    }

    logVerbose(PMC,"PowerDevice::tellChangeOfPowerStateToAC()-->OUT");
}

void PowerDevice::makeScreenShot()
{
    logVerbose(PMC,"PowerDevice::makeScreenShot()--> IN ");
    // if engineer trigger screen shot is true get four screenshots first
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetType(HMIC_REQUEST_ALL_SCREEN_SHOT);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    // if engineer trigger screen shot is true get four screenshots first
    logVerbose(PMC,"PowerDevice::makeScreenShot()-->OUT ");
}

void PowerDevice::setNormalBrightness()
{
    logVerbose(PMC,"PowerDevice::setNormalBrightness()-->IN ");
    LCDManager::GetInstance()->RecoverLastDegree(mSeatID);
    logVerbose(PMC,"PowerDevice::setNormalBrightness()-->OUT");
}

void PowerDevice::setLowestBrightness()
{
    logVerbose(PMC,"PowerDevice::setLowerestDegree()-->IN ");
    LCDManager::GetInstance()->SetLowestDegree(mSeatID);
    logVerbose(PMC,"PowerDevice::setLowerestDegree()-->OUT");
}
void PowerDevice::setLCDState(bool isOpen)
{
    logVerbose(PMC,"PowerDevice::setLCDState()-->IN isOpen:",isOpen);
    LCDManager::GetInstance()->SetLCDPower((E_SEAT)mSeatID,isOpen);
    logVerbose(PMC,"PowerDevice::setLCDState()-->OUT");
}

void PowerDevice::setTouchState(bool isEnable)
{
    logVerbose(PMC,"PowerDevice::setTouchState()-->IN isEnable:",isEnable);

    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetAMPID(AMPID_AMGR(mSeatID, E_APPID_GR));
    if(isEnable){
        hmicTask->SetUserData(false);
    }else{
        hmicTask->SetUserData(true);
    }
    hmicTask->SetType(HMIC_CMD_ENABLE_GESTURE_RECOGNIZER);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    logVerbose(PMC,"PowerDevice::setTouchState()-->OUT");
}
void PowerDevice::setTouchForbiden(bool isForbiden)
{
    logVerbose(PMC,"PowerDevice::setTouchForbiden()-->IN isForbiden:",isForbiden);
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetAMPID(AMPID_AMGR(mSeatID, E_APPID_TOUCH_FORBIDDER));
    if(isForbiden){
        hmicTask->SetUserData(true);
    }else{
        hmicTask->SetUserData(false);
    }
    hmicTask->SetType(HMIC_CMD_ENABLE_TOUCH_SCREEN);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    logVerbose(PMC,"PowerDevice::setTouchForbiden()-->OUT");
}
// bool PowerDevice::getIsSocTempNormal() const
// {
//     return isSocTempNormal;
// }
// void PowerDevice::setIsSocTempNormal(bool value)
// {
//     isSocTempNormal = value;
// }
// bool PowerDevice::getIsSocVoltageNormal() const
// {
//     return isSocVoltageNormal;
// }
// void PowerDevice::setIsSocVoltageNormal(bool value)
// {
//     isSocVoltageNormal = value;
// }
void PowerDevice::setAudioState(bool isEnable)
{
    logVerbose(PMC,"PowerDevice::setAudioState()-->IN isEnable:",isEnable);

    if(isEnable){
        sendSetDeviceMuteTaskToAC(mSeatID,false);
    }else{
        sendSetDeviceMuteTaskToAC(mSeatID,true);
    }

    logVerbose(PMC,"PowerDevice::setAudioState()-->OUT");
}

void PowerDevice::setAllAppsVisible(bool isVisible)
{
    logVerbose(PMC,"PowerDevice::",__FUNCTION__,"-->IN isVisible:",isVisible);

    //create a task to HMIC
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetAMPID(AMPID_AMGR(mSeatID, E_APPID_MASK_APPS));
    if(isVisible){
        hmicTask->SetUserData(false);
    }else{
        hmicTask->SetUserData(true);
    }
    hmicTask->SetType(HMIC_CMD_SET_MASK_SUR_VISIBLE);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    logVerbose(PMC,"PowerDevice::",__FUNCTION__,"-->OUT");
}

void PowerDevice::setMediaMute(bool isMediaMute)
{
    logVerbose(PMC,"PowerDevice::",__FUNCTION__,"-->IN isMediaMute:",isMediaMute);

    setMediaMuteByPowerStaToAC(isMediaMute);

    logVerbose(PMC,"PowerDevice::",__FUNCTION__,"-->OUT");
}
