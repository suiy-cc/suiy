/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PMCtrl.h
/// @brief contains class PowerMangaerController
///
/// Created by wangqi on 2017/4/6.
/// this file contains the definination of class PowerMangaerController
///

#ifndef PMCTRL_H_
#define PMCTRL_H_

#include <mutex>

#include "AMGRLogContext.h"
#include "ama_types.h"
#include "PMCTask.h"
#include "PowerManager.h"
#include "IVIPowerDevice.h"
#include "RSEPowerDevice.h"

using std::shared_ptr;
using std::string;
using std::map;

/// @class PowerMangaerController
/// @brief this class is the abstraction of PowerMangaerController
///
/// this class receive the task from TaskDispatcher. and
/// handle the actuall work about power State
class PowerManagerController
{
public:
    static PowerManagerController* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static PowerManagerController instance;
        return &instance;
    }
    ~PowerManagerController();

    void earlyInit(void);
    void Init(void);
    void doTask(shared_ptr<Task> task);
    bool isIVIDevicePowerOn(void);
    ama_PowerState_e getPowerState(uint32_t seat);
    void updatePowerStateByDevice(uint32_t seat,ama_PowerState_e powerState);
    void HandleAppRequest();
private:
    PowerManagerController();
private:
    IVIPowerDevice *mpIVIPowerDevice;
    RSEPowerDevice *mpLRSEPowerDevice;
    RSEPowerDevice *mpRRSEPowerDevice;
    ama_PowerState_e mIVIPowerStateFlag;
    ama_PowerState_e mLRSEPowerStateFlag;
    ama_PowerState_e mRRSEPowerStateFlag;
};

#endif //PMCTRL_H_
