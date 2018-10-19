/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppManager.cpp
/// @brief contains the implementation of class AppManager
///
/// Created by zs on 2016/7/13.
/// this file contains the implementation of class AppManager
///

#include "AppManager.h"
#include "ama_types.h"
#include "ama_audioTypes.h"
#include "HomePLCImplementation.h"
#include "LogContext.h"
#include "ILMClient.h"
#include "AppPower.h"
#include "Dependency.h"
#include "CAPINotifyEnum.h"
#include "ama_audioTypes.h"

#include "AppPower.h"
#include "PowerSDK.h"
#include "AppIS.h"
#include "AppIME.h"

#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/org/neusoft/AppMgrProxy.hpp>
#include <v0_1/org/neusoft/ImeIfProxy.hpp>
#include <map>
#include <cstring>
#include <cassert>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <ctime>
#include <mutex>
#include <signal.h>
#include <shutdown-0/libShutdown.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include <ilm_common.h>
#include <ilm_control.h>
#include <pthread.h>
#include <ILM/ILMException.h>
#include "ILM/Screen.h"

#include "AppAudioServiceImpl.h"
#include "StateManagerImpl.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

using namespace std;
#define gettid() syscall(__NR_gettid)
#define getpid() syscall(__NR_getpid)


// common api method call timeout
#define COMMONAPI_CALL_TIMEOUT			1000

//SM-------------START------------
//SM_handleReqSet setReqCallback;

int sKEYORDER=0;
::v0_1::org::neusoft::AppMgr::key_value app_key_value;

static pthread_t ScreenShotThread =0;// for check if bmp is write complete

//SM-------------END------------

inline void Alarm(std::condition_variable* CV)
{
    logVerbose("Alarm()--->IN");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CV->notify_all();
    logVerbose("Alarm()--->OUT");
}


// Common API
std::shared_ptr<v0_1::org::neusoft::AppMgrProxy<>> appManager;


void AudioFocusChangedNotify(const uint32_t connectID, const uint32_t oldFocusSta,const uint32_t newFocusSta,const uint32_t applyStreamID);

// this function is called if AMGR send pre-hide notify to APP
void PreHideNotify(uint32_t ampid, uint64_t id)
{
    AppManager::GetInstance()->NotifyPreHide(ampid, id);
}

// this function is called if AMGR send hide notify to APP
void HideNotify(uint32_t ampid)
{
    AppManager::GetInstance()->NotifyHide(ampid);
}

// this function is called if AMGR send stop notify to APP
void StopNotify(uint32_t ampid, const std::string &stopCMD)
{
    AppManager::GetInstance()->NotifyStop(ampid, stopCMD);
}

// this function is called if AMGR send resume notify to APP
void ResumeNotify(uint32_t ampid, uint64_t id)
{
    AppManager::GetInstance()->NotifyResume(ampid, id);
}

// this function is called if AMGR send pre-show notify to APP
void PreShowNotify(uint32_t ampid, uint32_t startArg, uint64_t id)
{
    AppManager::GetInstance()->NotifyPreShow(ampid, startArg, id);
}

// this function is called if AMGR send show notify to APP
void ShowNotify(uint32_t ampid)
{
    AppManager::GetInstance()->NotifyShow(ampid);
}

// this function is called if AMGR received early-intialize request
void EarlyInitializeNotify(uint32_t ampid)
{
    AppManager::GetInstance()->NotifyEarlyInitialize(ampid);
}

// this function is called if AMGR send APP started notify to APP
// APP started notify is sent because the preparation of booting
// a new APP is done.
void InitializeNotify(uint32_t ampid, uint32_t startArg, uint32_t isVisible, uint64_t id)
{
    AppManager::GetInstance()->NotifyInitialize(ampid, startArg, isVisible, id);
}

void InstalledPKGsChangedNotify(uint32_t relatedHome)
{
    AppManager::GetInstance()->NotifyInstalledPKGsChanged(relatedHome);
}

void PlayStartupAnimationNotify(uint32_t app)
{
    AppManager::GetInstance()->NotifyPlayStartupAnimation(app);
}

void StopStartupAnimationNotify(uint32_t app, uint64_t id)
{
    AppManager::GetInstance()->NotifyStopStartupAnimation(app, id);
}

void AnimationOverNotify(uint32_t app)
{
    AppManager::GetInstance()->NotifyAnimationOver(app);
}

void PowerStateChangeNotifyEx(uint32_t seat, uint32_t state)
{
    PowerSDK::GetInstance()->handlePowerStateChanged(static_cast<uint8_t>(seat), static_cast<ama_PowerState_e>(state));
}

void BroadcastShutdownNotify(uint32_t type)
{
    AppManager::GetInstance()->NotifyShutdown(type);
}

void SMorgDateNotify(::v0_1::org::neusoft::AppMgr::key_value _key_value)
{
	app_key_value = _key_value;
}

void BroadcastSMSetRequestNotify(uint32_t ampid_req, uint32_t ampid_reply, const std::string& key, const std::string& value)
{
    AppManager::GetInstance()->NotifySMSetRequest(ampid_req, ampid_reply, key, value);
}

void BroadcastSMSetDoneNotify(uint32_t ampid_req, uint32_t ampid_reply, const std::string& key, const std::string& value)
{
	//treat
    if(BROADCASTFLAG==ampid_req){
        StateManagerImpl::GetInstance()->handleBroadcast(key,value);
    }else{
        app_key_value[key].setAmpid_req(ampid_req);
    	app_key_value[key].setValue(value);
        AppManager::GetInstance()->NotifySMSetDone(ampid_req, ampid_reply, key, value);
    }
}

void BroadcastAMGRSetNotify(uint32_t state_type, const std::string& value)
{
    AppManager::GetInstance()->NotifyAMGRSet(state_type, value);
}

void ShowPopupNotify(uint32_t popupId, uint32_t reason)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("ShowPopupNotify(%d, %d)", popupId, reason));
}

void IMEFocusChangedNotify(uint32_t app)
{
    AppManager::GetInstance()->NotifyIMEFocusChanged(app);
}

void IMEClosedNotify(uint32_t app)
{
    AppManager::GetInstance()->NotifyIMEClosed(app);
}

void SwitchTitlebarVisibilityNotify(uint32_t titlebarID, uint32_t visibility, uint32_t style)
{
    AppManager::GetInstance()->NotifyTitlebarVisibilitySwitch(titlebarID, visibility, style);
}

void SwitchTitlebarDropDownStateNotify(uint32_t titlebarID, uint32_t isDropDown, uint32_t style)
{
    AppManager::GetInstance()->NotifyTitlebarDropDownStateSwitch(titlebarID, isDropDown, style);
}

void IntentNotify(uint32_t app, const std::string& action, const std::string& data)
{
    AppManager::GetInstance()->NotifyIntent(app, action, data);
}

void AppShareStateChangeNotify(uint32_t app, uint32_t seat, uint32_t state)
{
    AppManager::GetInstance()->NotifyAppShareStateChange(app, seat, state);
}

void AppStatusChangeNotify(uint32_t app, uint32_t state)
{
    AppManager::GetInstance()->NotifyAppStateChange(app, state);
}

void NSMRestartOKNotify()
{
    AppManager::GetInstance()->NotifyNSMRestartOK();
}

void GeneralNotify(uint32_t func, const std::vector<uint64_t>& u64Args, const std::vector<std::string>& stringArgs)
{
    logVerbose(SDK_AMGR,"AppManager::GeneralNotify-->>IN ,func:",func);
    switch(func)
    {
        case E_CAPI_NOTIFY_EARLYINITIALIZE:
            EarlyInitializeNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_INITIALIZE:
            InitializeNotify(u64Args[0], u64Args[1], u64Args[2], u64Args[3]);
            break;
        case E_CAPI_NOTIFY_INTENT:
            IntentNotify(u64Args[0], stringArgs[0], stringArgs[1]);
            break;
        case E_CAPI_NOTIFY_RESUME:
            ResumeNotify(u64Args[0], u64Args[1]);
            break;
        case E_CAPI_NOTIFY_PRESHOW:
            PreShowNotify(u64Args[0], u64Args[1], u64Args[2]);
            break;
        case E_CAPI_NOTIFY_SHOW:
            ShowNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_PREHIDE:
            PreHideNotify(u64Args[0], u64Args[1]);
            break;
        case E_CAPI_NOTIFY_HIDE:
            HideNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_STOP:
            StopNotify(u64Args[0], stringArgs[0]);
            break;
        case E_CAPI_NOTIFY_SHUTDOWN:
            BroadcastShutdownNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_INSTALLEDPACKAGESCHANGED:
            InstalledPKGsChangedNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_PLAYSTARTUPANIMATION:
            PlayStartupAnimationNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_STOPSTARTUPANIMATION:
            StopStartupAnimationNotify(u64Args[0], u64Args[1]);
            break;
        case E_CAPI_NOTIFY_ANIMATIONOVER:
            AnimationOverNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_POWERSTATECHANGEEX:
            PowerStateChangeNotifyEx(u64Args[0], u64Args[1]);
            break;
        case E_CAPI_NOTIFY_SM_SETREQUEST:
            BroadcastSMSetRequestNotify(u64Args[0], u64Args[1], stringArgs[0], stringArgs[1]);
            break;
        case E_CAPI_NOTIFY_SM_SETDONE:
            BroadcastSMSetDoneNotify(u64Args[0], u64Args[1], stringArgs[0], stringArgs[1]);
            break;
        case E_CAPI_NOTIFY_AMGR_SET:
            BroadcastAMGRSetNotify(u64Args[0], stringArgs[0]);
            break;
        case E_CAPI_NOTIFY_POPUP:
            ShowPopupNotify(u64Args[0], u64Args[1]);
            break;
        case E_CAPI_NOTIFY_IME_FOCUSCHANGED:
            IMEFocusChangedNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_APPS_IME_CLOSED:
            IMEClosedNotify(u64Args[0]);
            break;
        case E_CAPI_NOTIFY_AUDIO_FOCUSCHANGED:
            AudioFocusChangedNotify(u64Args[0], u64Args[1], u64Args[2], u64Args[3]);
            break;
        case E_CAPI_NOTIFY_TITLEBARVISIBILITYSWITCH:
            SwitchTitlebarVisibilityNotify(u64Args[0], u64Args[1], u64Args[2]);
            break;
        case E_CAPI_NOTIFY_TITLEBARDROPDOWNSTATESWITCH:
            SwitchTitlebarDropDownStateNotify(u64Args[0], u64Args[1], u64Args[2]);
            break;
        case E_CAPI_NOTIFY_APPSHARESTATECHANGE:
            AppShareStateChangeNotify(u64Args[0], u64Args[1], u64Args[2]);
            break;
        case E_CAPI_NOTIFY_APPSTATECHANGE:
            AppStatusChangeNotify(u64Args[0], u64Args[1]);
            break;
        case E_CAPI_NOTIFY_NSM_RESTART_OK:
            NSMRestartOKNotify();
            break;
        default:
            logWarn("GeneralNotify(): unexpected function number: ", func);
        break;
    };
}

