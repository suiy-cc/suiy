/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPLC.h
/// @brief contains class AppPLC
///
/// Created by zs on 2016/7/26.
/// this file contains the definition of class AppPLC
///

#ifndef APPPLC_H
#define APPPLC_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <ama_types.h>

class AppPLC
{
public:
    AppPLC();
    virtual ~AppPLC();

protected:
    friend class AppPLCImplementation;
    //virtual void OnInitialize();
    virtual void OnCreate()=0;
    virtual void OnPreShow(long nArgs)=0;
    virtual void OnShow()=0;
    virtual void OnResume()=0;
    virtual void OnPreHide()/*=0*/; // this interface may become pure virtual function in the future. so implement it.
    virtual void OnHide()=0;
    virtual void OnStop()=0;

    //cj add for srceen sync
    virtual void OnActiveAppChanged(unsigned int seatid);

public:
    bool Initialize(unsigned int ampid);
    void SetStartupPicture(const std::string& path);

protected:
    virtual void* GetImplementation();

protected:
    void* implementation;
};

void ShowMyself();
void HideMyself();
void StopMyself();
void StartHome();
void StartApp(unsigned int app, int startArg);
void Intent(unsigned int app, const std::string &action, const std::string &data);
void Intent(const std::string &itemID, const std::string &action, const std::string &data);
void Intent(unsigned int app, const std::string &action, const std::string &data, bool isQuiet);
void Intent(const std::string &itemID, const std::string &action, const std::string &data, bool isQuiet);

//cj add for screen sync
void getScreenShot(unsigned int seatid , char * picPath);
unsigned int getCurrentApp(int seatid);
void refreshScreenShot(int seatid,char *picPath);
void KeepScreenOn(E_SEAT seat, bool isAlwaysOn);
void SetLCDTimerStop(bool isStop);
void SetLCDScreenOn(E_SEAT seat);
void SetLCDPowerOff();
void AppPowerAwakeRequest(bool isAwake);
void requestPowerAwakeByApp(bool isPowerAwake);
void requestScreenAwakeByApp(bool isScreenAwake);
void requestAbnormalAwakeByApp(bool isAbnormalAwake);
void SetBrightness(int brightness,E_SEAT seatid);
void USBStateController(ama_USBOperate op);
void getUSBDevState(int ampid);
void RequestAppAlwaysOn(bool isAlwaysOn);
std::unordered_map<int32_t, int32_t> RequestLCDTemperature();

unsigned int GetAMPID();
unsigned int GetAMPID(const std::string& itemID);
std::string GetItemID();
std::string GetItemID(unsigned int app);
unsigned int GetSurfaceID(); // get main surface's ID

/// @name for video app, car-play app, car-life app, etc.
/// the two APIs returns ID of video-layer-surface &
/// video-background-surface of current app. if you use
/// those APIs, make sure app-manager support your app to
/// do so. if you are not sure, turn to system team for
/// more information.
/// @{
unsigned int GetVideoPlaySurfaceID(); // DEPRECATED
unsigned int GetVideoBKGSurfaceID(); // DEPRECATED
/// @}

/// HMI layers:
/// --------------------- normal layer      (layer number: 0x05)
/// --------------------- under layer       (layer number: 0x04)
/// --------------------- background layer  (layer number: 0x03)
/// @name additional layer
/// @{
unsigned int GetUnderLayerSurfaceID();
unsigned int GetBKGLayerSurfaceID();
/// @}


void KeepAppAlive(bool alive);
void KeepAppAlive(const std::string& itemID, bool alive);


typedef void (*ShutdownNotifyCB)(unsigned int);
void ListenToShutdownNotify(ShutdownNotifyCB f);


/// @name intent callback
/// @{
typedef void (*IntentNotifyCB)(const std::string&, const std::string&);
void ListenToIntentNotify(IntentNotifyCB f);
/// @}

/// @name state of app
/// @{
std::vector<AMPID> GetActiveAppList();
AppStatus GetAppState(AMPID app);
typedef void (*AppStateCB)(const AMPID, const AppStatus);
void ListenToAppStateNotify(AppStateCB f);
/// @}

/// WARNING: unless you know what you are doing,
/// don't use the functions below.
/// @name force PLC control
/// @{
void ForceMainSurfaceConfigured();
/// @}

#endif // FRAMEWORK_APPPLC_H
