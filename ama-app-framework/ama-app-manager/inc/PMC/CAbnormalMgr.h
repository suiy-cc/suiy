/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#pragma once

#include <mutex>
#include <memory>
#include "ama_enum.h"
#include "ama_types.h"
#include "IPowerState.h"
#include "PowerState.h"

using namespace std;
using std::shared_ptr;

class CAbnormalMgr
{
public:
    // static AbnormalMgr* GetInstance(){
    //     // the initialization of local static variable is not thread-safe
    //     std::mutex Mutex;
    //     std::unique_lock<std::mutex> Lock(Mutex);

    //     static AbnormalMgr instance;
    //     return &instance;
    // }
    CAbnormalMgr();
    ~CAbnormalMgr();

    ama_PowerState_e getCurAbnormalStateFlag();
    shared_ptr<IPowerState> getCurAbnormalState();
    void updateCurAbnormalState();
    void recordAbnormalFlag(ama_PowerSignal_e powerSignal);
    
    void changeAbnormalPowerStateTo(shared_ptr<IPowerState> pPowerState);

private:

    void setLastScreenTemp(int temp);
    int getLastScreenTemp() const;

    void setSocVoltageFlag(int value);
    int getSocVoltageFlag() const;
    
    void setIsSocTempNormal(bool value);
    bool getIsSocTempNormal() const;    

    void setIsAbnormalAwake(bool value);
    bool getIsAbnormalAwake() const;    
    
    shared_ptr<IPowerState> mpAbnormalPowerState;
    int mSocVoltageFlag;
    bool isSocTempNormal;
    int mLastScreenTemp;
    bool isAbnormalAwake;
};