AppManager::AppManager()
    : ampid(0)
    , initial(false)
    , isUseAnonymousAMPID(false)
    , seatHintOfAnonymousProcess(E_SEAT_NONE) // E_SEAT_NONE means no seat hint
    , appObj(nullptr)
    , appLogID()
    , playAnimation(nullptr)
    , stopAnimation(nullptr)
    , isPlayingStartupAnimation(false)
    , shutdownNotify(nullptr)
    , changeOfAudioFocusNotify(nullptr)
    , changeOfAudioFocusNotifyEx(nullptr)
    , titlebarVisibilitySwitchNotifyCB(nullptr)
    , titlebarDropDownNotifyCB(nullptr)
    , titleBarScreenShotCompleteCB(nullptr)
    , intentNotifyCB(nullptr)
    , appShareStateChangeNotifyCB(nullptr)
    , appStateCB(nullptr)
{

}

AppManager::~AppManager()
{
    LSD_Destroy();
}

static std::mutex appManagerInitializeMutex;

AppManager* AppManager::GetInstance()
{
    std::unique_lock<std::mutex> lock(appManagerInitializeMutex);
    static AppManager instance;
    return &instance;
}

void AppManager::Initialize()
{
    if(initial)
    {
        logWarn(SDK_AMGR,"AppManager::Initialize-->has already initial");
        return;
    }
    BootLog("AppManager::Initialize()-->IN");

    if(IsUseAnonymousAMPID())
        AnonymousInit();
    else
        NormalInit();

    //get newest powerState when SDK Init
    powerSDKInit();
    //init InformingSound
    informingSoundInit();
    imeInit();

    initial = true;
    logVerbose(SDK_AMGR,"AppManager::Initialize-->out");
    BootLog("AppManager::Initialize()-->OUT");
}
void AppManager::NormalInit()
{
    InitializeCAPI();

    InitializeLog();

    // connect to ILM
    InitializeILM();

    Subscribe();

	statMgr_GetOrgStates();

    InitializeLSD();
}

void AppManager::AnonymousInit()
{
    InitializeCAPI();

    AMPID ampid = GetAnonymousAMPID();
    if(ampid==INVALID_AMPID)
    {
        std::cerr << "no more anonymous AMPID, exit." << std::endl;
        exit(-1);
    }
    ampid = AMPID_ANMS(seatHintOfAnonymousProcess, GET_APPID(ampid));
    SetAMPID(ampid);

    InitializeLog();

    // connect to ILM
    InitializeILM();

    Subscribe();

	statMgr_GetOrgStates();

    InitializeLSD();
}

bool AppManager::IsUseAnonymousAMPID()const
{
    return isUseAnonymousAMPID;
}

void AppManager::UseAnonymousAMPID(bool usage)
{
    isUseAnonymousAMPID = usage;
}

void AppManager::SetSeatHint(E_SEAT seat)
{
    seatHintOfAnonymousProcess = seat;
}

void AppManager::InitializeLog()
{
    // initialize log
    const char* logID = nullptr;
    if(!appLogID.empty())
        logID = appLogID.c_str();
    else
        logID = GetLogID(ampid).c_str();
    LogInitialize(logID, appDescription.c_str(), /*IsEnableDLTLocalPrint()*/false); // TODO: fix this

    // initialize log-tag DT_APP_MGR
    RegisterContext(SDK_AMGR, "amgr");
    RegisterContext(SDK_AC, "SAC-");
    RegisterContext(SDK_ADBG, "SADB");
	RegisterContext(SDK_SET, "SET-");
    RegisterContext(SDK_PLC, "plc_");
    logVerbose(SDK_AMGR, "AppManager::Initialize-->in");
    logInfo(SDK_AMGR, "AppManager::Initialize(): ampid=", ToHEX(ampid));
}

extern std::shared_ptr<v0_1::org::neusoft::ImeIfProxy<>> gImeIfProxyPtr;
void AppManager::InitializeCAPI()
{
    CommonAPI::Runtime::setProperty("LogContext", "APLC");
    CommonAPI::Runtime::setProperty("LibraryBase", "AppMgr");

    string domain = "";
    string instance = "";
    string connection = "";

    // initialize CAPI
    shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    appManager = runtime->buildProxy<::v0_1::org::neusoft::AppMgrProxy>(domain,instance, connection);
    while (!appManager->isAvailable()){
        usleep(10);
    }
    // usleep(10);
    // gImeIfProxyPtr = runtime->buildProxy<::v0_1::org::neusoft::ImeIfProxy>(domain,instance, connection);
    // while (!gImeIfProxyPtr->isAvailable())
    // {
    //     usleep(10);
    // }
    std::cout << "appMgr CommonAPI service Available!" << std::endl;

    RequestEarlyInitialize();
}

void AppManager::InitializeILM()
{
    //ILMClient::Initialize();
    //logDebug(DT_APP_MGR, "AppManager::Initialize(): ILMClient is initialized!");
    //    if(!ILMClient::IsInitialized())
    //    {
    //        ILMClient::Initialize();
    //    }
    //    else
    //    {
    //        logWarn(DT_APP_MGR, "AppManager::Initialize(): ILMClient is already initialized!");
    //    }
}

void AppManager::Subscribe()
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::Subscribe()");

    // sm state changed notify
    appManager->getKey_value_mapEvent().subscribe(SMorgDateNotify);

    // general notify interface
    appManager->getGeneralNotifyEvent().subscribe(GeneralNotify);
}

void AppManager::InitializeLSD()
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::InitializeLSD()");

    // initialize libShutDown
    bool isOk = InitLSD();

    if(!isOk)
        logWarn(SDK_AMGR, LOG_HEAD, "failed in initializing libShutdown!");
}

