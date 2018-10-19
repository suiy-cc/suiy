/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPLCImplementation.h
/// @brief contains class AppPLCImplementation
///
/// Created by zs on 2016/7/26.
/// this file contains the definition of class AppPLCImplementation
///

#pragma once

#include <condition_variable>
#include <string>

#include "Surface.h"
#include "PLCImplementation.h"

class AppPLC;

class AppPLCImplementation
: public PLCImplementation
{
public:
    AppPLCImplementation(AppPLC*);
    virtual ~AppPLCImplementation();

private:
    friend class AppManager;
    virtual void OnNotifyEarlyInitialize()override;
    virtual void OnNotifyInitialize(int startArg, bool visible);
    virtual void OnNotifyStop(const std::string &stopCMD);
    virtual void OnNotifyResume();
    virtual void OnNotifyPreHide();
    virtual void OnNotifyHide();
    virtual void OnNotifyPreShow(int arg)override ;
    virtual void OnNotifyShow();

    //cj add for screen sync
    virtual void OnNotifyActiveAppChange(unsigned int seatid);

public:
    virtual bool Initialize(unsigned int ampid);
    void SetStartupPicture(const std::string& path);
    AppPLC* GetPLCInterface();

private:
    AppPLC* plcInterface;

    Surface surface;

    // startup animation
    std::string startupPicture;
    Surface startupScreen;
    bool isStartupScreenReady;
    std::mutex mutexForIsStartupScreenReady;
    std::condition_variable conditionVariable;

    // resume screen
    std::string resumePicture;
    Surface resumeScreen;
    static const unsigned int fastResumeDuration;
};
