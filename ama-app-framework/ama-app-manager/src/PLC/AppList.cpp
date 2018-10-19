/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <map>
#include <stdio.h>
#include <libpminfo.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <COMMON/UniqueID.h>

#include "SMList.h"
#include "AppList.h"
#include "HMICTask.h"
#include "StringOperation.h"
#include "ProcessInfo.h"
#include "ActiveAppHome.h"
#include "ReadXmlConfig.h"
#include "rpc_proxy.h"
#include "COMMON/TimeStamp.h"
#include "COMMON/NotifyWaiter.h"
#include "COMMON/DomainInvoker.h"
#include "StartService.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

extern RpcProxy rp;

static const ItemType COMMONAPP         = {E_PROCCESS_TYPE_APP,     "COMMONAPP"};
static const ItemType HTML5             = {E_PROCCESS_TYPE_APP,     "HTML5"};
static const ItemType THIRDAPP          = {E_PROCCESS_TYPE_APP,     "THIRDAPP"};
static const ItemType SYSTEMAPP         = {E_PROCCESS_TYPE_APP,     "SYSTEMAPP"};
static const ItemType EXTAPP            = {E_PROCCESS_TYPE_APP,     "EXTAPP"};
static const ItemType PRIVATESERVICE    = {E_PROCCESS_TYPE_SERVICE, "PRIVATESERVICE"};
static const ItemType PUBLICSERVICE     = {E_PROCCESS_TYPE_SERVICE, "PUBLICSERVICE"};
static const ItemType SYSTEMSERVICE     = {E_PROCCESS_TYPE_SERVICE, "SYSTEMSERVICE"};
static const std::vector<ItemType> itemTypes = {
    COMMONAPP,
    SYSTEMAPP,
    PRIVATESERVICE,
    PUBLICSERVICE,
    SYSTEMSERVICE,
    HTML5,
    THIRDAPP,
    EXTAPP,
};

static std::string appListFetchDiagPath = "/media/ivi-data/ama.d/AppListFetchDiag.log";

bool operator==(const ItemType& l, const ItemType& r)
{
    return l.proccessType == r.proccessType
            && l.typeName == r.typeName;
}

void pminfo_exception_cb(int error)
{
    logError(PLC, LOG_HEAD, "error code: ",error);
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "error code: " << error << std::endl;
        oFile.close();
    }
}

static void pminfo_status_callback(
        pminfo_status_t status,
        const void* detail,
        void* user)
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);

    logInfo(PLC, LOG_HEAD, "detail = ", detail);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "detail = " << (const char*)detail << "\n";
    }

    auto locale = AppList::GetInstance()->GetCurrentLocale();
    int isLoaded = pminfo_is_database_loaded(locale.c_str());
    logInfo(PLC, LOG_HEAD, "current locale = ", locale, ", isLoaded = ", isLoaded);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "current locale = " << locale << ", isLoaded = " << isLoaded << "\n";
    }

    // app list is loaded
    if(isLoaded)
    {
        // notify PLC
        logInfo(PLC, LOG_HEAD, "current locale is loaded.");
        if(oFile.is_open())
        {
            oFile << TimeStamp::GetString() << LOG_HEAD << "current locale is loaded." << std::endl;
        }
        Notify(reinterpret_cast<unsigned long long>(user));

        // update app list if DB load is late
        if(AppList::GetInstance()->IsLoadAppListTimeout())
        {
            logWarn(PLC, LOG_HEAD, "PMInfo DB ready notify is late.");
            if(oFile.is_open())
            {
                oFile << TimeStamp::GetString() << LOG_HEAD << "PMInfo DB ready notify is late." << "\n";
                oFile.close();
            }

            // update app list
            logInfo(PLC, LOG_HEAD, "try to fix app list");
            auto plcTask = std::make_shared<PLCTask>();
            plcTask->SetType(PLC_CMD_UPDATE_APPLIST);
            plcTask->SetPriority(E_TASK_PRIORITY_HIGH);
            TaskDispatcher::GetInstance()->Dispatch(plcTask);

            // notify homes
            logInfo(PLC, LOG_HEAD, "notify homes to update package list.");
            std::vector<E_SEAT> seats = GetAvailableScreens();
            logInfo(PLC, LOG_HEAD, "total seats: ", seats.size());
            for (std::vector<E_SEAT>::iterator seat = seats.begin() ; seat != seats.end(); ++seat)
            {
                AMPID home = HOME_AMPID(*seat);
                logInfo(PLC, LOG_HEAD, "home = ", ToHEX(home));

                auto procInfo = ProcessInfo::GetInstance();
                procInfo->Lock();
                auto homeObj = dynamic_pointer_cast<ActiveAppHome>(procInfo->FindApp(home));
                procInfo->Unlock();

                if(homeObj)
                    homeObj->InstalledPKGsChangedNotify();
                else
                    logError(PLC, LOG_HEAD, "home ", ToHEX(home), " is not found!");
            }

            AppList::GetInstance()->SetLoadAppListTimeout(false);
        }
    }
}