static std::string MakeBootLog(const std::string& log)
{
    timespec ts = {0,0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    std::string prefix = formatText("time stamp: %d.%09d s app: %#x ", ts.tv_sec, ts.tv_nsec, GetAMPID());
    return (prefix+log);
}

void AppManager::BootLog(const std::string& log)
{
    bootLog.Operate([&log](std::vector<std::string>& bootLog){
        bootLog.push_back(MakeBootLog(log));
    });
}

unsigned int AppManager::GetAMPID() const
{
    return ampid;
}

void AppManager::SetAMPID(unsigned int app)
{
    ampid = app;
}

void AppManager::SetAppLogID(const char* appLogID)
{
    if(appLogID)
        this->appLogID = appLogID;
    else
        this->appLogID.clear();
}

void AppManager::SetAppDescription(const char* appDescription)
{
    if(appDescription)
        this->appDescription = appDescription;
    else
        this->appDescription.clear();
}

void AppManager::SetNotifyReceiver(PLCImplementation *app)
{
    this->appObj = app;
}

void AppManager::SetPlayAnimationNotifyCB(PlayAnimationNotifyCB f)
{
    playAnimation = f;
}
void AppManager::SetStopAnimationNotifyCB(StopAnimationNotifyCB f)
{
    stopAnimation = f;
}

void AppManager::SetAudioFocusChangeNotifyCB(changeOfAudioFocusCB f)
{
    changeOfAudioFocusNotify = f;
}

void AppManager::SetAudioFocusChangeNotifyCBEx(changeOfAudioFocusCBEx f)
{
    changeOfAudioFocusNotifyEx = f;
}

void AppManager::SetShutdownNotifyCB(ShutdownNotifyCB f)
{
    shutdownNotify = f;
}

void AppManager::SetIntentNotifyCB(IntentNotifyCB f)
{
    intentNotifyCB = f;
}

void AppManager::SetAppShareStateNotifyCB(AppShareStateChangeNotifyCB f)
{
    appShareStateChangeNotifyCB = f;
}

static IntentInformation GetIntentInfo(const v0_1::org::neusoft::AppMgr::IntentInfo& CAPIInfo)
{
    IntentInformation info;
    info.itemID = CAPIInfo.getItemID();
    info.action = CAPIInfo.getAction();
    info.data = CAPIInfo.getData();
    info.isQuiet = CAPIInfo.getIsQuiet();

    return std::move(info);
}

static std::vector<IntentInformation> GetIntentInfoVector(const v0_1::org::neusoft::AppMgr::IntentInfoArray& array)
{
    std::vector<IntentInformation> infoVector;
    for(auto& i: array)
        infoVector.push_back(GetIntentInfo(i));

    return std::move(infoVector);
}

static v0_1::org::neusoft::AppMgr::IntentInfo GetCAPIIntentInfo(const IntentInformation &info)
{
    v0_1::org::neusoft::AppMgr::IntentInfo intentInfo;
    intentInfo.setItemID(info.itemID);
    intentInfo.setAction(info.action);
    intentInfo.setData(info.data);
    intentInfo.setIsQuiet(info.isQuiet);

    return std::move(intentInfo);
}

static v0_1::org::neusoft::AppMgr::IntentInfoArray GetCAPIIntentInfoArray(const std::vector<IntentInformation>& intentInfoVector)
{
    v0_1::org::neusoft::AppMgr::IntentInfoArray array;

    for(auto& info: intentInfoVector)
        array.push_back(GetCAPIIntentInfo(info));

    return std::move(array);
}

std::vector<IntentInformation> AppManager::GetLastSource()
{
    DomainVerboseLog chatter(SDK_AMGR, __STR_FUNCTIONP__);

    v0_1::org::neusoft::AppMgr::IntentInfoArray array;
    std::vector<IntentInformation> lastSourceSet;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetRawLastSourceReq(callStatus, array, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::GetRawLastSource()!");

    lastSourceSet = GetIntentInfoVector(array);

    return std::move(lastSourceSet);
}

std::vector<IntentInformation> AppManager::GetCurrentSource()
{
    DomainVerboseLog chatter(SDK_AMGR, __STR_FUNCTIONP__);

    v0_1::org::neusoft::AppMgr::IntentInfoArray array;
    std::vector<IntentInformation> lastSourceSet;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetLastSourceReq(callStatus, array, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::GetLastSource()!");

    lastSourceSet = GetIntentInfoVector(array);

    return std::move(lastSourceSet);
}

void AppManager::SetCurrentSource(const std::vector<IntentInformation> &lastSourceSet)
{
    DomainVerboseLog chatter(SDK_AMGR, __STR_FUNCTIONP__);

    for(auto& source: lastSourceSet)
        logInfo(SDK_AMGR, LOG_HEAD, source.itemID, ":", source.action, ":", source.data, ":", source.isQuiet?"true":"false");

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->SetLastSourceReq(GetCAPIIntentInfoArray(lastSourceSet), callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::SetLastSource()!");
}

void AppManager::SetAppStateNotifyCB(AppStateCB f)
{
    appStateCB = f;
}

unsigned int AppManager::GetAnonymousAMPID()
{
    std::cout << "AppManager::GetAnonymousAMPID()--->IN" << std::endl;
    unsigned int ampid = 0;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);
    appManager->GetAnonymousAMPIDReq(callStatus, ampid, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        std::cerr << "failed calling AppManager::GetAnonymousAMPID()!" << std::endl;

    if(ampid==INVALID_AMPID)
        std::cerr << "AppManager::GetAnonymousAMPID(): invalid ampid!" << std::endl;

    std::cout << "AppManager::GetAnonymousAMPID()--->OUT" << std::endl;
    return ampid;
}

void AppManager::Intent(const std::string &itemID, const std::string &action, const std::string &data, bool isQuiet)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::Intent(%s, %s, %s, %s)", itemID.c_str(), action.c_str(), data.c_str(), isQuiet?"true":"false"));

    CommonAPI::CallStatus callStatus;
    appManager->StartAppReq(itemID, action, data, isQuiet, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::StartApp()!");
}

void AppManager::StopApp(unsigned int app)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::StopApp(%#x)", app));

    CommonAPI::CallStatus callStatus;
    appManager->StopAppReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::StopApp()!");
}

void AppManager::HideApp(unsigned int app)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::HideApp(%#x)", app));

    CommonAPI::CallStatus callStatus;
    appManager->HideAppReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::HideApp()!");
}

void AppManager::KillApp(unsigned int app)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::KillApp(%#x)", app));

    CommonAPI::CallStatus callStatus;
    appManager->KillAppReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::KillApp()!");
}

void AppManager::Respond2InitializeApp(unsigned long long id)
{
    CommonAPI::CallStatus callStatus;
    if(ampid!=INVALID_AMPID)
    {
        appManager->InitializeReq(ampid, id, callStatus);
        if (callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::Respond2InitializeApp()!");
    }
    else
        logError(SDK_AMGR, "AppManager::Respond2InitializeApp(): invalid AMPID!");
}

void AppManager::Respond2ResumeApp(unsigned long long id)
{
    CommonAPI::CallStatus callStatus;
    if(ampid!=INVALID_AMPID)
    {
        appManager->ResumeReq(ampid, id, callStatus);
        if (callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::Respond2ResumeApp()!");
    }
    else
        logError(SDK_AMGR, "AppManager::Respond2ResumeApp(): invalid AMPID!");
}

void AppManager::Respond2PreShowApp(unsigned long long id)
{
    CommonAPI::CallStatus callStatus;
    if(ampid!=INVALID_AMPID)
    {
        appManager->PreShowReq(ampid, id, callStatus);
        if (callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::Respond2PreShowApp()!");
    }
    else
        logError(SDK_AMGR, "AppManager::Respond2PreShowApp(): invalid AMPID!");
}

void AppManager::Respond2PreHideApp(unsigned long long id)
{
    CommonAPI::CallStatus callStatus;
    if(ampid!=INVALID_AMPID)
    {
        appManager->PreHideReq(ampid, id, callStatus);
        if (callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::Respond2PreHideApp()!");
    }
    else
        logError(SDK_AMGR, "AppManager::Respond2PreHideApp(): invalid AMPID!");
}

void AppManager::Respond2SystemShutdown()
{
    CommonAPI::CallStatus callStatus;
    if(ampid!=INVALID_AMPID)
    {
        appManager->ShutdownReq(ampid, callStatus);
        if (callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::Respond2SystemShutdown()!");
        else
            logInfo(SDK_AMGR, "AppManager::Respond2SystemShutdown()");
    }
    else
        logError(SDK_AMGR, "AppManager::Respond2SystemShutdown(): invalid AMPID!");
}

void AppManager::RequestEarlyInitialize()
{
//    CommonAPI::CallStatus callStatus;
//    if(ampid!=INVALID_AMPID)
//    {
//        appManager->EarlyInitializeReq(ampid, callStatus);
//        if (callStatus != CommonAPI::CallStatus::SUCCESS)
//            logError(DT_APP_MGR, "failed calling AppManager::RequestEarlyInitialize()!");
//        else
//            logDebug(DT_APP_MGR, "AppManager::RequestEarlyInitialize()");
//    }
//    else
//        logError(DT_APP_MGR, "AppManager::RequestEarlyInitialize(): invalid AMPID!");
}


::v0_1::org::neusoft::AppMgr::TempMap AppManager::GetLCDTemperature()
{
    logVerbose(SDK_AMGR, "AppManager::GetLCDTemperature()--> IN");


    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    ::v0_1::org::neusoft::AppMgr::TempMap retMap;
    // appManager->RequestLcdTemperature(callStatus,retMap,&info);
    //(CommonAPI::CallStatus &_internalCallStatus, AppMgr::TempMap &_temperatureMap, const CommonAPI::CallInfo *_info = nullptr)
    appManager->RequestLcdTemperature(callStatus,retMap,&info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::GetLCDTemperature()!");
    }


    logVerbose(SDK_AMGR, "AppManager::GetLCDTemperature()-->OUT");
    return retMap;
}

unsigned int AppManager::GetSurfaceID()
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);

    unsigned int surfaceID = 0;

    if(appManager)
    {
        appManager->GetSurfaceIDReq(ampid, callStatus, surfaceID, &callInfo);
        if(callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::GetSurfaceID()!");
        else
            logInfo(SDK_AMGR, "AppManager::GetSurfaceID() successed!");
    }
    else
        logError(SDK_AMGR, "AppManager::GetSurfaceID(): invalid app-manager pointer!");

    return surfaceID;
}

unsigned int AppManager::GetUnderLayerSurfaceID()
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);

    unsigned int surfaceID = 0;

    if(appManager)
    {
        appManager->GetUnderLayerSurfaceIDReq(ampid, callStatus, surfaceID, &callInfo);
        if(callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::GetUnderLayerSurfaceID()!");
        else
            logDebug(SDK_AMGR, "AppManager::GetUnderLayerSurfaceID() successed!");
    }
    else
        logError(SDK_AMGR, "AppManager::GetUnderLayerSurfaceID(): invalid app-manager pointer!");

    return surfaceID;
}

unsigned int AppManager::GetBKGLayerSurfaceID()
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);

    unsigned int surfaceID = 0;

    if(appManager)
    {
        appManager->GetBKGLayerSurfaceIDReq(ampid, callStatus, surfaceID, &callInfo);
        if(callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::GetBKGLayerSurfaceID()!");
        else
            logInfo(SDK_AMGR, "AppManager::GetBKGLayerSurfaceID() successed!");
    }
    else
        logError(SDK_AMGR, "AppManager::GetBKGLayerSurfaceID(): invalid app-manager pointer!");

    return surfaceID;
}


void AppManager::KeepAppAlive(bool alive)
{
    const unsigned int apmid=GetAMPID();
    logVerboseF(SDK_AMGR, "AppManager::KeepAppAlive apmid:%d, alive:%d -->IN",apmid,alive);

    CommonAPI::CallStatus callStatus;
    appManager->KeepAppAlive(apmid,alive,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::KeepAppAlive()!");
    }

    logVerbose(SDK_AMGR, "AppManager::KeepAppAlive-->OUT");
}

void AppManager::KeepAppAlive(const std::string& itemID, bool alive)
{
    logVerboseF(SDK_AMGR, "AppManager::KeepAppAlive itemID:%s, alive:%d -->IN",itemID,alive);
    const unsigned int apmid = GetAMPID(itemID);
    CommonAPI::CallStatus callStatus;
    appManager->KeepAppAlive(apmid,alive,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::KeepAppAlive()!");
    }

    logVerbose(SDK_AMGR, "AppManager::KeepAppAlive-->OUT");
}

void AppManager::PlayStartupAnimation(unsigned int app)
{
    std::unique_lock<std::mutex> lock(mutex4IsPlayingStartupAnimation);
    isPlayingStartupAnimation = true;
    std::thread waker(Alarm, &CV4IsPlayingStartupAnimation);
    waker.detach();
    CommonAPI::CallStatus callStatus;
    appManager->PlayStartupAnimationReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::PlayStartupAnimation()!");
}

void AppManager::StopStartupAnimation(unsigned int app)
{
    CommonAPI::CallStatus callStatus;
    appManager->StopStartupAnimationReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::StopStartupAnimation()!");
}

void AppManager::StopSuccess(unsigned int app, unsigned long long id)
{
    CommonAPI::CallStatus callStatus;
    appManager->StopSuccessReq(app, id, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::StopSuccess()!");
}

void AppManager::WaitForAnimationOver()
{
    std::unique_lock<std::mutex> lock(mutex4IsPlayingStartupAnimation);
    if(isPlayingStartupAnimation)
    {
        CV4IsPlayingStartupAnimation.wait(lock);
        isPlayingStartupAnimation = false;
    }
}

void AppManager::StopAnimation(unsigned int app)
{
    CommonAPI::CallStatus callStatus;
    appManager->StopAnimationReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::StopAnimation()!");
}

void AppManager::Move2BottomLayer()
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1); // wait forever until server return
    appManager->Move2BottomLayerReq(ampid, callStatus, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::Move2BottomLayer()!");
}

void AppManager::Return2HomeLayer()
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1); // wait forever until server return
    appManager->Return2HomeLayerReq(ampid, callStatus, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::Return2HomeLayer()!");
}

void AppManager::ForceMainSurfaceConfigured(unsigned int app)
{
    CommonAPI::CallStatus callStatus;
    appManager->ForceMainSurfaceConfiguredReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::ForceMainSurfaceConfigured()!");
}

void AppManager::LightUpScreenReq(E_SEAT seat)
{
    CommonAPI::CallStatus callStatus;
    appManager->LightUpScreenReq(seat, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::LightUpScreenReq()!");
}

std::vector<unsigned int> AppManager::GetAppList()
{
    std::vector<unsigned int> appList;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1); // wait forever until server return
    appManager->GetInstalledAppReq(ampid, callStatus, appList, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::GetAppList()!");

    return std::move(appList);
}

std::vector<AppPKGInfo> AppManager::GetAppInfoList()
{
    std::vector<AppPKGInfo> appInfoList;
    v0_1::org::neusoft::AppMgr::AppPropertiesArray appPropertiesArrayArray;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1); // wait forever until server return
    appManager->GetInstalledAppInfoReq(ampid, callStatus, appPropertiesArrayArray, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::GetAppInfoList()!");
    else
    {
        for(auto& appProperties : appPropertiesArrayArray)
        {
            AppPKGInfo appPKGInfo = {0, "", "", "", "", ""};
            appPKGInfo.ampid = appProperties.getAmpid();
            appPKGInfo.appName = appProperties.getAppName();
            appPKGInfo.itemID = appProperties.getItemID();
            appPKGInfo.bigIconPath = appProperties.getBigIconPath();
            appPKGInfo.smallIconPath = appProperties.getSmallIconPath();
            appPKGInfo.appType = appProperties.getAppType();
            appInfoList.push_back(appPKGInfo);
        }
    }
    return std::move(appInfoList);
}

AppPKGInfo AppManager::GetAppPKGInfo(unsigned int app)
{
    logVerbose(SDK_AMGR, "AppManager::GetAppPKGInfo(", ToHEX(app), ")--->IN");
    AppPKGInfo appPKGInfo = {0, "", "", "", "", ""};
    v0_1::org::neusoft::AppMgr::AppProperties appProperties;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1); // wait forever until server return
    appManager->GetAppPropertiesReq(app, callStatus, appProperties, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::GetAppPKGInfo()!");
    else
    {
        appPKGInfo.ampid = appProperties.getAmpid();
        appPKGInfo.appName = appProperties.getAppName();
        appPKGInfo.itemID = appProperties.getItemID();
        appPKGInfo.bigIconPath = appProperties.getBigIconPath();
        appPKGInfo.smallIconPath = appProperties.getSmallIconPath();
        appPKGInfo.appType = appProperties.getAppType();
        logInfoF(SDK_AMGR, "ampid=%#x name=%s itemID=%s bIconPath=%s sIconPath=%s", appPKGInfo.ampid, appPKGInfo.appName.c_str(), appPKGInfo.itemID.c_str(), appPKGInfo.bigIconPath.c_str(), appPKGInfo.smallIconPath.c_str());
    }

    logVerbose(SDK_AMGR, "AppManager::GetAppPKGInfo(", ToHEX(app), ")--->OUT");
    return appPKGInfo;
}

void AppManager::AppendBootLog(const std::string& log)
{
    CommonAPI::CallStatus callStatus;

    BootLog(log);

    SendBootLog();
}

void AppManager::SendBootLog()
{

    if(appManager)
    {
        bootLog.Operate([this, &appManager](std::vector<std::string>& bootLog){
            CommonAPI::CallStatus callStatus;
            if(!bootLog.empty())
            {
                appManager->SendBootLogReq(GetAMPID(), bootLog, callStatus);
                if(callStatus != CommonAPI::CallStatus::SUCCESS)
                    logError(SDK_AMGR, "failed calling AppManager::SendBootLog()!");
                else
                {
                    logDebug(SDK_AMGR, "AppManager::SendBootLog()");
                    bootLog.clear();
                }
            }
        });
    }
    else
        logError(SDK_AMGR, "AppManager::SendBootLog(): invalid app-manager pointer!");
}

std::string AppManager::GetLogID(unsigned int ampid)
{
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1);

    std::string logID;

    if(appManager)
    {
        appManager->GetLogIDReq(ampid, callStatus, logID, &callInfo);
        if(callStatus != CommonAPI::CallStatus::SUCCESS)
            std::cerr << "failed calling AppManager::GetLogID()!" << std::endl;
        else
            std::cout << "AppManager::GetLogID()" << std::endl;
    }
    else
        std::cerr << "AppManager::GetLogID(): invalid app-manager pointer!" << std::endl;

    return logID;
}

void AppManager::StartHome(unsigned int seat)
{
    CommonAPI::CallStatus callStatus;

    if(seat==0)
        seat = GET_SEAT(GetAMPID());

    if(appManager)
    {
        appManager->StartHomeReq(seat, callStatus);
        if(callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "failed calling AppManager::StartHome()!");
        else
            logInfo(SDK_AMGR, "AppManager::StartHome() successed!");
    }
    else
        logError(SDK_AMGR, "AppManager::StartHome(): invalid app-manager pointer!");
}

unsigned int AppManager::GetAMPID(const std::string& itemID)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::GetAMPID(%s)", itemID.c_str()));

    AMPID ampid = INVALID_AMPID;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo callInfo(-1); // wait forever until server return
    appManager->GetAMPIDReq(itemID, callStatus, ampid, &callInfo);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
        logError(SDK_AMGR, "failed calling AppManager::GetAppPKGInfo()!");

    return ampid;
}

//////////////////////////////////////////////////////////////////////////
// load broadcast from app manager

void AppManager::NotifyEarlyInitialize(unsigned int app)
{
    if (ampid != app)
        return;

    // handle the notify
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyEarlyInitialize(%#x)", app));

        logDebug(SDK_AMGR, "&appObj = ", reinterpret_cast<unsigned long>(appObj));
        if(appObj)
            appObj->OnNotifyEarlyInitialize();
    }
}

void AppManager::NotifyInitialize(unsigned int app, int startArg, bool isVisible, unsigned long long id)
{
    if (ampid != app)
        return;

    // handle the notify
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyInitialize(%#x)", app));

        logInfo(SDK_AMGR, "&appObj = ", reinterpret_cast<unsigned long>(appObj));
        if(appObj)
            appObj->OnNotifyInitialize(startArg, isVisible);

        // respond to app-manager
        Respond2InitializeApp(id);
    }
}

void AppManager::NotifyPreHide(unsigned int app, unsigned long long id)
{

    if (ampid != app)
        return;

    // handle the notify
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyPreHide(%#x)", app));
        if(appObj)
            appObj->OnNotifyPreHide();

        // respond to app-manager
        Respond2PreHideApp(id);
    }
}

void AppManager::NotifyHide(unsigned int app)
{
    if (ampid != app)
        return;

    // handle the notify
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyHide(%#x)", app));
        if(appObj)
            appObj->OnNotifyHide();
    }
}

void AppManager::NotifyResume(unsigned int app, unsigned long long id)
{
    if (ampid != app)
        return;

    // handle the notify
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyResume(%#x)", app));
        if(appObj)
            appObj->OnNotifyResume();

        // respond to app-manager
        Respond2ResumeApp(id);
    }
}

void AppManager::NotifyPreShow(unsigned int app, int startArg, unsigned long long id)
{
    if (ampid != app)
        return;

    // handle the notify
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyPreShow(%#x)", app));
        if(appObj)
            appObj->OnNotifyPreShow(startArg);

        // respond to app-manager
        Respond2PreShowApp(id);
    }
}

void AppManager::NotifyShow(unsigned int app)
{
    //check if is a common app
    SetCurrentApp(app);
    AddActiveApp(app);
    //save a screen shot
    //    if (ampid==0x1102||ampid==0x2102||ampid==0x3102)
    //    {
    //        bool retVal = SetScreenShot(app);
    //        if (retVal)
    //        { //notify active app changed
    //            unsigned int seatid     = GET_SEAT(app);

    //            appObj->OnNotifyActiveAppChange(seatid);

    //        }
    //    }
    if (ampid != app)
        return;
    else
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyShow(%#x)", app));
        if(appObj){
            appObj->OnNotifyShow();
        }
    }

}

void AppManager::NotifyStop(unsigned int app, const string &stopCMD)
{
    DelActiveApp(app);
    if (ampid != app)
        return;

    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyStop(%#x)", app));

        if(appObj)
            appObj->OnNotifyStop(stopCMD);

        // kill current app after stop-notify is handled.
        KillApp(app);
    }
}

void AppManager::NotifyInstalledPKGsChanged(unsigned int relatedHome)
{
    if(relatedHome!=ampid)
        return;

    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyInstalledPKGsChanged(%#x)", relatedHome));
        auto home = dynamic_cast<HomePLCImplementation *>(appObj);
        logDebug(SDK_AMGR, "AppManager::NotifyInstalledPKGsChanged(): &homeObj = ", home);
        if(home)
            home->OnInstalledPKGsChanged();
    }
}

void AppManager::NotifyPlayStartupAnimation(unsigned int app)
{
    if(playAnimation)
        playAnimation(app);
}

void AppManager::NotifyStopStartupAnimation(unsigned int app, unsigned long long id)
{
    if(stopAnimation)
        stopAnimation(app);
}

void AppManager::NotifyAnimationOver(unsigned int app)
{
    std::unique_lock<std::mutex> lock(mutex4IsPlayingStartupAnimation);
    isPlayingStartupAnimation = false;
    CV4IsPlayingStartupAnimation.notify_all();
}

void AppManager::NotifyAudioFocusChanged(uint32_t connectID,uint32_t oldFocusSta, uint32_t newFocusSta, uint32_t applyStreamID)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyAudioFocusChanged(%d,%d,%d,%d)",connectID,oldFocusSta,newFocusSta,applyStreamID));

    if(changeOfAudioFocusNotify){
        changeOfAudioFocusNotify(connectID, newFocusSta);
    }

    if(changeOfAudioFocusNotifyEx){
        changeOfAudioFocusNotifyEx(connectID,oldFocusSta,newFocusSta,applyStreamID);
    }
}

void AppManager::NotifyShutdown(unsigned int type)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyShutdown(%d)", type));

    if(shutdownNotify)
        shutdownNotify(type);

    Respond2SystemShutdown();
}

//SM-------------START------------

void AppManager::NotifyTitlebarVisibilitySwitch(uint32_t titlebarID, bool visibility, int style)
{
    if(titlebarID!=GetAMPID())
        return;

    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyTitlebarVisibilitySwitch(%#x, %s, %d)", titlebarID, visibility?"true":"false", style));

        if(titlebarVisibilitySwitchNotifyCB)
            titlebarVisibilitySwitchNotifyCB(visibility, style);
    }
}

void AppManager::NotifyTitlebarDropDownStateSwitch(uint32_t titlebarID, bool isDropDown, int style)
{
    if(titlebarID!=GetAMPID())
        return;

    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyTitlebarDropDownStateSwitch(%#x, %s, %d)", titlebarID, isDropDown?"true":"false", style));

        if(titlebarDropDownNotifyCB)
            titlebarDropDownNotifyCB(isDropDown, style);
    }
}

void AppManager::NotifySMSetRequest(unsigned int requester, unsigned int replyer, const std::string& key, const std::string& value)
{
    if(replyer!=GetAMPID())
        return;

    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifySMSetRequest(%#x, %#x, %s, %s)", requester, replyer, key.c_str(), value.c_str()));

        SMReqSetBroadcaster.NotifyAll([requester, replyer, &key, &value](SM_handleReqSet func){
            DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifySMSetRequest(%#x, %#x, %s, %s)", requester, replyer, key.c_str(), value.c_str()));

            logInfo(SDK_AMGR, "AppManager::NotifySMSetRequest(): callback(", reinterpret_cast<uint64_t>(func), ") is called.");
            func(requester, key, value);
        });
#if 0
        //handleRequestSet
        if(setReqCallback!=NULL)
        {
            setReqCallback(requester,key,value);
            logDebug(DT_APP_MGR,"AppManager.cpp::SMstateSetReply::setReqCallback::ampid_req::",requester,key," =",value);
        }
        else
        {
            logError(DT_APP_MGR,"AppManager.cpp::SMstateSetReply::setReqCallback is NULL! Please register by regStateHandler()");
        }
#endif
    }
}

void AppManager::NotifySMSetDone(unsigned int requester, unsigned int replyer, const std::string& key, const std::string& value)
{
    SMStateChangeBroadcaster.NotifyAll([requester, replyer, &key, &value](SM_handleStateChange func){
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifySMSetDone(%#x, %#x, %s, %s)", requester, replyer, key.c_str(), value.c_str()));

        logInfo(SDK_AMGR, LOG_HEAD, "callback(", reinterpret_cast<uint64_t>(func), ") is called.");
        func(requester, replyer, key, value);
    });
}

void AppManager::NotifyAMGRSet(unsigned int key, const std::string& value)
{
    AMGRStateChangeBroadcaster.NotifyAll([key, &value](AMGR_handleStateChange func){
        DomainVerboseLog chatter(SDK_AMGR, formatText(__STR_FUNCTION__+"(%#x, %s)", key, value.c_str()));

        logInfo(SDK_AMGR, LOG_HEAD, "callback(", reinterpret_cast<uint64_t>(func), ") is called.");
        func(key, value);
    });
}

void AppManager::NotifyIMEFocusChanged(unsigned int app)
{
    IMEFocusChangedBroadcaster.NotifyAll([app](std::function<void(unsigned int)> func){
        DomainVerboseLog chatter(SDK_AMGR, formatText(__STR_FUNCTION__+"(%#x)", app));

        logInfo(SDK_AMGR, "AppManager::NotifyIMEFocusChanged(): callback is called.");
        func(app);
    });
}

void AppManager::NotifyIMEClosed(unsigned int app)
{
    IMEClosedBroadcaster.NotifyAll([app](std::function<void(unsigned int)> func){
        DomainVerboseLog chatter(SDK_AMGR, formatText(__STR_FUNCTION__+"(%#x)", app));

        logInfo(SDK_AMGR, "AppManager::NotifyIMEClosed(): callback is called.");
        func(app);
    });
}

void AppManager::NotifyIntent(unsigned int app, const std::string& action, const std::string& data)
{
    if(app!=GetAMPID())
        return;

    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyIntent(%#x, %s)", app, action.c_str()));

        if(intentNotifyCB)
            intentNotifyCB(action, data);
        else
            logWarn(SDK_AMGR, "AppManager::NotifyIntent(): intent callback is null when a valid intent is arrival!");
    }
}

void AppManager::NotifyAppShareStateChange(unsigned int app, unsigned int seat, unsigned int state)
{
    if(appShareStateChangeNotifyCB)
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyAppShareStateChange(%#x, %d, %d)", app, seat, state));

        appShareStateChangeNotifyCB(app, static_cast<E_SEAT>(seat), state);
    }
}

void AppManager::NotifyAppStateChange(uint32_t app, uint32_t state)
{
    if(appStateCB)
    {
        DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::NotifyAppStateChange(%#x, %#x)", app, state));

        appStateCB(app, state);
    }

    AppAudioServiceImpl::GetInstance()->updateVisibleSta(app,IS_VISIBLE(state));
}

void AppManager::NotifyNSMRestartOK()
{
    FUNCTION_SCOPE_LOG_C(SDK_AMGR);

    OnLSDReboot();
}

void AppManager::statMgr_GetOrgStates(void)
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::statMgr_GetOrgStates()");

	uint32_t Ampid = AppManager::GetInstance()->GetAMPID();
	CommonAPI::CallStatus callStatus;
	appManager->get_orgStateInfo(Ampid,callStatus,app_key_value);
	if (callStatus != CommonAPI::CallStatus::SUCCESS)
            logError(SDK_AMGR, "AppManager.cpp::AppManager::statMgr_GetOrgStates::appManager->get_orgStateInfo failed!");
	logVerbose(SDK_AMGR,"AppManager.cpp::",Ampid,"AppManager::statMgr_GetOrgStates::",app_key_value.size(),"keys-->>IN");
	if(!app_key_value.empty())
	{
		for (::v0_1::org::neusoft::AppMgr::key_value::iterator it=app_key_value.begin(); it!=app_key_value.end(); ++it)
		{
			logInfoF(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_GetOrgStates::key:%s value:%s itemid_reply:%s ampid_req:0x%x\n",it->first.c_str(),it->second.getValue().c_str(),it->second.getItemid_reply().c_str(),it->second.getAmpid_req());
		}
	}
	logVerbose(SDK_AMGR,"AppManager.cpp::",Ampid,"AppManager::statMgr_GetOrgStates::",app_key_value.size(),"keys-->>OUT");
}

void AppManager::statMgr_SetReq(std::string key, std::string value)
{
    uint16_t ampid_req=AppManager::GetInstance()->GetAMPID();
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SetReq::ampid_req::",ampid_req,key,"=",value, "-->>IN");

    CommonAPI::CallStatus callStatus;

    //async fire&forget
    appManager->SM_SetReq(ampid_req,key,value,callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SetReq",key,"=",key,"CommonAPI Remote call failed!");
    }

	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SetReq::ampid_req::",ampid_req,key,"=",value, "-->>OUT");
}

void AppManager::statMgr_StateNotify(uint16_t ampid_req,std::string key, std::string value)
{
	uint16_t ampid_reply=AppManager::GetInstance()->GetAMPID();
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_StateNotify::ampid_req::",ampid_req,"ampid_reply::",ampid_reply,key,"=",value, "-->>IN");

	CommonAPI::CallStatus callStatus;

	//async fire&forget
	appManager->SM_SetState(ampid_req,key,value,callStatus);
	if (callStatus != CommonAPI::CallStatus::SUCCESS)
	{
		logError(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_StateNotify::SM_SetState::",key,"=",key,"CommonAPI Remote call failed!");
	}

	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_StateNotify::ampid_req::",ampid_req,"ampid_reply::",ampid_reply,key,"=",value, "-->>OUT");
}

void AppManager::statMgr_SMReqSet(SM_handleReqSet callback)
{
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SMReqSet-->>IN");
	//appManager->getSM_SetNotifyEvent().subscribe(callback);
    SMReqSetBroadcaster.Register(callback);
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SMReqSet-->>OUT");
}

void AppManager::statMgr_SMChangedNotify(SM_handleStateChange callback)
{
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SMChangedNotify-->>IN");
	//appManager->getSM_SetNotifyEvent().subscribe(callback);
    SMStateChangeBroadcaster.Register(callback);
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::statMgr_SMChangedNotify-->>OUT");
}

void AppManager::AMGR_changedNotify(AMGR_handleStateChange callback)
{
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::AMGR_changedNotify-->>IN");
	//appManager->getAMGR_SetNotifyEvent().subscribe(callback);
    AMGRStateChangeBroadcaster.Register(callback);
	logVerbose(SDK_AMGR,"AppManager.cpp::AppManager::AMGR_changedNotify-->>OUT");
}
//SM-------------END------------

void AppManager::requestAudioDeviceFocus(const ama_connectID_t connectID)
{
    logVerbose(SDK_AC,"AppManager::requestAudioDeviceFocus-->>IN connectID:", ToHEX(connectID));

    CommonAPI::CallStatus callStatus;
    appManager->requestAudioDeviceFocusReq(connectID, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::requestAudioDeviceFocus()!");
    }

    logVerbose(SDK_AC,"AppManager::requestAudioDeviceFocus-->>OUT");
}

void AppManager::releaseAudioDeviceFocus(const ama_connectID_t connectID)
{
    logVerbose(SDK_AC,"AppManager::releaseAudioDeviceFocus-->>IN connectID:", ToHEX(connectID));

    CommonAPI::CallStatus callStatus;
    appManager->releaseAudioDeviceFocusReq(connectID, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::releaseAudioDeviceFocus()!");
    }

    logVerbose(SDK_AC,"AppManager::releaseAudioDeviceFocus-->>OUT");
}

void AppManager::setStreamMuteState(const ama_streamID_t streamID,const bool isMute)
{
    logVerbose(SDK_AC,"AppManager::setStreamMuteState-->>IN streamID:", ToHEX(streamID)," isMute:",isMute);

    CommonAPI::CallStatus callStatus;
    appManager->setStreamMuteStateReq(streamID, isMute, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS){
        logError(SDK_AC, "failed calling AppManager::setStreamMuteState()!");
    }

    logVerbose(SDK_AC,"AppManager::setStreamMuteState-->>OUT");
}

void AppManager::ExpandTitlebar(const unsigned int titlebar)
{
    logVerbose(SDK_AMGR, "AppManager::ExpandTitlebar(", titlebar, ")--->IN");

    CommonAPI::CallStatus callStatus;
    appManager->ExpandTitlebarReq(titlebar, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::ExpandTitlebar()!");
    }

    logVerbose(SDK_AMGR, "AppManager::ExpandTitlebar(", titlebar, ")--->OUT");
}

void AppManager::ShrinkTitlebar(const unsigned int titlebar)
{
    logVerbose(SDK_AMGR, "AppManager::ShrinkTitlebar(", titlebar, ")--->IN");

    CommonAPI::CallStatus callStatus;
    appManager->ShrinkTitlebarReq(titlebar, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::ExpandTitlebar()!");
    }

    logVerbose(SDK_AMGR, "AppManager::ShrinkTitlebar(", titlebar, ")--->OUT");
}

void AppManager::SetTitlebarVisibility(unsigned int requester, bool visibility, int style)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::SetTitlebarVisibility(%#x, %s, %d)", requester, visibility?"true":"false", style));

    CommonAPI::CallStatus callStatus;
    appManager->SetTitlebarVisibilityReq(requester, visibility, style, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::SetTitlebarVisibility()!");
    }
}

bool AppManager::IsTitlebarVisible()const
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::IsTitlebarVisible()");

    bool isVisible = true;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->GetTitlebarVisibilityReq(GetAMPID(), callStatus, isVisible, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "AppManager::IsTitlebarVisible(): callStatus=", static_cast<unsigned int>(callStatus));
    }
    logInfo(SDK_AMGR, "AppManager::IsTitlebarVisible(): ", isVisible);

    return isVisible;
}

void AppManager::DropDownTitlebar(bool isDropDown, int style)
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::DropDownTitlebar()");

    CommonAPI::CallStatus callStatus;
    appManager->DropDownTitlebarReq(GetAMPID(), isDropDown, style, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::EnableTouch()!");
    }
}

bool AppManager::IsTitlebarDropDown()
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::IsTitlebarDropDown()");

    bool isDropDown = false;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->GetTitlebarDropDownStateReq(GetAMPID(), callStatus, isDropDown, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "AppManager::IsTitlebarDropDown(): callStatus=", static_cast<unsigned int>(callStatus));
    }
    logInfo(SDK_AMGR, "AppManager::IsTitlebarDropDown(): ", isDropDown);

    return isDropDown;
}

void AppManager::EnableTouch(bool enable)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::EnableTouch(%s)", enable?"true":"false"));

    CommonAPI::CallStatus callStatus;
    appManager->EnableTouchAppReq(GetAMPID(), enable, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::EnableTouch()!");
    }
}

