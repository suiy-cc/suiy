/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APPLIST
#define APPLIST

#include <mutex>
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include <unistd.h>

#include <libpminfo.h>
#include <libpmerror.h>
extern  "C" {
#include "pkgmgr.h"

};

//to read luc
#include <persistence_client_library.h>
#include <persistence_client_library_error_def.h>
#include <persistence_client_library_file.h>
#include <persistence_client_library_key.h>


/*
#include <libpkgmgr.h>
#include <libpmerror.h>
#include <libpmenv.h>
#include <libpmutils.h>
*/
//#include "tinyxml.h"
//#include "tinystr.h"
//#include <tinyxml2.h>

#include "ama_types.h"
#include "AMGRLogContext.h"
#include "App.h"
#include "FastBootInfo.h"
#include "rpc_proxy_def.h"
#include "COMMON/StringOperation.h"
#include "COMMON/ThreadSafeValueKeeper.h"
#include "KeyValuePairObserver.h"
#include "Capi.h"
#include "SMCtrl.h"
#include "AppCount.h"

struct xmlStruct
{
    int id;
    std::string value1;
    std::string value2;
    std::string value3;
    std::string value4;
    std::string value5;
    std::string value6;

};

#define APP_TPYE_CNT  6

enum E_PROCCESS_TYPE{
    E_PROCCESS_TYPE_APP = 0,
    E_PROCCESS_TYPE_SERVICE,
};

struct ItemType{
    E_PROCCESS_TYPE proccessType;
    std::string typeName;
};

bool operator==(const ItemType& l, const ItemType& r);

static const char* homeId[] = {
    "com.hs7.home.scr1",
    "com.hs7.home.scr2",
    "com.hs7.home.scr3"
};

#define MAX_APP_CNT 256

#define LOCALE_CHINESE 0
#define LOCALE_ENGLISH 1
static const char * localeArray[]={
    "zh_CN",
    "C"
};

class FastAppList
{
public:
    FastAppList(){};
    ~FastAppList(){};

    const App * GetApp(const AMPID app)const;
    const App * GetApp(const std::string &itemID)const;
    std::vector<AMPID> GetApps(std::function<bool(const App *)> filter)const;
    void AddApp(const App& app);
    void DeleteApp(const AMPID app);
    void DeleteApp(const std::string &itemID);
    void Clear();
    unsigned int Count()const;

private:
    std::unordered_map<AMPID, App> AMPID2app;
    std::unordered_map<std::string, App> itemID2app;

};

class AppList
{

public:
    AppList();
    ~AppList();
    void init();
    void clear();
    void fastInit();
    static AppList* GetInstance();

    /// @name operations
    /// @{
    void UpdateAppList();
    /// @}

    /// @name app pkg info getters
    /// @{
    bool IsValidApp(AMPID process);
    App GetApp(AMPID app)const;
    std::string GetAppName(AMPID app);
    std::string GetItemID(AMPID app);
    AMPID GetAMPID(const std::string& itemID)const;
    std::string GetPackageIcon(std::string packageID); // warning: not implemented yet
    std::string GetBigIconPath(AMPID app);
    std::string GetSmallIconPath(AMPID app);
    std::string GetSystemD(AMPID app);
    std::string GetAppType(AMPID app);
    std::vector<AMPID> GetInstalledAppList(AMPID home);
    std::vector<AMPID> GetAutoStartAppList();
    bool IsAutoStart(AMPID app)const;
    bool IsPlayStartupAnimation(AMPID app)const;
    bool IsIntent(AMPID app)const;
    std::string GetIntentItemID(AMPID app)const;
    std::string GetIntentAction(AMPID app)const;
    std::string GetIntentData(AMPID app)const;
    /// @}

    /// @name app pkg info getters
    /// @{
    bool IsLoadAppListTimeout()const;
    void SetLoadAppListTimeout(bool isTimeout);
    bool IsCurrentLocaleReady()const;
    const std::string GetCurrentLocale()const;
    void SetCurrentLocale(const std::string& locale);
    /// @}

public:
    FastBootInfo fastBootList;

private:
    void _updateAppList(const char* locale);
    void updateAppList();
    void waitForPKGMGR();
    void InitEOLInfo();
    bool waitForPMInfoDB();
    std::set<string> getDisabledApps();
    std::set<string> getAppsDisabledByEOL();
    std::set<string> getAppsDisabledByConfig();
    vector<App> PMInfoList2AppList(const ItemType &itemType, pminfo_item_list_t *typeItemList);
    void makeSMMap();
    void observeSYSLang();
    void registerUpdateCallBack();
    int  searchPkgIndex(int amaPid);
    static void OnSYSLANGChanged(const std::string& key, const SMValue& value, SMValueObserver::E_EVENT event);
    static void pkgUpdateCallback(const char *id, const char *info, void *user_data);

private:
    FastAppList apps;
    mutable std::mutex mutex4ListApp;
    EOLInfo eolInfo;
    AppCount appCount;
    bool isLoadAppListTimeout;
    ThreadSafeValueKeeper<std::string> currentLocale;
};



#endif // APPLIST