void AppList::pkgUpdateCallback(const char *id, const char *info, void *user_data)
{
    DomainVerboseLog chatter(PLC, formatText("AppList::pkgUpdateCallback(%s, %s)", id, info));
    std::unique_lock<std::mutex> lock(AppList::GetInstance()->mutex4ListApp);
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "id = " << id << ", info = " << info << "\n";
    }

    AppList::GetInstance()->updateAppList();

    // notify home that app list has changed
    std::vector<E_SEAT> seats = GetAvailableScreens();
    logInfo(PLC, LOG_HEAD, "total seat cout: ", seats.size());
    for (std::vector<E_SEAT>::iterator seat = seats.begin() ; seat != seats.end(); ++seat)
    {
        logInfo(PLC, LOG_HEAD, "pushing package-update message to home ", ToHEX(HOME_AMPID(*seat)));

        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto home = dynamic_pointer_cast<ActiveAppHome>(procInfo->FindApp(HOME_AMPID(*seat)));
        procInfo->Unlock();
        if(home)
            home->InstalledPKGsChangedNotify();
        else
            logError(PLC, LOG_HEAD, "home ", ToHEX(HOME_AMPID(*seat)), " is not found!");
    }


}

const App * FastAppList::GetApp(const AMPID app)const
{
    if(AMPID2app.find(app)==AMPID2app.end())
    {
        logWarn(PLC, "FastAppList::GetApp(): app ", ToHEX(app), " is not found.");
        return nullptr;
    }
    else
        return &(AMPID2app.at(app));
}

const App * FastAppList::GetApp(const std::string &itemID)const
{
    if(itemID2app.find(itemID)==itemID2app.end())
    {
        logWarn(PLC, "FastAppList::GetApp(): app ", itemID, " is not found.");
        return nullptr;
    }
    else
        return &(itemID2app.at(itemID));
}

std::vector<AMPID> FastAppList::GetApps(std::function<bool(const App *)> filter)const
{
    std::vector<AMPID> rtv;
    rtv.reserve(AMPID2app.size());
    for(auto& app: AMPID2app)
        if(filter(&(app.second)))
            rtv.push_back(app.second.getAMPID());

    return std::move(rtv);
}

void FastAppList::AddApp(const App& app)
{
    DomainVerboseLog chatter(PLC, formatText("FastAppList::AddApp(%#x, %s, %s)", app.getAMPID(), app.getItemID().c_str(), app.getTarget().c_str()));

    AMPID ampid = app.getAMPID();
    std::string itemID = app.getItemID();
    if(ampid==INVALID_AMPID || itemID.empty())
    {
        logWarnF(PLC, "FastAppList::AddApp(): invalid AMPID or null itemID! AMPID[%#x] itemID[%s]", ampid, itemID.c_str());
    }
    else
    {
        AMPID2app[ampid] = app;
        itemID2app[itemID] = app;
    }
}

void FastAppList::DeleteApp(const AMPID app)
{
    if(AMPID2app.find(app)!=AMPID2app.end())
    {
        auto itemID = AMPID2app[app].getItemID();
        itemID2app.erase(itemID);
        AMPID2app.erase(app);
    }
}

void FastAppList::DeleteApp(const std::string &itemID)
{
    if(itemID2app.find(itemID)!=itemID2app.end())
    {
        auto ampid = itemID2app[itemID].getAMPID();
        AMPID2app.erase(ampid);
        itemID2app.erase(itemID);
    }
}

void FastAppList::Clear()
{
    AMPID2app.clear();
    itemID2app.clear();
}

