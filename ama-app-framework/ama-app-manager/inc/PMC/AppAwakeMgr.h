/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APPAWAKEMGR_H
#define APPAWAKEMGR_H
#include <map>
#include <list>

#include "ama_types.h"
#include "PLCTask.h"
#include "TaskDispatcher.h"
#include "log.h"
#include "ama_powerTypes.h"

using std::shared_ptr;
using namespace std;

class AppAwakeMgr
{
public:
    static AppAwakeMgr* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static AppAwakeMgr instance;
        return &instance;
    }
    ~AppAwakeMgr();

    //clearAwakeStateWhenAppCrash
    void clearAwakeStateWhenAppCrash(AMPID ampid);

    //record awake request from app
    void handlePowerAwakeRequest(AMPID ampid,bool isPowerAwake);
    void handleScreenAwakeRequest(AMPID ampid,bool isScreenAwake);
    void handleAbnormalAwakeRequest(AMPID ampid,bool isAbnormalAwake);
    

    //when we change power state we should check those state!
    bool isPowerAwake();
    bool isScreenAwake();
private:
    AppAwakeMgr();

    void sendSignalToPMC(ama_PowerSignal_e powerAwakeSig);

    bool checkIsAppCanAwake(AMPID ampid);
    bool checkIsPowerAwakeInMap(void);
    bool checkIsScreenAwakeInMap(void);
    bool checkIsAbnormalAwakeInMap(void);    

    std::list<AMPID> mActiveAwakeAppList;
    std::map<AMPID,bool> mPowerAwakeAppMap;
    std::map<AMPID,bool> mScreenAwakeAppMap;
    std::map<AMPID,bool> mAbnormalAwakeAppMap;

    bool mIsPowerAwake;
    bool mIsScreenAwake;
    bool mIsAbnormalAwake;
};

#endif // APPAWAKEMGR_H
