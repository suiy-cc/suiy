/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APPMANAGERPROXY_H
#define APPMANAGERPROXY_H

#include "AppSDK.h"
#include "ama_audioTypes.h"
#include <mutex>

#include "CommonServiceLog.h"

class AppManagerProxy
{
public:
    static AppManagerProxy* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static AppManagerProxy instance;
        return &instance;
    }
    ~AppManagerProxy();
    void startListeningEvents(void);

    //excuting in project module thread
    void handleIGSingal(ama_PowerState_e powerState);
    void handleAbnormalSignal(ama_PowerState_e powerState);
    void startFactoryApp(void);
    void startCarlifeApp(void);
    void startWelinkApp(void);
protected:
    static void appStatusChangedCallback(const AMPID ampid, const AppStatus appStatus);
    static void powerStateChangedCallback(uint8_t seatID,ama_PowerState_e powerState);
    static void changeOfPowerStaChangedCallback(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta);
    static void changeOfAudioFocusCallback(ama_connectID_t connectID,ama_focusSta_t oldFocusSta,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID);
private:
    AppManagerProxy();
};

#endif//APPMANAGERPROXY_H