void AppManager::SetUnderLayerCenter(bool isCenter)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::SetUnderLayerCenter(%s)", isCenter?"true":"false"));

    CommonAPI::CallStatus callStatus;
    appManager->SetUnderLayerCenterReq(GetAMPID(), isCenter, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::SetUnderLayerCenter()!");
    }
}

bool AppManager::IsUnderLayerCenter()
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::IsUnderLayerCenter()");

    bool isCenter = true;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->IsUnderLayerCenterReq(GetAMPID(), callStatus, isCenter, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "AppManager::IsUnderLayerCenter(): callStatus=", static_cast<unsigned int>(callStatus));
    }
    logDebug(SDK_AMGR, "AppManager::IsUnderLayerCenter(): ", isCenter);

    return isCenter;
}

std::vector<E_SEAT> AppManager::GetScreens()
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::GetScreens()");

    std::vector<unsigned int> rawScreens;
    std::vector<E_SEAT> screens;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->GetScreensReq(callStatus, rawScreens, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "AppManager::GetScreens(): callStatus=", static_cast<unsigned int>(callStatus));
    }
    logInfo(SDK_AMGR, "AppManager::GetScreenCount(): raw screen cout = ", rawScreens.size());

    for(auto& rawScreen: rawScreens)
        screens.push_back(static_cast<E_SEAT>(rawScreen));
    logInfo(SDK_AMGR, "AppManager::GetScreenCount(): screen cout = ", screens.size());

    return std::move(screens);
}

