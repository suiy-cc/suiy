/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef POWERMANAGER_H_
#define POWERMANAGER_H_

#include <mutex>

#include "AMGRLogContext.h"
#include "ama_types.h"

using std::shared_ptr;
using std::string;
using std::map;

class PowerManager
{
public:
    static PowerManager* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static PowerManager instance;
        return &instance;
    }
    ~PowerManager();
    void Init(void);
    void sendSignalToPMC(int powerMode, int detail);
    void SetScreenOnByApp();
    void responsePowerMode(int powerMode);
    void activeAppStaChanged();
    bool isAccOn(void);
    bool isPowerOn(void);
private:
    PowerManager();
    void DeInit();
};

#endif //POWERMANAGER_H_
