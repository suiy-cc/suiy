/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HPAppPLC.cpp
/// @brief contains the implementation of High-priorty AppPLC APIs
///
/// Created by zs on 2016/8/16.
/// this file contains the implementation of High-priorty AppPLC APIs
///

#include "LogContext.h"
#include "HomePLC.h"
#include "AppManager.h"
#include "HomePLCImplementation.h"
#include "AppPKGInfo.h"

#include <fstream>

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

void StopApp(unsigned int app)
{
    AppManager::GetInstance()->StopApp(app);
}

void HideApp(unsigned int app)
{
    AppManager::GetInstance()->HideApp(app);
}

HomePLC::HomePLC()
{
    implementation = GetImplementation();
}

HomePLC::~HomePLC()
{

}

void *HomePLC::GetImplementation()
{
    return new HomePLCImplementation(this);
}

std::vector<AppPKG> GetAppList()
{
    logVerbose(SDK_AMGR, "GetAppList()--->IN");

    std::vector<AppPKG> pkgList;
    std::vector<AppPKGInfo> pkgInfoList = AppManager::GetInstance()->GetAppInfoList();
    for(auto& appPKGInfo: pkgInfoList)
    {
        pkgList.push_back(ChangeToAppPKGInfo(appPKGInfo));
    }
    logVerbose(SDK_AMGR, "GetAppList()--->OUT");
    return std::move(pkgList);
}

AppPKG GetAppPKGInfo(unsigned int app)
{
    logVerbose(SDK_AMGR, "GetAppPKGInfo(", app, ")--->IN");

    AppPKGInfo appPKGInfo = AppManager::GetInstance()->GetAppPKGInfo(app);
    AppPKG appPKG = ChangeToAppPKGInfo(appPKGInfo);

    logVerbose(SDK_AMGR, "GetAppPKGInfo(", app, ")--->OUT");
    return appPKG;
}

std::vector<AppPKGEx> GetAppListEx()
{
    FUNCTION_SCOPE_LOG_C(SDK_AMGR);

    std::vector<AppPKGEx> pkgExList;
    std::vector<AppPKGInfo> pkgInfoList = AppManager::GetInstance()->GetAppInfoList();
    for(auto& appPKGInfo: pkgInfoList)
        pkgExList.push_back(ChangeToAppPKGInfoEx(appPKGInfo));

    return std::move(pkgExList);
}

AppPKGEx GetAppPKGInfoEx(unsigned int app)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("GetAppPKGInfo(%#x)", app));

    AppPKGInfo appPKGInfo = AppManager::GetInstance()->GetAppPKGInfo(app);
    AppPKGEx appPKGEx = ChangeToAppPKGInfoEx(appPKGInfo);

    return appPKGEx;
}

AppPKGEx ChangeToAppPKGInfoEx(AppPKGInfo appPKGInfo)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("ChangeToAppPKGInfoEx(%#x)", appPKGInfo.ampid));

    AppPKGEx appPKGEx;

    appPKGEx.SetAmpid(appPKGInfo.ampid);
    appPKGEx.SetAppName(appPKGInfo.appName);
    appPKGEx.SetItemID(appPKGInfo.itemID);
    appPKGEx.SetBigIconPath(appPKGInfo.bigIconPath);
    appPKGEx.SetSmallIconPath(appPKGInfo.smallIconPath);
    appPKGEx.Set("AppType", appPKGInfo.appType);

    logInfoF(SDK_AMGR, "ampid=%#x name=%s itemID=%s bIconPath=%s sIconPath=%s AppType=%s", appPKGEx.GetAmpid(), appPKGEx.GetAppName().c_str(), appPKGEx.GetItemID().c_str(), appPKGEx.GetBigIconPath().c_str(), appPKGEx.GetSmallIconPath().c_str(), appPKGEx.Get("AppType")->c_str());
    return appPKGEx;
}

AppPKG ChangeToAppPKGInfo(AppPKGInfo appPKGInfo)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("ChangeToAppPKGInfo(%#x)", appPKGInfo.ampid));

    AppPKG appPKG;

    appPKG.SetAmpid(appPKGInfo.ampid);
    appPKG.SetAppName(appPKGInfo.appName);
    appPKG.SetItemID(appPKGInfo.itemID);
    appPKG.SetBigIconPath(appPKGInfo.bigIconPath);
    appPKG.SetSmallIconPath(appPKGInfo.smallIconPath);
    
    logInfoF(SDK_AMGR, "ampid=%#x name=%s itemID=%s bIconPath=%s sIconPath=%s", appPKG.GetAmpid(), appPKG.GetAppName().c_str(), appPKG.GetItemID().c_str(), appPKG.GetBigIconPath().c_str(), appPKG.GetSmallIconPath().c_str());
    return appPKG;
}