void AppManager::GetScreenSize(E_SEAT seat, unsigned int& width, unsigned int& height)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText(__STR_FUNCTION__+"(%u)", seat));

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    unsigned int _width = 0;
    unsigned int _height = 0;
    appManager->GetScreenSizeReq(seat, callStatus, _width, _height, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, LOG_HEAD, "callStatus=", static_cast<unsigned int>(callStatus));
    }
    if(_width && _height)
    {
        width = _width;
        height = _height;
    }
    logInfo(SDK_AMGR, LOG_HEAD, "width = ", width, " height = ", height);
    logInfo(SDK_AMGR, LOG_HEAD, "_width = ", _width, " _height = ", _height);
}

void AppManager::RemoveFromZOrder(unsigned int app)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText(__STR_FUNCTION__+"(%#x)", app));

    CommonAPI::CallStatus callStatus;
    appManager->RemoveAppFromZOrderReq(app, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, LOG_HEAD, "callStatus=", static_cast<unsigned int>(callStatus));
    }
}

void AppManager::AddExtraSurface(unsigned int surface, unsigned int layer)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText(__STR_FUNCTION__+"(%#x, %#x)", surface, layer));

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->AddExtraSurfaceReq(surface, layer, callStatus, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, LOG_HEAD, "callStatus=", static_cast<unsigned int>(callStatus));
    }
}