unsigned int FastAppList::Count()const
{
    if(AMPID2app.size()!=itemID2app.size())
        logError(PLC, "FastAppList::Count(): the size of AMPID2app & itemID2app are not the same!!!");
    return AMPID2app.size();
}

AppList::AppList()
: isLoadAppListTimeout(false)
{
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << "===================================" << std::endl;
        oFile.close();
    }
}

AppList::~AppList()
{
    pkgmgr_unregister_package_update();
	pminfo_release();
}

void AppList::fastInit()
{
    DomainVerboseLog chatter(PLC, "AppList::fastInit()");

    std::unique_lock<std::mutex> lock(mutex4ListApp);

    // get app info from cache file
    auto apps = fastBootList.GetFastBootApps();

    // append homes after app list
    for(int i = 0; i < apps.size(); ++i)
    {
        this->apps.AddApp(apps[i]);
    }
}

void AppList::init()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    waitForPKGMGR();
    InitEOLInfo();
    if(waitForPMInfoDB())
    {
        std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
        if(oFile.is_open())
        {
            oFile << TimeStamp::GetString() << LOG_HEAD << "update app list..." << std::endl;
            oFile.close();
        }
        updateAppList();
    }
    observeSYSLang();
    registerUpdateCallBack();
    logVerbose(PLC,"AppList::init-->>OUT");
}
void AppList::waitForPKGMGR()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);

    int ret=pminfo_new(homeId[0], pminfo_exception_cb);
    while(PKGMGR_ERROR_NONE != ret) {
        logInfo(PLC, LOG_HEAD, "package manager is not ready yet, error code: ",ret);
        if(oFile.is_open())
        {
            oFile << TimeStamp::GetString() << LOG_HEAD << "package manager is not ready yet, error code: " << ret << '\n';
        }
        usleep(1000);
        ret=pminfo_new(homeId[0], pminfo_exception_cb);
    }

    oFile.close();
    logInfo(PLC, LOG_HEAD, "package manager is ready.");
}
void AppList::InitEOLInfo()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    eolInfo = rp.getEOLInfo();

    SetCurrentLocale(((uint8_t)eolInfo.LANG == 0)? //0x00:Chinese  0x01:English
                                             localeArray[LOCALE_CHINESE]:
                                             localeArray[LOCALE_ENGLISH]);
}

bool AppList::waitForPMInfoDB()
{
    FUNCTION_SCOPE_LOG_C(PLC);
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "waiting for PMInfo DB ready..." << std::endl;
    }

    unsigned long long handler = GetUniqueID();
    DomainInvoker invoker([handler](){RegisterWaiterID(handler);}, [handler](){UnregisterWaiterID(handler);});
    pminfo_register_status_callback(pminfo_status_callback, reinterpret_cast<void*>(handler));
    int isLoaded = pminfo_is_database_loaded(GetCurrentLocale().c_str());
    if(isLoaded==0)
    {
        bool isIntime = Wait(handler, 3000); // wait for PMInfo DB for 3 seconds at most
        if(!isIntime)
        {
            isLoadAppListTimeout = true;
            logError(PLC, LOG_HEAD, "pminfo DB loading is timeout.");
            if(oFile.is_open())
            {
                oFile << TimeStamp::GetString() << LOG_HEAD << "pminfo DB loading is timeout." << std::endl;
            }
            return false;
        }
        else
        {
            isLoadAppListTimeout = false;
            logInfo(PLC, LOG_HEAD, "pminfo DB is ready.");
            if(oFile.is_open())
            {
                oFile << TimeStamp::GetString() << LOG_HEAD << "pminfo DB is ready." << std::endl;
            }
            return true;
        }
    }
    else
    {
        logInfo(PLC, LOG_HEAD, "pminfo DB is already loaded.");
        if(oFile.is_open())
        {
            oFile << TimeStamp::GetString() << LOG_HEAD << "pminfo DB is already loaded." << std::endl;
        }
        return true;
    }

    return true;
}

std::set<string> AppList::getDisabledApps()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    auto itemsFromEOL = getAppsDisabledByEOL();
    auto itemsFromConfig = getAppsDisabledByConfig();

    for(auto& item: itemsFromConfig)
        itemsFromEOL.insert(item);

    return std::move(itemsFromEOL);
}

