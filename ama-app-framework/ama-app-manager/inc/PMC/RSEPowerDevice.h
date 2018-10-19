/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef RSEPOWERDEVICE_H_
#define RSEPOWERDEVICE_H_

#include "ama_enum.h"
#include "PowerDevice.h"

class RSEPowerDevice : public PowerDevice
{
public:
    RSEPowerDevice(E_SEAT seatID);
    virtual ~RSEPowerDevice();

    void handlePowerSignal(ama_PowerSignal_e powerSignal);
protected:
    void recordLastNormalState(ama_PowerState_e powerStateFlag);
  //  void changePowerStateByRecord(void);
    void refreshDeviceByPowerState(ama_PowerState_e powerStateFlag);

private:
     void setTimerState(bool isEnable);
};

#endif//RSEPOWERDEVICE_H_