void AppManager::MarkTitlebarDropDownState(bool isDropDown)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::MarkTitlebarDropDownState(%s)", isDropDown?"true":"false"));

    CommonAPI::CallStatus callStatus;
    appManager->MarkTitlebarDropDownStateReq(GetAMPID(), isDropDown, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::MarkTitlebarDropDownState()!");
    }
}

void AudioFocusChangedNotify(const uint32_t connectID, const uint32_t oldFocusSta,const uint32_t newFocusSta,const uint32_t applyStreamID)
{
    logVerbose(SDK_AC,"AudioFocusChangedNotify-->>IN connectID:", ToHEX(connectID));
    logVerbose(SDK_AC,"oldFocusSta:",ToHEX(oldFocusSta)," newFocusSta:",ToHEX(newFocusSta)," applyStreamID:",ToHEX(applyStreamID));

    AppManager::GetInstance()->NotifyAudioFocusChanged(connectID,oldFocusSta,newFocusSta,applyStreamID);

    auto pAudioServiceImpl = static_cast<AppAudioServiceImpl*>(AppAudioServiceImpl::GetInstance());
    if(!IS_APP(GetAMPID())){
        if(NULL!=pAudioServiceImpl){
            pAudioServiceImpl->OnAudioDeviceFocusChangedNotify(connectID,(ama_focusSta_t)newFocusSta,(ama_streamID_t)applyStreamID);
        }
        return;
    }else if((GET_AMPID_FROM_CONNECTID(connectID)) == (AppManager::GetInstance()->GetAMPID())){
        if(NULL!=pAudioServiceImpl){
            pAudioServiceImpl->OnAudioDeviceFocusChangedNotify(connectID,(ama_focusSta_t)newFocusSta,(ama_streamID_t)applyStreamID);
        }
    }else{
        logVerbose(SDK_AC,"it's not AMPID of my app return ");
    }

    logVerbose(SDK_AC,"AudioFocusChangedNotify-->>OUT");
}

void AppManager::requestToDuckAudio(const ama_streamID_t streamID,const bool isDunck,const int durationsms,const int volume)
{
    logVerbose(SDK_AC,"AppManager::requestToDuckAudio-->>IN");

    CommonAPI::CallStatus callStatus;
    appManager->requestDuckAudio(streamID,isDunck,durationsms,volume,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::requestToDuckAudio()!");
    }

    logVerbose(SDK_AC,"AppManager::requestToDuckAudio-->>OUT");
}

void AppManager::requestToUseSpeaker(const ama_seatID_t seatID)
{
    logVerbose(SDK_AC,"AppManager::requestToUseSpeaker-->>IN seatID:", ToHEX(seatID));

    CommonAPI::CallStatus callStatus;
    appManager->requestToUseSpeakerReq(seatID, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::requestToUseSpeaker()!");
    }

    logVerbose(SDK_AC,"AppManager::requestToUseSpeaker-->>OUT");
}

void AppManager::abandonToUseSpeaker(const ama_seatID_t seatID)
{
    logVerbose(SDK_AC,"AppManager::abandonToUseSpeaker-->>IN seatID:", ToHEX(seatID));

    CommonAPI::CallStatus callStatus;
    appManager->abandonToUseSpeakerReq(seatID, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::abandonToUseSpeaker()!");
    }

    logVerbose(SDK_AC,"AppManager::abandonToUseSpeaker-->>OUT");
}

ama_AMPID_t AppManager::reqToGetLastApp(const ama_audioDeviceType_t deviceID)
{
    logVerbose(SDK_AC,"AppManager::reqToGetLastApp-->>IN deviceID:", ToHEX(deviceID));

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    uint32_t ampid = 0x0;
    appManager->getLastMediaAppReq(deviceID, callStatus, ampid,&info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::reqToGetLastApp()!");
        return 0x0;
    }

    logVerbose(SDK_AC,"AppManager::reqToGetLastApp-->>OUT");
    return (ama_AMPID_t)ampid;
}

bool AppManager::checkIsVolumeTypeActive(const int volumeType)
{
    logVerbose(SDK_AC,"AppManager::checkIsVolumeTypeActive-->>IN volumeType:", volumeType);

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    bool isActive = true;
    appManager->isVolumeTypeActive(volumeType, callStatus, isActive,&info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AC, "failed calling AppManager::checkIsVolumeTypeActive()!");
        return true;
    }

    logVerbose(SDK_AC,"AppManager::checkIsVolumeTypeActive-->>OUT");
    return isActive;
}

void AppManager::requestToSet(uint16_t ampid,int key,int value)
{
    logVerbose(SDK_SET,"AppManager::requestToSet()-->>IN");

    CommonAPI::CallStatus callStatus;
    appManager->setByAppManagerReq(ampid,key,value,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_SET, "failed calling AppManager::requestToSet()!");
    }

    logVerbose(SDK_SET,"AppManager::requestToSet()-->>OUT");
}

std::string AppManager::AMGRGetState(uint16_t ampid,int state_type)
{
	logVerbose(SDK_SET,"AppManager::AMGRGetState()-->>IN state_type:",state_type);

	CommonAPI::CallStatus callStatus;
	CommonAPI::CallInfo info(5000);
	std::string cur_state = "null";
	appManager->AMGRGetState(ampid,state_type,callStatus,cur_state,&info);
	if(callStatus!=CommonAPI::CallStatus::SUCCESS)
	{
		logError(SDK_SET, "failed calling AppManager::AMGRGetState()!");
	}

	logVerbose(SDK_SET,"AppManager::AMGRGetState()-->>OUT state_type:",state_type," =",cur_state);

	return cur_state;
}


bool isShownApp(unsigned int appid)
{
    if ( appid==0x01 ||appid==0x04 ||appid==0x05 ||appid==0x06 ||appid==0x07
         ||appid==0x08 ||appid==0x0A ||appid==0x0B ||appid==0x0C ||appid==0x16
         ||appid==0x17||appid==0x40)//"commonApp" and "home"  cj add iefish
        return true;
    else
        return false;
}

static int _mkdir(const char* path)
{
    if(path == NULL) {
        return -1;
    }

    char name[ 64 ];
    strcpy(name, path);
    int i, len = strlen(name);

    if (name[len - 1] != '/') {
        strcat(name, "/");
    }

    len = strlen(name);

    for (i = 1 ; i < len ; i++) {
        if (name[i] == '/') {
            name[i] = 0;

            if (access(name, F_OK) != 0) {
                if (mkdir(name, 0755) == -1) {
                    perror("mkdir error");
                    printf("mkdir(%s) : %d[%s]\n", name, errno, strerror(errno));
                    return -1;
                }
            }

            name[i] = '/';
        }
    }

    return 0;
}

unsigned int AppManager::SetIMEFocusChangedCB(const std::function<void(unsigned int)>& f)
{
    return IMEFocusChangedBroadcaster.Register(f);
}

unsigned int AppManager::SetIMEClosedCB(const std::function<void(unsigned int)>& f)
{
    return IMEClosedBroadcaster.Register(f);
}

void AppManager::SetTitlebarVisibilitySwitchNotifyCB(TitlebarVisibilitySwitchNotifyCB f)
{
    titlebarVisibilitySwitchNotifyCB=f;
}

void AppManager::SetTitlebarDropDownNotifyCB(TitlebarDropDownNotifyCB f)
{
    titlebarDropDownNotifyCB = f;
}

void AppManager::setTitleBarScreenShotCompleteCB(TitleBarScreenShotCompleteCB f)
{
    logVerbose(SDK_AMGR," setTitleBarScreenShotCompleteCB()--> IN  ");
    titleBarScreenShotCompleteCB=f;
    logVerbose(SDK_AMGR," setTitleBarScreenShotCompleteCB()--> OUT ");
}
TitleBarScreenShotCompleteCB AppManager::getTitleBarScreenShotCompleteCB()
{
    return titleBarScreenShotCompleteCB;
}
void AppManager::NotifyTitleBarScreenShotCompleteCB(unsigned int seatID)
{
    logVerbose(SDK_AMGR," NotifyTitleBarScreenShotCompleteCB()--> IN  seatID:",seatID);
    char picPath[64]={0};
    memcpy(picPath,screenShotList[seatID-1],strlen(screenShotList[seatID-1]));
    if(titleBarScreenShotCompleteCB){
        titleBarScreenShotCompleteCB(seatID,picPath);
    }
    logVerbose(SDK_AMGR," NotifyTitleBarScreenShotCompleteCB()--> OUT  seatID:",seatID);
}

