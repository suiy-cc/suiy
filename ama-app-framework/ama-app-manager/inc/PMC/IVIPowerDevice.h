/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef IVIPOWERDEVICE_H_
#define IVIPOWERDEVICE_H_

#include "ama_enum.h"
#include "PowerDevice.h"

#include <string>

class IVIPowerDevice : public PowerDevice
{
public:
    static IVIPowerDevice* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static IVIPowerDevice instance;
        return &instance;
    }
    virtual ~IVIPowerDevice();

    void handlePowerSignal(ama_PowerSignal_e powerSignal);
    bool isPowerOn(void);
protected:
    void changeUsbStateByPowerSingal(ama_PowerSignal_e powerSignal);
    void recordLastNormalState(ama_PowerState_e powerStateFlag);
    // void changePowerStateByRecord(void);
    void refreshDeviceByPowerState(ama_PowerState_e powerStateFlag);

    void recordPowerStateByRAM(ama_PowerState_e powerStateFlag);
    ama_PowerState_e getPowerStateFromRAM();
    bool isPowerStateRecordByRAM();
    void restorePowerStateByRAM();
private:
    IVIPowerDevice();
    void setLinKeyIsWork(bool isWork);

    bool usbCurrentNormal;
    void checkUSBState();
    char * usbfaultPath={0};
    std::string mPowerStateFileByRAM;
};

#endif//IVIPOWERDEVICE_H_
