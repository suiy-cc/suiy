/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppProfileManager.h
/// @brief contains 
/// 
/// Created by zs on 2017/09/18.
///
/// this file contains the definition of class AppProfileManager
/// 

#ifndef APPPROFILEMANAGER_H
#define APPPROFILEMANAGER_H

#include <unordered_map>

#include "ama_enum.h"
#include "AppProfile.h"
#include "ThreadSafeValueKeeper.h"

class AppProfileManager
{
public:
    virtual ~AppProfileManager();

    static AppProfileManager* GetInstance();

    void OnAppStart(AMPID app);
    void OnStartCMDSent(AMPID app);
    void OnStarted(AMPID app);
    void OnAppSDKBootLogReceived(AMPID app, const std::vector<std::string>& bootLog);
    void OnSurfaceCreated(AMPID app);
    void OnSurfaceConfigured(AMPID app);
    void OnPreshowIn(AMPID app);
    void OnPreshowOut(AMPID app);
    void OnSurfaceShow(AMPID app);
    void OnSurfaceHide(AMPID app);
    void OnLoadingScreenEnd(AMPID app);
    void AppendLog(AMPID app, const std::string& log);

protected:
    void RegisterCB();
    void UnregisterCB();

private:
    AppProfileManager(); // for singleton pattern

    static const std::string dir;
    typedef std::unordered_map<AMPID, AppProfile> ProfileMapType;
    ThreadSafeValueKeeper<ProfileMapType> profiles;
    unsigned int startProcCBHandle;
    unsigned int startedCBHandle;
    unsigned int showCBHandle;
};


#endif // APPPROFILEMANAGER_H