//to mark if bmp write complete
static bool Flag_IVI=true;
static bool Flag_Rse1=true;
static bool Flag_Rse2=true;
static bool allComplete=false;
static bool threadIsRunning=false;
#define SCREEN_SHOT_BMP_SIZE 3686454

static unsigned long GetFileSize(char *path)
{
    unsigned long filesize = 0;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
       // return filesize;
    }else{
        filesize = statbuff.st_size;
        logInfo(SDK_AMGR," checkSSComplete printfFileSize: ",filesize);
    }

    return filesize;
}

static void *checkSSComplete(void *)//SS= Screen Shot
{
    logVerbose(SDK_AMGR," checkSSComplete()--> IN ");
    bool IVINotifyComplete=true;
    bool RSE1NotifyComplete=true;
    bool RSE2NotifyComplete=true;
    char tmpPicPath[64]={0};
    while( !allComplete)
    {
        allComplete=Flag_IVI&&Flag_Rse1&&Flag_Rse2;
        if (!Flag_IVI)
        {
            memset(tmpPicPath,0,64);
            AppManager::GetInstance()->GetScreenShot(E_SEAT_IVI,tmpPicPath);
            //logDebug(DT_APP_MGR," checkSSComplete tmpPicPath:",std::string(tmpPicPath));
            Flag_IVI = access(tmpPicPath, F_OK) == 0;
            Flag_IVI=Flag_IVI&(SCREEN_SHOT_BMP_SIZE<=GetFileSize(tmpPicPath));
                IVINotifyComplete=false;
        }
        else if (Flag_IVI&&!IVINotifyComplete)
        {
            AppManager::GetInstance()->NotifyTitleBarScreenShotCompleteCB(E_SEAT_IVI);
            logInfo(SDK_AMGR," checkSSComplete()  seatid:  E_SEAT_IVI  COMPLETE!");

            IVINotifyComplete=true;
        }
        else{}
        if (!Flag_Rse1)
        {
            memset(tmpPicPath,0,64);
            AppManager::GetInstance()->GetScreenShot(E_SEAT_RSE1,tmpPicPath);
          // logDebug(DT_APP_MGR," checkSSComplete tmpPicPath:",std::string(tmpPicPath));
            Flag_Rse1 = access(tmpPicPath, F_OK) == 0;
            Flag_Rse1=Flag_Rse1&(SCREEN_SHOT_BMP_SIZE<=GetFileSize(tmpPicPath));
            RSE1NotifyComplete=false;
        }
        else if (Flag_Rse1&&!RSE1NotifyComplete)
        {
            AppManager::GetInstance()->NotifyTitleBarScreenShotCompleteCB(E_SEAT_RSE1);
            logInfo(SDK_AMGR," checkSSComplete()  seatid:  E_SEAT_RSE1  COMPLETE!");
            RSE1NotifyComplete=true;
        }
        else{}
        if (!Flag_Rse2)
        {
            memset(tmpPicPath,0,64);
            AppManager::GetInstance()->GetScreenShot(E_SEAT_RSE2,tmpPicPath);
            //logDebug(DT_APP_MGR," checkSSComplete tmpPicPath:",std::string(tmpPicPath));
            Flag_Rse2 = access(tmpPicPath, F_OK) == 0;
            Flag_Rse2=Flag_Rse2&(SCREEN_SHOT_BMP_SIZE<=GetFileSize(tmpPicPath));
                RSE2NotifyComplete=false;
        }
        else if (Flag_Rse2&&!RSE2NotifyComplete)
        {
            AppManager::GetInstance()->NotifyTitleBarScreenShotCompleteCB(E_SEAT_RSE2);
            logInfo(SDK_AMGR," checkSSComplete()  seatid:  E_SEAT_RSE2  COMPLETE!");
            RSE2NotifyComplete=true;
        }
        else{}
    //    logDebug(DT_APP_MGR,"SSComplete ",Flag_IVI,Flag_Rse1,Flag_Rse2);
        usleep(1000*2);
    }
    logDebug(SDK_AMGR," checkSSComplete()  allComplete STOP thread!");
    pthread_join(ScreenShotThread,NULL);
    threadIsRunning=false;
    allComplete=false;
    logVerbose(SDK_AMGR," checkSSComplete()--> OUT ");

}
unsigned int AppManager::GetScreenIDBySeatID(unsigned int seatid)
{

    std::map<unsigned int ,unsigned int> ScreenSeatMap;
    auto ilmScreens = GetAllScreens();
    for(auto& screen: ilmScreens){
        Section _section = screen.GetRectangle();
        switch (_section.width)
        {
            case 1920://cluster
                ScreenSeatMap.insert(std::pair<unsigned int,unsigned int>(E_SEAT_TEST,screen.GetID()));
                logDebugF(SDK_AMGR,"GetScreenIDBySeatID() get width :%d  seatid:%d",_section.width,seatid);
                break;
            case 1280://ivi
                ScreenSeatMap.insert(std::pair<unsigned int,unsigned int>(E_SEAT_IVI,screen.GetID()));
                logDebugF(SDK_AMGR,"GetScreenIDBySeatID() get width :%d  seatid:%d",_section.width,seatid);
                break;
            case 2560://rse1
                ScreenSeatMap.insert(std::pair<unsigned int,unsigned int>(E_SEAT_RSE1,screen.GetID()));
                ScreenSeatMap.insert(std::pair<unsigned int,unsigned int>(E_SEAT_RSE2,screen.GetID()));
                logDebugF(SDK_AMGR,"GetScreenIDBySeatID() get width :%d  seatid:%d",_section.width,seatid);
                break;
            default:
                logDebugF(SDK_AMGR," GetScreenIDBySeatID() create get unknown screen.witdh: %d ",_section.width);
                break;
        }
    }
    return ScreenSeatMap[seatid];
}

void AppManager::CheckScreenShotNumber(const char* path)
{

    logVerbose(SDK_AMGR,"AppManager::CheckScreenShotNumber()--> IN  ");
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(path); //打开目录指针
    std::vector<char *> fileList;
    while ((dp = readdir(dirp)) != NULL) { //通过目录指针读目录
        if (0==strcmp(dp->d_name,".")||0==strcmp(dp->d_name,".."))
            continue;

        fileList.push_back(dp->d_name);
        // logDebugF(DT_APP_MGR," AppManager::CheckScreenShotNumber %s", dp->d_name );
    }
    std::string minPath="";
    std::string begin="";
    if (fileList.size()>36){
        minPath=std::string (*fileList.begin());
        begin =std::string(*fileList.begin());
        begin=begin.substr (16,30);
    }
    if (fileList.size()>36)
    {
        for (std::vector<char *>::iterator it = fileList.begin() ; it != fileList.end(); ++it){

            std::string current=std::string(*it);
            current=current.substr (16,30);
            int retVal=begin.compare(current);
            if (retVal>0){
                minPath=std::string(*it);
                begin=minPath.substr (16,30);
                logDebugF(SDK_AMGR,"begin: %s  current: %s value: %d ",begin.c_str(), current.c_str(),retVal);
            }
        }
        extern int errno;
        minPath=std::string(path)+"/"+minPath;
        int errorCode = remove(minPath.c_str());
        if (0!=errorCode){
            logErrorF(SDK_AMGR,"delete file : %s failed with errno: %d" ,minPath.c_str(),errno);
        }else
            logDebugF(SDK_AMGR,"delete file : %s success" ,minPath.c_str());
    }

    //
    (void) closedir(dirp); //关闭目录

    logVerbose(SDK_AMGR,"AppManager::CheckScreenShotNumber()--> OUT  ");
}

bool AppManager::SetScreenShot(unsigned int seatid)
{
    logVerbose(SDK_AMGR," SetScreenShot()--> IN  seatid:",seatid);
    char *bigBMP = NULL;
    unsigned int tmpseat=0;
     unsigned int x = 0;
     unsigned int y = 0;
     unsigned int w = 1280;
     unsigned int h = 720;
    time_t timep;
    time(&timep);
    struct tm* p = gmtime(&timep);
    const char *path = "/tmp/screenshot";
    _mkdir(path);
    if (seatid==1)
    {
      //  tmpseat=seatid;
        Flag_IVI=false;
    }
    else if (seatid==2)
    {
        Flag_Rse1=false;
       // tmpseat=seatid;
    }
    else if (seatid==3)
    {
     //   tmpseat=2;
        x=1280;
        y=0;
        Flag_Rse2=false;
    }
    else{;}
    tmpseat=GetScreenIDBySeatID(seatid);
    asprintf(&bigBMP, "%s/big_screenshot%d.%s", path,seatid,"bmp");
  //  asprintf(&bigBMP, "%s/big_screenshot%d_%04d%02d%02d%02d%02d%02d.%s", path,seatid,
   //          p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,"bmp");
    if (access(bigBMP, F_OK) == 0)//delete last bmp
    {
        remove(bigBMP);
    }

    //CheckScreenShotNumber(path);

    ilm_takeScreenshotEx(tmpseat, bigBMP, x, y, w, h, "bmp");
    memset(screenShotList[seatid-1],0,64);
    strcpy(screenShotList[seatid-1],bigBMP);
    logDebugF(SDK_AMGR," SetScreenShot 222tmpseat:%d  x:%d, y:%d, w:%d, h:%d,",tmpseat ,x, y, w, h);
    //start thread to check bmp write complete
    if (!threadIsRunning)//如果线程已经关闭 创建线程
    {
        int ret=0;
        ret=pthread_create(&ScreenShotThread,NULL,checkSSComplete,NULL);
        if(ret!=0){
            logError (SDK_AMGR,"Create countTime thread error!");
            threadIsRunning=false;
            return false;
        }
        else
        {
            threadIsRunning=true;
            logDebug(SDK_AMGR," checkSSComplete() CREATE thread!");
        }

    }

   // if(bigBMP != NULL)
    {
        ama_free(bigBMP);
    }

    logVerbose(SDK_AMGR," SetScreenShot()--> OUT  seatid:",seatid);
    return true;
}

void AppManager::GetScreenShot(int seatid ,char *picPath)
{
   // logVerbose(DT_APP_MGR," GetScreenShot --->IN");
    memcpy(picPath,screenShotList[seatid-1],strlen(screenShotList[seatid-1]));
   // logVerbose(DT_APP_MGR," GetScreenShot --->OUT");
}

char *AppManager::GetScreenShot(int seatid)
{
    return screenShotList[seatid-1];
}
void AppManager::refreshScreenShot(int seatid,char *picPath)
{
    logVerbose(SDK_AMGR," refreshScreenShot()--> IN  seatid:",seatid);
    SetScreenShot(seatid);
    //  GetScreenShot(seatid , picPath);
    logVerbose(SDK_AMGR," refreshScreenShot()--> OUT seatid:",seatid);
}