std::set<string> AppList::getAppsDisabledByEOL()
{
    std::set<string> itemIDs;
    XMLConfigMMap mmap;
    GetConfigFromMMap("EOLName2ItemID", mmap);

    if(eolInfo.AP==0)
    {
        auto idRange = mmap.equal_range("AP");
        for(auto i = idRange.first; i != idRange.second; ++i)
            itemIDs.insert(i->second);
    }
#ifndef CODE_FOR_EV
    if(eolInfo.MPI==0)
    {
        auto idRange = mmap.equal_range("MPI");
        for(auto i = idRange.first; i != idRange.second; ++i)
            itemIDs.insert(i->second);
    }
    if(eolInfo.RI_AVS==0)
    {
        auto idRange = mmap.equal_range("RI_AVS");
        for(auto i = idRange.first; i != idRange.second; ++i)
            itemIDs.insert(i->second);
    }
    if(eolInfo.TBOX==0)
    {
        auto idRange = mmap.equal_range("TBOX");
        for(auto i = idRange.first; i != idRange.second; ++i)
            itemIDs.insert(i->second);
    }
    if(eolInfo.BROW==0)
    {
        auto idRange = mmap.equal_range("BROW");
        for(auto i = idRange.first; i != idRange.second; ++i)
            itemIDs.insert(i->second);
    }
#endif // CODE_FOR_EV

    return std::move(itemIDs);
}

std::set<string> AppList::getAppsDisabledByConfig()
{
    std::vector<std::string> disabledApps;
    GetConfigFromVector("DisabledApps", disabledApps);

    std::set<std::string> items;
    for(auto& app: disabledApps)
        items.insert(app);

    return std::move(items);
}

void AppList::clear()
{
    DomainVerboseLog chatter(PLC, "AppList::clear()");

}

void AppList::_updateAppList(const char* locale)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%s)", locale));

    // get app list
    unsigned int pmListCount = 0;
    std::vector<std::vector<App>> appVecVec;
    appVecVec.reserve(itemTypes.size());
    for (const auto& itemType: itemTypes)
    {
        pminfo_item_list_t itemList;
        pminfo_get_package_item_list_by_type(locale,itemType.typeName.c_str(), &itemList);

        if(itemList.count>0)
        {
            appVecVec.resize(appVecVec.size()+1);
            appVecVec.back() = PMInfoList2AppList(itemType, &itemList);
            pmListCount += itemList.count;
        }
        pminfo_free_package_item_list(&itemList);
    }
    logInfo(PLC, LOG_HEAD, "total PM-items are: ", pmListCount);
    std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "total PM-items are: " << pmListCount << '\n';
    }

    // filter disabled apps
    static auto disabledApps = getDisabledApps(); // disabled apps are static. so we just call it once
    for(auto& subVec: appVecVec)
        for(auto& app: subVec)
        {
            if(disabledApps.find(app.getItemID())!=disabledApps.end())
            {
                logInfo(PLC, LOG_HEAD, "app [", app.getItemID(), "] is disabled.");
                continue;
            }

            apps.AddApp(app);
        }
    logInfo(PLC, LOG_HEAD, "total apps are: ", apps.Count());
    if(oFile.is_open())
    {
        oFile << TimeStamp::GetString() << LOG_HEAD << "total apps are: " << apps.Count() << '\n';
    }
}

void AppList::updateAppList()
{
    FUNCTION_SCOPE_LOG_C(PLC);

    if(!pminfo_is_database_loaded(GetCurrentLocale().c_str()))
    {
        logError(PLC, LOG_HEAD, "current locale [", GetCurrentLocale(), "] is not ready yet!");
        std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
        if(oFile.is_open())
        {
            oFile << TimeStamp::GetString() << LOG_HEAD << "current locale [" << GetCurrentLocale() << "] is not ready yet!" << '\n';
        }
        return;
    }

    _updateAppList(GetCurrentLocale().c_str());

    // if the number of apps is less than lasttime
    // ama-app-manager start up, we assume that current
    // app fetch is failed.
    // in this case, we try again 300 ms later.
    appCount.SetCurrentAppCount(apps.Count());
    if(appCount.IsThereMissingApps() || apps.Count()<50)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        _updateAppList(GetCurrentLocale().c_str());
    }

    static bool isStarted = false;
    if(!isStarted)
    {
        // proceed auto start after app list is ready
        logInfo(PLC, LOG_HEAD, "send task to boot auto start apps");
        SendAutoStartTask();
        isStarted = true;
    }
}

