/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPLC.cpp
/// @brief contains the implementation of class AppPLC
///
/// Created by zs on 2016/7/26.
/// this file contains the implementation of class AppPLC
///

#include "AppPKGInfo.h"
#include "AppManager.h"
#include "AppPLC.h"
#include "AppPLCImplementation.h"
#include "HomePLC.h"
#include "LogContext.h"
#include "PowerSDK.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

AppPLC::AppPLC()
{
    implementation = GetImplementation();
}

AppPLC::~AppPLC()
{
    auto appPLC = static_cast<AppPLCImplementation*>(implementation);
    delete appPLC;
}
//cj add for srceen sync
void AppPLC::OnActiveAppChanged(unsigned int seatid)
{
    logWarn(SDK_PLC, "AppPLC::OnActiveAppChanged() is not implemented!");
}

void imeCloseIME();
uint32_t imeGetFocusAmpid();

//void AppPLC::OnInitialize()
//{
//    // it's okay if app developers don't use this interface.
//    logInfo(DT_APP_PLC, "AppPLC::OnInitialize() is not implemented.");
//}

void AppPLC::OnPreHide()
{
    logWarn(SDK_PLC, "AppPLC::OnPreHide() is not implemented!");
    if (imeGetFocusAmpid() == GetAMPID()) {
        imeCloseIME();
    }
}

bool AppPLC::Initialize(unsigned int ampid)
{
    auto appPLC = static_cast<AppPLCImplementation*>(implementation);

    return appPLC->Initialize(ampid);
}

void AppPLC::SetStartupPicture(const std::string &path)
{
    auto appPLC = static_cast<AppPLCImplementation*>(implementation);

    appPLC->SetStartupPicture(path);
}

void* AppPLC::GetImplementation()
{
    return new AppPLCImplementation(this);
}


void getScreenShot(unsigned int seatid , char * picPath)
{
    AppManager::GetInstance()->GetScreenShot(seatid,picPath);

    logInfo(SDK_PLC, "AppPLC::getScreenShot() picPath:" ,std::string(picPath));
}
unsigned int getCurrentApp(int seatid)
{
    return AppManager::GetInstance()->GetCurrentApp(seatid);
}
void refreshScreenShot(int seatid,char *picPath)
{
    AppManager::GetInstance()->refreshScreenShot(seatid,picPath);
}
void KeepScreenOn(E_SEAT seat, bool isAlwaysOn)
{
    AppManager::GetInstance()->SetLCDTimerStop(CHANGE_SEAT(AppManager::GetInstance()->GetAMPID(), seat), isAlwaysOn);

    if(isAlwaysOn)
        AppManager::GetInstance()->SetLCDScreenOn(seat);
}
void SetLCDTimerStop(bool isStop)
{
    AppManager::GetInstance()->SetLCDTimerStop(AppManager::GetInstance()->GetAMPID(), isStop);
}
void SetLCDPowerOff()
{
    AppManager::GetInstance()->SetLCDPowerOff();
}
void SetLCDScreenOn(E_SEAT seat)
{
    AppManager::GetInstance()->SetLCDScreenOn(seat);
}

void AppPowerAwakeRequest(bool isAwake)
{
    AppManager::GetInstance()->AppAwakeRequest(isAwake);
}

void requestPowerAwakeByApp(bool isPowerAwake)
{
    requestPowerAwake(AppManager::GetInstance()->GetAMPID(),isPowerAwake);
}

void requestScreenAwakeByApp(bool isScreenAwake)
{
    requestScreenAwake(AppManager::GetInstance()->GetAMPID(),isScreenAwake);
}

void requestAbnormalAwakeByApp(bool isAbnormalAwake)
{
    requestAbnormalAwake(AppManager::GetInstance()->GetAMPID(),isAbnormalAwake);
}

void SetBrightness(int brightness,E_SEAT seatid)
{
 AppManager::GetInstance()->SetBrightness( brightness,seatid);
}

void ShowMyself()
{
    Intent(GetAMPID(), std::string(), std::string());
}

void HideMyself()
{
    AppManager::GetInstance()->HideApp(GetAMPID());
}

void StopMyself()
{
    AppManager::GetInstance()->StopApp(GetAMPID());
}

void StartHome()
{
    AppManager::GetInstance()->StartHome();
}

void StartApp(unsigned int app, int startArg)
{
    Intent(app, std::string(), std::string());
}

void Intent(unsigned int app, const std::string &action, const std::string &data)
{
    Intent(app, action, data, false);
}

void Intent(const std::string &itemID, const std::string &action, const std::string &data)
{
    Intent(itemID, action, data, false);
}

void Intent(unsigned int app, const std::string &action, const std::string &data, bool isQuiet)
{
    Intent(GetItemID(app), action, data, isQuiet);
}

void Intent(const std::string &itemID, const std::string &action, const std::string &data, bool isQuiet)
{
    AppManager::GetInstance()->Intent(itemID, action, data, isQuiet);
}

unsigned int GetAMPID()
{
    return AppManager::GetInstance()->GetAMPID();
}

unsigned int GetAMPID(const std::string& itemID)
{
    return AppManager::GetInstance()->GetAMPID(itemID);
}

std::string GetItemID()
{
    return GetItemID(GetAMPID());
}

std::string GetItemID(unsigned int app)
{
    auto package = AppManager::GetInstance()->GetAppPKGInfo(app);
    return std::move(package.itemID);
}

unsigned int GetSurfaceID()
{
    return AppManager::GetInstance()->GetSurfaceID();
}

unsigned int GetVideoPlaySurfaceID()
{
    return GetUnderLayerSurfaceID();
}

unsigned int GetVideoBKGSurfaceID()
{
    return GetBKGLayerSurfaceID();
}

unsigned int GetUnderLayerSurfaceID()
{
    return AppManager::GetInstance()->GetUnderLayerSurfaceID();
}

unsigned int GetBKGLayerSurfaceID()
{
    return AppManager::GetInstance()->GetBKGLayerSurfaceID();
}

void KeepAppAlive(bool alive)
{
   // return AppManager::GetInstance()->KeepAppAlive(alive);
}

void KeepAppAlive(const std::string& itemID, bool alive)
{
  //  return AppManager::GetInstance()->KeepAppAlive(itemID, alive);
}

void ListenToShutdownNotify(ShutdownNotifyCB f)
{
    AppManager::GetInstance()->SetShutdownNotifyCB(f);
}

void ListenToIntentNotify(IntentNotifyCB f)
{
    AppManager::GetInstance()->SetIntentNotifyCB(f);
}

std::vector<AMPID> GetActiveAppList()
{
    return std::move(AppManager::GetInstance()->GetActiveAppList());
}

AppStatus GetAppState(AMPID app)
{
    return AppManager::GetInstance()->GetAppState(app);
}

void ListenToAppStateNotify(AppStateCB f)
{
    AppManager::GetInstance()->SetAppStateNotifyCB(f);
}

void ForceMainSurfaceConfigured()
{
    AppManager::GetInstance()->ForceMainSurfaceConfigured(GetAMPID());
}
void USBStateController(ama_USBOperate op)
{
    AppManager::GetInstance()->USBStateControll(op);
}

void getUSBDevState(int ampid)
{
    AppManager::GetInstance()->getUSBDevState(ampid);
}

void RequestAppAlwaysOn(bool isAlwaysOn)
{
 AppManager::GetInstance()->requestAlwaysOn(isAlwaysOn);
}
std::unordered_map<int32_t, int32_t> RequestLCDTemperature()
{
   return AppManager::GetInstance()->GetLCDTemperature();
}
