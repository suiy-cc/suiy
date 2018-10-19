/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef BOOTLISTENER_H
#define BOOTLISTENER_H

#include <mutex>
#include <thread>

#include "ama_types.h"

/// @class BootListener
/// @brief this class is the abstraction of BootListener
///
/// this class check IVI boot state. and send boot signal
/// to PMC
class BootListener
{
public:
    static BootListener* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static BootListener instance;
        return &instance;
    }
    ~BootListener();

    void IVCInit();
    int setLCDBrightness(int screen_type , int brightness);
    int setLCDPower(int screen_type , int bl_power);
    bool checkIsWelcomePlaying(void);
    bool checkIsOpeningPlaying(void);
    void startListenBootSta(void);
    void stopListen(void);
    bool isBootOver(){return mIsBootOver;}
    bool isFirstAppReady(){return mIsFirstAppReady;}
    bool isWelcomeOver(){return mIsWelcomeOver;}
    void stopOpeningMode(void);
    void setFirstAppReady(void);
    void setWelcomeOver(bool isOver){mIsWelcomeOver=isOver;}
    void reqWelcomeFlg();
protected:
    static void cb_AppOnShow(AMPID ampid);
private:
    BootListener();
    void listeningRun();
    bool checkIsFirstAppAlreadyOk(void);

    bool mIsBootOver;
    unsigned int mHomeOnShowHandle;
    static const std::string mOpeningModeDevFile;
    static const std::string mWelcomeDeviceFile;
    static const std::string mFirstAppReadyFile;

    bool mIsWelcomeFileExist;
    std::thread mListeningThread;

    bool mIsWelcomeOver;
    bool mIsFirstAppReady;
};

#endif//BOOTLISTENER_H
