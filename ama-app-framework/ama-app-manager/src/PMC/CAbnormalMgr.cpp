/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "CAbnormalMgr.h"
#include "AMGRLogContext.h"

using namespace std;

const int SOC_VOLTAGE_HIGH = 1; 
const int SOC_VOLTAGE_LOW = 2; 
const int SOC_VOLTAGE_NORMAL = 0; 

CAbnormalMgr::CAbnormalMgr()
:isSocTempNormal(true)
,mSocVoltageFlag(SOC_VOLTAGE_NORMAL)
,mLastScreenTemp(E_SCR_TEMP_HIGH_90)
,isAbnormalAwake(false)
{
    logVerbose(PMC,"CAbnormalMgr::",__FUNCTION__,"-->>IN");

    mpAbnormalPowerState = std::make_shared<IAccOn_AbormalState>();

    logVerbose(PMC,"CAbnormalMgr::",__FUNCTION__,"-->>OUT");
}

CAbnormalMgr::~CAbnormalMgr()
{

}

ama_PowerState_e CAbnormalMgr::getCurAbnormalStateFlag()
{
    return mpAbnormalPowerState->GetPowerStateFlag();
}

shared_ptr<IPowerState> CAbnormalMgr::getCurAbnormalState()
{
    switch(getCurAbnormalStateFlag())
    {
        case E_PS_ACCON_ABNORMAL_AWAKE:
            return std::make_shared<AccOn_Abnormal_AwakeState>();
        break;
        case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
            return std::make_shared<AccOn_Abnormal_SocVoltageState>();
        break;
        case E_PS_ACCON_ABNORMAL_SOCTEMP:
            return std::make_shared<AccOn_Abnormal_SocTempState>();
        break;
        case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
            return std::make_shared<AccOn_Abnormal_ScrTempHighState>();
        break;
        case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
            return std::make_shared<AccOn_Abnormal_ScrTempHigherState>();
        break;
        default:
            return std::make_shared<IAccOn_AbormalState>();
        break;
    }
}

void CAbnormalMgr::changeAbnormalPowerStateTo(shared_ptr<IPowerState> pPowerState)
{
    mpAbnormalPowerState = pPowerState;
}


void CAbnormalMgr::updateCurAbnormalState()
{
    auto abnormalflg = true; 

    if(SOC_VOLTAGE_HIGH == getSocVoltageFlag())
    {
        mpAbnormalPowerState->handlePM_Soc_Voltage_High(this);
    }
    else if(SOC_VOLTAGE_LOW == getSocVoltageFlag())
    {
        mpAbnormalPowerState->handlePM_Soc_Voltage_Low(this);
    }
    else if(!getIsSocTempNormal())
    {
        mpAbnormalPowerState->handlePM_Soc_Temp_High(this);
    }
    else if(E_SCR_TEMP_HIGH_100 == getLastScreenTemp())
    {
        mpAbnormalPowerState->handleSCR_TEMP_HIGH_100(this);
    }
    else if(E_SCR_TEMP_HIGH_95_100 == getLastScreenTemp())
    {
        mpAbnormalPowerState->handleSCR_TEMP_HIGH_95_100(this);
    }
    else if(E_SCR_TEMP_HIGH_90_95 == getLastScreenTemp())
    {
        mpAbnormalPowerState->handleSCR_TEMP_HIGH_90_95(this);
    }
    else
    {
        abnormalflg = false;
        mpAbnormalPowerState->handleABN_To_Normal(this);
      //  mpAbnormalPowerState = std::make_shared<IAccOn_AbormalState>();
    }

    if(abnormalflg == true && isAbnormalAwake == true)
    {
        mpAbnormalPowerState->handleAPP_ABNORMAL_AWAKE_ON(this);
    }
}