void AppManager::SetCurrentApp(unsigned int currAmpid)
{
    unsigned int seatid     = GET_SEAT(currAmpid);
    logVerbose(SDK_AMGR," SetCurrentApp --->IN   SEATID:",seatid,"APP:",currAmpid);
    m_CurrAmpidList[seatid-1]=currAmpid;
    logVerbose(SDK_AMGR," SetCurrentApp --->OUT  SEATID:",seatid,"APP:",currAmpid);
}
void AppManager::AddActiveApp(unsigned int currAmpid)
{
    logVerbose(SDK_AMGR," AddActiveApp --->IN  APP:",currAmpid);
    std::vector<unsigned int>::iterator  iter;
    int flag=0;
    for (iter=m_ActiveAppList.begin();iter!=m_ActiveAppList.end();iter++)
    {
        if (*iter==currAmpid)
        {
            flag=1;
            break;
        }
    }
    if (flag==0)
    {
        m_ActiveAppList.push_back(currAmpid);
    }


    logVerbose(SDK_AMGR," AddActiveApp --->OUT APP:",currAmpid);
}
void AppManager::DelActiveApp(unsigned int currAmpid)
{
    logVerbose(SDK_AMGR," DelActiveApp --->IN  APP:",currAmpid);
    std::vector<unsigned int>::iterator  iter;

    for (iter=m_ActiveAppList.begin();iter!=m_ActiveAppList.end();)
    {
        if (*iter==currAmpid){
            iter=m_ActiveAppList.erase(iter);
        }else{
            iter++;
        }
    }



    logVerbose(SDK_AMGR," DelActiveApp --->OUT APP:",currAmpid);
}

bool AppManager::isAlive(unsigned int ampid)
{
    logVerbose(SDK_AMGR," isAlive --->IN  APP:",ampid);
    std::vector<unsigned int>::iterator  iter;

    for (iter=m_ActiveAppList.begin();iter!=m_ActiveAppList.end();iter++)
    {
        if (*iter==ampid)
        {
            logInfo(SDK_AMGR,"AppManager::isAlive() get ActiveApp :",ampid," success!");
            logVerbose(SDK_AMGR," isAlive --->OUT APP:",ampid);
            return true;
        }

    }
    logInfo(SDK_AMGR,"AppManager::isAlive() get ActiveApp :",ampid," failed!");
    logVerbose(SDK_AMGR," isAlive --->OUT APP:",ampid);
    return false;
}


unsigned int  AppManager::GetCurrentApp(int seatid)
{
    return  m_CurrAmpidList[seatid-1];
}

void AppManager::StartShareApp(const unsigned int app, unsigned int seat, bool isMove)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::StartShareApp(%#x, %d, %s)", app, seat, isMove?"true":"false"));

    CommonAPI::CallStatus callStatus;
    appManager->StartShareAppReq(app, seat, isMove, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::StartShareApp()!");
    }
}

void AppManager::StopShareApp(const unsigned int app, E_SEAT seat)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::StopShareApp(%#x, %d)", app, seat));

    CommonAPI::CallStatus callStatus;
    appManager->StopShareAppReq(app, seat, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::StopShareApp()!");
    }
}

unsigned int AppManager::GetCurrentShare(const unsigned int seat)const
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::GetCurrentShare(%d)", seat));

    unsigned int app = 0;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->GetCurrentShareReq(seat, callStatus, app, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::GetCurrentShare()!");
    }

    return app;
}

std::vector<unsigned int> AppManager::GetSourceSurfaces()const
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::GetSourceSurfaces()"));

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    std::vector<unsigned int> sourceSurfaces;
    appManager->GetSourceSurfacesReq(GetAMPID(), callStatus, sourceSurfaces, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::GetSourceSurfaces()!");
    }

    return std::move(sourceSurfaces);
}

bool AppManager::GetWorkingLayers(std::vector<unsigned int> & workingLayers)const
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("AppManager::GetWorkingLayers()"));

    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    bool isMove = false;
    appManager->GetWorkingLayersReq(GetAMPID(), callStatus, workingLayers, isMove, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::GetWorkingLayers()!");
    }

    return isMove;
}

static void ShutdownCallback(LSD_ShutdownType type, void *userData)
{
    DomainVerboseLog chatter(SDK_AMGR, formatText("ShutdownCallback(type=%d)", type));

    BroadcastShutdownNotify(type);

    std::thread complete([](){
        try
        {
            logDebug(SDK_AMGR, "ShutdownCallback(): send shutdown complete...");
            LSD_ShutdownComplete();
            logDebug(SDK_AMGR, "ShutdownCallback(): send shutdown complete OK");
        }
        catch(std::exception& e)
        {
            logError(SDK_AMGR, LOG_HEAD, "exception is caught: ", (&e)->what());
        }
        catch(...)
        {
            logError(SDK_AMGR, LOG_HEAD, "unexpected exception is caught!");
        }
    });
    complete.detach();
}

bool AppManager::InitLSD()
{
    DomainVerboseLog chatter(SDK_AMGR, "InitLSD()");

    auto status = LSD_Init(ShutdownCallback, static_cast<LSD_ShutdownType>(LSD_SHUTDOWN_NORMAL|LSD_SHUTDOWN_FAST), 1000, nullptr);

    logInfo(SDK_AMGR, "InitLSD(): status = ", ToHEX(static_cast<unsigned int>(status)));


    return LSD_RESULT_SUCCESS == status;
}

void AppManager::OnLSDReboot()
{
    FUNCTION_SCOPE_LOG_C(SDK_AMGR);

    // destroy LSD
    logWarn(SDK_AMGR, LOG_HEAD, "destroy LSD...");
    auto status = LSD_Destroy();
    logWarn(SDK_AMGR, LOG_HEAD, "status = ", ToHEX(static_cast<unsigned>(status)));
    logWarn(SDK_AMGR, LOG_HEAD, "done.");

    // initialize LSD
    if(InitLSD())
        logWarn(SDK_AMGR, LOG_HEAD, "LSD initialized again.");
    else
        logError(SDK_AMGR, LOG_HEAD, "LSD failed initializing.");
}

std::vector<AMPID> AppManager::GetActiveAppList()const
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::GetActiveAppList()");

    std::vector<AMPID> list;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->GetActiveAppListReq(callStatus, list, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::GetActiveAppList()!");
    }

    return list;
}

AppStatus AppManager::GetAppState(AMPID app)
{
    DomainVerboseLog chatter(SDK_AMGR, "AppManager::GetAppState()");

    AppStatus appState = 0;
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    appManager->GetAppStateReq(app, callStatus, appState, &info);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::GetAppState()!");
    }

    return appState;
}

void AppManager::SetLCDTimerStop(unsigned int ampid, bool isStop)
{
    logVerbose(SDK_AMGR, "AppManager::SetLCDTimerStop( ampid:", ampid, "isStop: ",isStop,")-->IN");

    CommonAPI::CallStatus callStatus;
    appManager->SetRearTimerSTOP( ampid, isStop,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::SetLCDTimerStop()!");
    }

    logVerbose(SDK_AMGR, "AppManager::SetLCDTimerStop( ampid:", ampid, "isStop: ",isStop,")-->OUT");
}

void AppManager::SetLCDScreenOn(E_SEAT seat)
{
    logVerbose(SDK_AMGR, "AppManager::SetLCDScreenOn( seat:", seat,")-->IN");
    CommonAPI::CallStatus callStatus;
    appManager->SetScreenOnByApp( seat, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::SetScreenOnByApp()!");
    }
    logVerbose(SDK_AMGR, "AppManager::SetLCDScreenOn( seat:", seat,")-->OUT");
}
void AppManager::SetLCDPowerOff( )
{
    const unsigned int apmid=GetAMPID();
    unsigned int seat=GET_SEAT(apmid);
    logVerbose(SDK_AMGR, "AppManager::SetLCDPowerOff( ampid:", apmid,")-->IN");

    CommonAPI::CallStatus callStatus;
    appManager->SetRearPowerOff( seat, callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::SetLCDPowerOff()!");
    }

    logVerbose(SDK_AMGR, "AppManager::SetLCDPowerOff( ampid:", apmid,")-->OUT");
}
void AppManager::AppAwakeRequest(bool isAwake )
{
    const unsigned int apmid=GetAMPID();
    logVerboseF(SDK_AMGR, "AppManager::AppAwakeRequest apmid:%d, isAwake:%d -->IN",apmid,isAwake);

    CommonAPI::CallStatus callStatus;
    appManager->AppAwakeRequest(apmid,isAwake,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::AppAwakeRequest()!");
    }

    logVerbose(SDK_AMGR, "AppManager::AppAwakeRequest-->OUT");
}

void AppManager::SetBrightness(int brightness ,E_SEAT seatid)
{
 //   const unsigned int apmid=GetAMPID();

    logVerboseF(SDK_AMGR, "AppManager::SetBrightness seatid:%d, brightness:%d -->IN",seatid,brightness);

    CommonAPI::CallStatus callStatus;
    appManager->SetBrightness(seatid,brightness,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logError(SDK_AMGR, "failed calling AppManager::SetBrightness()!");
    }

    logVerbose(SDK_AMGR, "AppManager::SetBrightness-->OUT");
}
void AppManager::USBStateControll(ama_USBOperate op)
{
    logVerboseF(SDK_AMGR," USBStateControll()--> IN  ");
    CommonAPI::CallStatus callStatus;
    appManager->USBStateController((uint16_t)op,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logErrorF(SDK_AMGR, "failed calling AppManager::USBStateController()! op: 0x%04x",(uint16_t)op);
    }
    logVerboseF(SDK_AMGR," USBStateControll()--> OUT ");
}
void AppManager::getUSBDevState(int ampid)
{
    logVerboseF(SDK_AMGR," USBStateControll()--> IN  ");
    CommonAPI::CallStatus callStatus;
    appManager->getUSBDevState((uint16_t)ampid,callStatus);
    if(callStatus!=CommonAPI::CallStatus::SUCCESS)
    {
        logErrorF(SDK_AMGR, "failed calling AppManager::getUSBDevState()! op: 0x%04x",(uint16_t)ampid);
    }
    logVerboseF(SDK_AMGR," USBStateControll()--> OUT ");
}


 void AppManager::requestAlwaysOn(bool isAlwaysOn)
 {
     const unsigned int apmid=GetAMPID();
     logVerbose(SDK_AMGR, "AppManager::requestAlwaysOn( ampid:", apmid, "isAlwaysOn: ",isAlwaysOn,")-->IN");


     CommonAPI::CallStatus callStatus;
     appManager->requestAlwaysOn( apmid, isAlwaysOn,callStatus);
     if(callStatus!=CommonAPI::CallStatus::SUCCESS)
     {
         logError(SDK_AMGR, "failed calling AppManager::requestAlwaysOn()!");
     }

     logVerbose(SDK_AMGR, "AppManager::requestAlwaysOn( ampid:", apmid, "isAlwaysOn: ",isAlwaysOn,")-->OUT");
 }
