/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "AppPower.h"
#include "PowerSDK.h"

void powerSDKInit(void)
{
    PowerSDK::GetInstance()->initAsync();
}

bool checkIsPowerOn(uint8_t seatID)
{
    return PowerSDK::GetInstance()->isPowerOn(seatID);
}

bool checkIsACCOn(uint8_t seatID)
{
    return PowerSDK::GetInstance()->isACCOn(seatID);
}

ama_PowerState_e getPowerState(uint8_t seatID)
{
    return PowerSDK::GetInstance()->getPowerState(seatID);
}

void registerPowerStateChangedCB(powerStateChangedCB f)
{
    PowerSDK::GetInstance()->setPowerStateChangedCB(f);
}

void unregisterPowerStateChangedCB(powerStateChangedCB f)
{
    PowerSDK::GetInstance()->removePowerStateChangedCB(f);
}

void registerChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f)
{
    PowerSDK::GetInstance()->setChangeOfPowerStaChangedCB(f);
}

void unregisterChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f)
{
    PowerSDK::GetInstance()->removeChangeOfPowerStaChangedCB(f);
}

void registerChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f)
{
    PowerSDK::GetInstance()->setChangeOfPowerStaChangedCBWithIndex(f);
}

void unregisterChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f)
{
    PowerSDK::GetInstance()->removeChangeOfPowerStaChangedCBWithIndex(f);
}

void requestPowerAwake(int32_t ampid,bool isPowerAwake)
{
    PowerSDK::GetInstance()->requestPowerAwake(ampid, isPowerAwake);
}

void requestScreenAwake(int32_t ampid,bool isScreenAwake)
{
    PowerSDK::GetInstance()->requestScreenAwake(ampid,isScreenAwake);
}

void requestAbnormalAwake(int32_t ampid,bool isAbnormalAwake)
{
    PowerSDK::GetInstance()->requestAbnormalAwake(ampid,isAbnormalAwake);
}

void requestPowerGoodbye(void)
{
    PowerSDK::GetInstance()->requestPowerGoodbye();
}

/***************************************old interface *************************/
bool isPowerOn(uint32_t seat)
{
    return PowerSDK::GetInstance()->isPowerOn(seat);
}
bool isACCOn(uint32_t seat)
{
    return PowerSDK::GetInstance()->isACCOn(seat);
}

void ListenToPowerStateChangeExNotify(PowerStateChangeExNotifyCB f)
{
    PowerSDK::GetInstance()->SetPowerStateChangeExNotifyCB(f);
}

void ListenToChangeOfPowerStateNotify(changeOfPowerStateCB f)
{
    PowerSDK::GetInstance()->SetChangeOfPowerStateCB(f);
}

void ListenToPowerStateChangeNotify(PowerStateChangeNotifyCB f)
{
    PowerSDK::GetInstance()->SetPowerStateChangeNotifyCB(f);
}

void ListenToACCStateChangeNotify(ACCStateChangeNotifyCB f)
{
    PowerSDK::GetInstance()->SetACCStateChangeNotifyCB(f);
}