void CAbnormalMgr::recordAbnormalFlag(ama_PowerSignal_e powerSignal)
{
    logVerbose(PMC,"CAbnormalMgr::recordAbnormalFlag()-->IN powerSignal:",powerSignal);
    switch(powerSignal)
    {
        case E_PM_SOC_TEMP_HIGH:            setIsSocTempNormal(false);      break;
        case E_PM_SOC_TEMP_NORMAL:          setIsSocTempNormal(true);       break;
        case E_PM_SOC_VOLTAGE_HIGH:         setSocVoltageFlag(SOC_VOLTAGE_HIGH);           break;
        case E_PM_SOC_VOLTAGE_NORMAL:       setSocVoltageFlag(SOC_VOLTAGE_NORMAL);           break;
        case E_PM_SOC_VOLTAGE_LOW:          setSocVoltageFlag(SOC_VOLTAGE_LOW);           break;
        case E_SCR_TEMP_HIGH_90:
        case E_SCR_TEMP_HIGH_90_95:
        case E_SCR_TEMP_HIGH_95_100:
        case E_SCR_TEMP_HIGH_100:           setLastScreenTemp(powerSignal); break;
        case E_APP_ABNORMAL_AWAKE_ON:       setIsAbnormalAwake(true);        break;
        case E_APP_ABNORMAL_AWAKE_OFF:      
            setIsAbnormalAwake(false);       
            mpAbnormalPowerState->handleAPP_ABNORMAL_AWAKE_OFF(this);
            break;
        default: logInfo(PMC,"powerSignal is not about abnormal flag");     return;
    }
    logVerbose(PMC,"CAbnormalMgr::recordAbnormalFlag()-->OUT");
}

void CAbnormalMgr::setLastScreenTemp(int temp)
{
    logVerbose(PMC,"CAbnormalMgr::setLastScreenTemp()-->IN ");
    mLastScreenTemp = temp;
    logVerbose(PMC,"CAbnormalMgr::setLastScreenTemp()-->OUT");
}

int CAbnormalMgr::getLastScreenTemp() const
{
    logVerbose(PMC,"CAbnormalMgr::getLastScreenTemp()-->IN ");
    return mLastScreenTemp;
    logVerbose(PMC,"CAbnormalMgr::getLastScreenTemp()-->OUT");
}

void CAbnormalMgr::setIsSocTempNormal(bool value)
{
    logVerbose(PMC,"CAbnormalMgr::setSocTempNormal()-->IN ");
    isSocTempNormal = value;
    logVerbose(PMC,"CAbnormalMgr::setSocTempNormal()-->OUT");
}

bool CAbnormalMgr::getIsSocTempNormal() const
{
    logVerbose(PMC,"CAbnormalMgr::getLastScreenTemp()-->IN ");
    return isSocTempNormal;
    logVerbose(PMC,"CAbnormalMgr::getLastScreenTemp()-->OUT");
}

void CAbnormalMgr::setIsAbnormalAwake(bool value)
{
    logVerbose(PMC,"CAbnormalMgr::setIsAbnormalAwake()-->IN ");
    isAbnormalAwake = value;
    logVerbose(PMC,"CAbnormalMgr::setIsAbnormalAwake()-->OUT");
}

bool CAbnormalMgr::getIsAbnormalAwake() const
{
    logVerbose(PMC,"CAbnormalMgr::getIsAbnormalAwake()-->IN ");
    return isAbnormalAwake;
    logVerbose(PMC,"CAbnormalMgr::getIsAbnormalAwake()-->OUT");  
}

void CAbnormalMgr::setSocVoltageFlag(int value)
{
    logVerbose(PMC,"CAbnormalMgr::setSocVoltageNormal()-->IN ");
    mSocVoltageFlag = value;
    logVerbose(PMC,"CAbnormalMgr::setSocVoltageNormal()-->OUT");
}

int CAbnormalMgr::getSocVoltageFlag() const
{ 
    logVerbose(PMC,"CAbnormalMgr::getSocVoltageNormal()-->IN ");
    return mSocVoltageFlag;
    logVerbose(PMC,"CAbnormalMgr::getSocVoltageNormal()-->OUT");
}