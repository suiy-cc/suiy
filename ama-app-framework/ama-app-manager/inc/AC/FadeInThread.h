/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FadeInThread.h
/// @brief contains class FadeInThread
///
/// Created by wangqi on 2017/08/03.
/// this file contains the definination of class FadeInThread
///

#ifndef FADEINTHREAD_H_
#define FADEINTHREAD_H_

#include <mutex>
#include <vector>
#include <map>
#include <condition_variable>
#include <thread>
#include <stdio.h>
#include <unistd.h>

#include "AMGRLogContext.h"
#include "ama_audioTypes.h"
#include "ama_types.h"
#include "AMdefine.h"

using std::shared_ptr;
using std::vector;
using std::map;
using std::condition_variable;

typedef std::function<bool(int)> HandlerFuncType;

enum ama_FadeInType_e{
    E_SYSTEM_DEF = 0x00,
    E_DUCK_CARPLAY,
    E_FADEINTYPE_MAX
};

#define MAX_FADEIN_STEP (81)
#define SYSTEM_DEFAULT_DURATION (0)
#define SYSTEM_DEFAULT_INTERVAL (10)

/// @class FadeInThread
/// @brief this class is the abstraction of Audio module
///
class FadeInThread
{
public:
    FadeInThread(HandlerFuncType handler);
    ~FadeInThread();
    void setVolumeByFadeIn(ama_FadeInType_e fadeInType,int durations_ms,int fromGain,int toGain);
    void stopCurrentTask();
    void start();
    void stop();
private:
    void run();
    void planVolStepBySysDef(int fromGain,int toGain);
    void planVolStepByDuck(int durations_ms,int fromGain,int toGain);
    void debugToPrintVolPlan(void);
    void startDoingTask();
private:
    bool mIsRunning;
    bool mIsSetToStop;
    bool mIsDoingTask;
    bool mIsInterruptTask;

    int volStepArray[MAX_FADEIN_STEP];
    int mStepCnt;
    int mStepInterval_ms;

    HandlerFuncType mSetVolumeHandler;

    std::thread mThisThread;
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    ama_connectID_t mConnectID;
};

#endif //FADEINTHREAD_H_