void AppList::OnSYSLANGChanged(const std::string& key, const SMValue& value, SMValueObserver::E_EVENT event)
{
    if(key==SYS_LANG && event==SMValueObserver::E_EVENT_VALUE_CHANGED)
    {
        DomainVerboseLog chatter(PLC, "AppList::OnSYSLANGChanged()");

        std::unique_lock<std::mutex> lock(AppList::GetInstance()->mutex4ListApp);

        logInfo(PLC, LOG_HEAD, "current language: ", AppList::GetInstance()->GetCurrentLocale());
        logInfo(PLC, LOG_HEAD, "new language: ", value.value);
        if(AppList::GetInstance()->GetCurrentLocale()==value.value)
        {
            logInfo(PLC, LOG_HEAD, "updating app list is not necessary.");
            return;
        }

        // change current locale
        AppList::GetInstance()->SetCurrentLocale(value.value);
        std::ofstream oFile(appListFetchDiagPath.c_str(), std::ios::app);
        if(oFile.is_open())
        {
            oFile << TimeStamp::GetString() << LOG_HEAD << "locale changed. new locale: [" << value.value << "]" << '\n';
            oFile.close();
        }

        // update app list
        if(pminfo_is_database_loaded(value.value.c_str())) // make sure pminfo DB is ready
        {
            AppList::GetInstance()->apps.Clear();
            AppList::GetInstance()->updateAppList();
            logDebug(PLC, "AppList::OnSYSLANGChanged(): app list is updated.");
        }
    }
}

void AppList::observeSYSLang()
{
    DomainVerboseLog chatter(PLC, "AppList::observeSYSLang()");

    sSMKeyValuePairObserver.AddObserver(SYS_LANG, &AppList::OnSYSLANGChanged, SMValueObserver::E_EVENT_VALUE_CHANGED);
}
void AppList::registerUpdateCallBack()
{
    DomainVerboseLog chatter(PLC, "AppList::registerUpdateCallBack()");
  //  pkgmgr_compile_version();
    pkgmgr_register_package_update("appFramework",pkgUpdateCallback,NULL);
}

static void PMItem2App(pminfo_item_t& pmItem, App& app, const ItemType& itemType)
{
    E_APPID appID = GetAppID(::GetAppIDStr(pmItem.id));

    app.setItemID(ToString(pmItem.id));
    app.setAppName(ToString(pmItem.label));
    app.setAppType(ToString(pmItem.type));
    app.setAMPID(AMPID_CUSTOM(GetScreenID(pmItem.id), itemType.proccessType, appID));
    app.setBigIcon(ToString(pmItem.large_icon));
    app.setSmallIcon(ToString(pmItem.small_icon));
    app.setSystemdFile(ToString(pmItem.systemd));
    app.setIsAutoStart(pmItem.isAutoStart);
    app.setDisableStartupAnimation(pmItem.DisableStartupScreen);
    app.setTarget(ToString(pmItem.target));
}

std::vector<App> AppList::PMInfoList2AppList(const ItemType &itemType, pminfo_item_list_t *list)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%s)", itemType.typeName.c_str()));

    std::vector<App> appVec;
    appVec.reserve(list->count);

    for (int i=0;i<list->count;i++)
    {
        appVec.resize(appVec.size()+1);
        PMItem2App(list->item[i], appVec.back(), itemType);
    }

    return std::move(appVec);
}

void AppList::UpdateAppList()
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    updateAppList();
}

bool AppList::IsValidApp(AMPID process)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    return apps.GetApp(process)!=nullptr;
}

App AppList::GetApp(AMPID app)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    App rtv;
    auto pApp = apps.GetApp(app);
    if(pApp)
    {
        rtv = *pApp;
    }
    return std::move(rtv);
}

std::string AppList::GetAppName(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appPkgInfo = apps.GetApp(app);

    if(appPkgInfo)
    {
        return appPkgInfo->getAppName();
    }
    else
        return std::string();
}

std::string AppList::GetPackageIcon(std::string packageID)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    return "";
}

std::string AppList::GetItemID(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appPkgInfo = apps.GetApp(app);

    if(appPkgInfo)
    {
        return appPkgInfo->getItemID();
    }
    else
        return std::string();
}

