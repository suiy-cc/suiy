/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PROCTRL_H_
#define PROCTRL_H_

#include "ProTask.h"

#include "ama_audioTypes.h"
#include "ama_powerTypes.h"
#include "ama_types.h"

#include <mutex>
#include <memory>
#include <map>

using std::shared_ptr;
using std::string;
using std::map;

class ProjectController
{
public:
    static ProjectController* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static ProjectController instance;
        return &instance;
    }
    ~ProjectController();

    void doTask(shared_ptr<Task> task);
private:
    ProjectController();
    void ProjectControllerInit(void);
    void notifyIVIPowerStaToCluster(void);
    void handleIVIPowerStaChanged(ama_PowerState_e iviPowerState);
    void handleChangeOfPowerChanged(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta);
    void handleAudioFocusChanged(ama_connectID_t connectID,ama_focusSta_t oldFocusSta,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
    void handleAppStateChanged(const AMPID ampid, const AppStatus appStatus);
    void handleWelinkIOSDeviceAttachedState(bool isAttached);
    void handleWelinkSelectedStateInSyssetting(bool isSelected);
private:
    ama_PowerState_e mCurIVIPowerSta;
    bool mIsWelinkIOSDeviceAttached;
    bool mIsWelinkSelectedInSyssetting;
};

void sendInitTaskToProjectModule(void);
void updateWelinkIOSDeviceAttacheStaToPro(bool isAttached);
void updateWelinkSelectedStaToPro(bool isSelected);

#endif //PROCTRL_H_
