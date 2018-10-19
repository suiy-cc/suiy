/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APPPOWER_H
#define APPPOWER_H

#include "ama_powerTypes.h"

#include <stdint.h>

//please make sure call appSDKInit() first!!!
void powerSDKInit(void);

bool checkIsPowerOn(uint8_t seatID);
bool checkIsACCOn(uint8_t seatID);
ama_PowerState_e getPowerState(uint8_t seatID);

typedef void (*powerStateChangedCB)(uint8_t seatID,ama_PowerState_e powerState);
void registerPowerStateChangedCB(powerStateChangedCB f);
void unregisterPowerStateChangedCB(powerStateChangedCB f);

typedef void (*changeOfPowerStaChangedCB)(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta);
void registerChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f);
void unregisterChangeOfPowerStaChangedCB(changeOfPowerStaChangedCB f);

typedef void (*changeOfPowerStaChangedCBWithIndex)(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta,uint32_t changeIndex);
void registerChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f);
void unregisterChangeOfPowerStaChangedCBWithIndex(changeOfPowerStaChangedCBWithIndex f);

void requestPowerAwake(int32_t ampid,bool isPowerAwake);
void requestScreenAwake(int32_t ampid,bool isScreenAwake);
void requestAbnormalAwake(int32_t ampid,bool isAbnormalAwake);

void requestPowerGoodbye(void);

//old interface of power
/// @name power state change
/// @{
bool isPowerOn(uint32_t seat);
bool isACCOn(uint32_t seat);

typedef void (*PowerStateChangeNotifyCB)(bool);
void ListenToPowerStateChangeNotify(PowerStateChangeNotifyCB f);

typedef void (*ACCStateChangeNotifyCB)(bool);
void ListenToACCStateChangeNotify(ACCStateChangeNotifyCB f);

typedef void (*PowerStateChangeExNotifyCB)(ama_PowerState_e, unsigned int);
void ListenToPowerStateChangeExNotify(PowerStateChangeExNotifyCB f);

typedef void (*changeOfPowerStateCB)(ama_changeOfPowerSta_e, unsigned int);
void ListenToChangeOfPowerStateNotify(changeOfPowerStateCB f);

/// @}

#endif//APPPOWER_H