AMPID AppList::GetAMPID(const std::string& itemID)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto app = apps.GetApp(itemID);
    if(app)
        return app->getAMPID();
    else
    {
        char cThreadName[32] = {0};
        prctl(PR_GET_NAME, (unsigned long)cThreadName);
        long int tid = syscall(SYS_gettid);
        logWarn(PLC, LOG_HEAD, "app ", itemID, " is not found! thread name: ", std::string(cThreadName), " tid: ", tid);
        return INVALID_AMPID;
    }
}

std::string AppList::GetBigIconPath(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appPkgInfo = apps.GetApp(app);

    if(appPkgInfo)
    {
        return appPkgInfo->getBigIcon();
    }
    else
        return std::string();
}

std::string AppList::GetSmallIconPath(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appPkgInfo = apps.GetApp(app);

    if(appPkgInfo)
    {
        return appPkgInfo->getSmallIcon();
    }
    else
        return std::string();
}

std::string AppList::GetSystemD(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appPkgInfo = apps.GetApp(app);

    if(appPkgInfo)
    {
        return appPkgInfo->getSystemdFile();
    }
    else
        return std::string();
}

std::string AppList::GetAppType(AMPID app)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appPkgInfo = apps.GetApp(app);

    if(appPkgInfo)
    {
        return appPkgInfo->getAppType();
    }
    else
        return std::string();
}

std::vector<AMPID> AppList::GetInstalledAppList(AMPID home)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    std::vector<AMPID> AMPIDList;
    E_SEAT homeSeat = GET_SEAT(home);
    AMPIDList = apps.GetApps([homeSeat](const App* app)->bool{
        if(app)
            return GET_SEAT(app->getAMPID())==homeSeat;
        else
            return false;
    });
    return std::move(AMPIDList);
}

std::vector<AMPID> AppList::GetAutoStartAppList()
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    std::vector<AMPID> AMPIDList;
    AMPIDList = apps.GetApps([](const App* app)->bool{
        if(app)
            return app->getIsAutoStart();
        else
            return false;
    });
    return std::move(AMPIDList);
}

bool AppList::IsAutoStart(AMPID app) const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appObj = apps.GetApp(app);
    if(appObj)
        return appObj->getIsAutoStart();
    else
        return false;
}

bool AppList::IsPlayStartupAnimation(AMPID app)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appObj = apps.GetApp(app);
    if(appObj)
        return !appObj->getDisableStartupAnimation();
    else
        return true;
}

bool AppList::IsIntent(AMPID app)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appObj = apps.GetApp(app);
    if(appObj)
    {
        auto strings = split(appObj->getSystemdFile(), ":");
        if(strings.size()>=2)
            return strings[0]=="intent";
        else
            return false;
    }
    else
        return false;
}

std::string AppList::GetIntentItemID(AMPID app)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appObj = apps.GetApp(app);
    if(appObj)
    {
        auto strings = split(appObj->getSystemdFile(), ":");
        if(strings.size()>=2)
            return strings[1];
        else
            return std::string();
    }
    else
        return std::string();
}

std::string AppList::GetIntentAction(AMPID app)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appObj = apps.GetApp(app);
    if(appObj)
    {
        auto strings = split(appObj->getSystemdFile(), ":");
        if(strings.size()>=3)
            return strings[2];
        else
            return std::string();
    }
    else
        return std::string();
}

std::string AppList::GetIntentData(AMPID app)const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    auto appObj = apps.GetApp(app);
    if(appObj)
    {
        auto strings = split(appObj->getSystemdFile(), ":");
        if(strings.size()>=4)
            return strings[3];
        else
            return std::string();
    }
    else
        return std::string();
}

bool AppList::IsLoadAppListTimeout()const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    return isLoadAppListTimeout;
}

void AppList::SetLoadAppListTimeout(bool isTimeout)
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    isLoadAppListTimeout = isTimeout;
}

bool AppList::IsCurrentLocaleReady()const
{
    std::unique_lock<std::mutex> lock(mutex4ListApp);

    ;
}
const std::string AppList::GetCurrentLocale()const
{
    return currentLocale.GetValue();
}

void AppList::SetCurrentLocale(const std::string& locale)
{
    currentLocale.SetValue(locale);
}

AppList* AppList::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static AppList m_Instance;
    return &m_Instance;
}
