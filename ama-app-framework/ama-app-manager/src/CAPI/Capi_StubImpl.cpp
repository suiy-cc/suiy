/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "AppProfileManager.h"
#include "StartService.h"
#include "AppShareManager.h"
#include "ProcessInfo.h"
#include "Capi_StubImpl.h"
#include "TaskDispatcher.h"
#include "PLCTask.h"
#include "SMTask.h"
#include "HMICTask.h"
#include "STHTask.h"
#include "AppList.h"
#include "cs_task.h"
#include "AudioTask.h"
#include "Capi.h"
#include "ama_audioTypes.h"
#include "ama_types.h"
#include "popup.h"
#include "ime_mgr.h"
#include "NotifyWaiter.h"
#include "BPMTask.h"
#include "BPMCtrl.h"
#include "PMCTask.h"
#include "PMCtrl.h"
#include "SMCtrl.h"
#include "LCDManager.h"
#include "USBManager.h"
#include "AMClient.h"
#include "ama_stateTypes.h"
#include "VideoLikeUI.h"
#include "PMCtrl.h"
#include "AudioSetting.h"
#include "AppAwakeMgr.h"
#ifdef ENABLE_DEBUG_MODE
#include "AppProfileManager.h"
#endif // ENABLE_DEBUG_MODE

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

extern std::shared_ptr<NeusoftAppMgrStubImpl> AppMgr_Service;

extern PopupManager gPopupMgr;

#ifdef CODE_FOR_EV
NeusoftAppMgrStubImpl::NeusoftAppMgrStubImpl()
{
    mStatesMap[E_SpeakerWho] = "1";
    mStatesMap[E_ActiveStreamForHardKey] = "5";
	mStatesMap[E_MODULE_EV_FCW] = "0";
	mStatesMap[E_MODULE_EV_AEB] = "0";
	mStatesMap[E_MODULE_EV_BSD] = "0";
	mStatesMap[E_MODULE_EV_LDW] = "0";
	mStatesMap[E_MODULE_EV_LKA] = "0";
	mStatesMap[E_MODULE_EV_DSM] = "0";
	mStatesMap[E_MODULE_EV_LDWS] = "0";
	mStatesMap[E_MODULE_EV_TRM] = "0";
	mStatesMap[E_MODULE_EV_HBA] = "0";
	mStatesMap[E_MODULE_EV_AP] = "0";
	mStatesMap[E_MODULE_EV_TSR] = "0";
	mStatesMap[E_MODULE_EV_LANG] = "0";
	mStatesMap[E_MODULE_EV_ALC] = "0";
	mStatesMap[E_MODULE_EV_ALCD] = "0";
    mStatesMap[E_MODULE_EV_SCM] = "0";
    mStatesMap[E_MODULE_EV_DSCU] = "0";
    mStatesMap[E_MODULE_EV_HUD] = "0";
    mStatesMap[E_MODULE_EV_DVR] = "0";
    mStatesMap[E_MODULE_EV_EFI] = "0";
    mStatesMap[E_MODULE_EV_MSMTD] = "0";
    mStatesMap[E_MODULE_EV_BMFS] = "0";
    mStatesMap[E_MODULE_EV_ALL] = "0";
	mStatesMap[E_MODULE_EV_CURISE] = "0";
	mStatesMap[E_MODULE_EV_TDMC] = "0";
    mStatesMap[E_RVC_REVERSE_SATATE] = "0";

}

#else
NeusoftAppMgrStubImpl::NeusoftAppMgrStubImpl()
{
    mStatesMap[E_SpeakerWho] = "1";
    mStatesMap[E_ActiveStreamForHardKey] = "5";
	mStatesMap[E_MODULE_FCW] = "0";
	mStatesMap[E_MODULE_AEB] = "0";
	mStatesMap[E_MODULE_MoPhCon] = "0";
	mStatesMap[E_MODULE_LDW] = "0";
	mStatesMap[E_MODULE_LKA] = "0";
	mStatesMap[E_MODULE_DSM] = "0";
	mStatesMap[E_MODULE_RIorAVS] = "0";
	mStatesMap[E_MODULE_RPE] = "0";
	mStatesMap[E_MODULE_HBA] = "0";
	mStatesMap[E_MODULE_AP] = "0";
	mStatesMap[E_MODULE_TBOX] = "0";
	mStatesMap[E_MODULE_LANG] = "0";
	mStatesMap[E_MODULE_WIFI] = "0";
	mStatesMap[E_MODULE_BROW] = "0";
	mStatesMap[E_MODULE_DMSCfg] = "0";
	mStatesMap[E_MODULE_DVR] = "0";
	mStatesMap[E_MODULE_AVS] = "0";
	mStatesMap[E_MODULE_CURISE] = "0";
	mStatesMap[E_MODULE_TDMC] = "0";
    mStatesMap[E_RVC_REVERSE_SATATE] = "0";

}
#endif

NeusoftAppMgrStubImpl::~NeusoftAppMgrStubImpl()
{
}


void NeusoftAppMgrStubImpl::StartAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _itemID, std::string _action, std::string _data, bool _isQuiet)
{
    logInfo(CAPI,"1-plc_appStartReq(_itemID:", _itemID, "_action:", _action.c_str(), "_isQuiet:", _isQuiet, ")");

    // get ampid
    AMPID app = AppList::GetInstance()->GetAMPID(_itemID);
    if(app==INVALID_AMPID)
    {
        logWarn(CAPI, "unknown item ID: ", _itemID);
        return;
    }

#ifdef ENABLE_DEBUG_MODE
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    if(procInfo->FindProc(app)==nullptr)
        AppProfileManager::GetInstance()->OnAppStart(app);
    procInfo->Unlock();
#endif // ENABLE_DEBUG_MODE

    IntentInfo intentInfo = {_action, _data, _isQuiet};
    DispatchIntentTask(app, intentInfo);
}

void NeusoftAppMgrStubImpl::StopAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    logInfo(CAPI,"2-plc_appStopReq(_ampid:", ToHEX(_ampid), ")");

    AMPID app = _ampid;

    auto pTask = std::make_shared<PLCTask>();
    pTask->SetType(PLC_CMD_STOP);
    pTask->SetAMPID(app);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

extern void Respond2InitializeNotify(unsigned int ampid, unsigned long long id);
void NeusoftAppMgrStubImpl::InitializeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id)
{
    DomainVerboseLog chatter(CAPI, formatText("InitializeReq(%#x)", _ampid));

    Respond2InitializeNotify(_ampid, _id);
}

extern void Respond2ResumeNotify(unsigned int ampid, unsigned long long id);
void NeusoftAppMgrStubImpl::ResumeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id)
{
    DomainVerboseLog chatter(CAPI, formatText("ResumeReq(%#x)", _ampid));

    Respond2ResumeNotify(_ampid, _id);
}

extern void Respond2PreShowNotify(unsigned int ampid, unsigned long long id);
void NeusoftAppMgrStubImpl::PreShowReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id)
{
    DomainVerboseLog chatter(CAPI, formatText("PreShowReq(%#x)", _ampid));

    Respond2PreShowNotify(_ampid, _id);
}

extern void Respond2PreHideNotify(unsigned int ampid, unsigned long long id);
void NeusoftAppMgrStubImpl::PreHideReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id)
{
    logInfo(CAPI, "PreHideReq(", ToHEX(_ampid), ")--->IN");

    Respond2PreHideNotify(_ampid, _id);

    logDebug(CAPI, "PreHideReq(", ToHEX(_ampid), ")--->OUT");
}

void NeusoftAppMgrStubImpl::HideAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    logInfo(CAPI,"7-plc_appHideReq(_ampid:", ToHEX(_ampid), ")");

    AMPID app = _ampid;

    auto pTask = std::make_shared<PLCTask>();
    pTask->SetType(PLC_CMD_HIDE);
    pTask->SetAMPID(app);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

void NeusoftAppMgrStubImpl::KillAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::KillAppReq(%#x)", _ampid));

    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetType(PLC_CMD_KILL);
    plcTask->SetAMPID(_ampid);

    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}

void NeusoftAppMgrStubImpl::GetInstalledAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetInstalledAppReqReply_t _reply)
{
    logDebug(CAPI,"GetInstalledAppReq(", ToHEX(_requester), ")-->IN");

    auto ampidList = AppList::GetInstance()->GetInstalledAppList(_requester);

    _reply(ampidList);

    logDebug(CAPI,"GetInstalledAppReq(", ToHEX(_requester), ")-->OUT");
}

static v0_1::org::neusoft::AppMgr::AppProperties GetCAPIAppProperties(const AppPKGInfo& apppkgInfo)
{
    v0_1::org::neusoft::AppMgr::AppProperties appProperties;
    appProperties.setAmpid(apppkgInfo.ampid);
    appProperties.setAppName(apppkgInfo.appName);
    appProperties.setItemID(apppkgInfo.itemID);
    appProperties.setBigIconPath(apppkgInfo.bigIconPath);
    appProperties.setSmallIconPath(apppkgInfo.smallIconPath);
    appProperties.setAppType(apppkgInfo.appType);

    return std::move(appProperties);
}

static v0_1::org::neusoft::AppMgr::AppPropertiesArray GetCAPIAppPropertiesArray(const std::vector<AppPKGInfo>& appPKGInfos)
{
    v0_1::org::neusoft::AppMgr::AppPropertiesArray array;

    for(auto& info: appPKGInfos)
    {
        array.push_back(GetCAPIAppProperties(info));
    }

    return std::move(array);
}

void NeusoftAppMgrStubImpl::GetInstalledAppInfoReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetInstalledAppInfoReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, __STR_FUNCTIONP__);

    std::vector<AppPKGInfo> list;
    auto data = ZIP_ARG(_requester, list);

    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetType(PLC_QUERY_APP_INFO_LIST);
    plcTask->SetUserData(TO_USERDATA(data));
    SendQueryTaskNReplyAtTaskDone(plcTask, [_reply, data](std::shared_ptr<Task> pTask){
        logInfo(CAPI, LOG_HEAD, "replying...");

        auto plcTask = std::dynamic_pointer_cast<PLCTask>(pTask);
        if(!plcTask)
            _reply(::v0_1::org::neusoft::AppMgr::AppPropertiesArray());
        else
            _reply(GetCAPIAppPropertiesArray(REF_ARG(pTask, decltype(data), 1)));
        logInfo(CAPI, LOG_HEAD, "done.");
    });
}

void NeusoftAppMgrStubImpl::GetAppPropertiesReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, GetAppPropertiesReqReply_t _reply)
{
    logDebug(CAPI,"GetAppPropertiesReq(", ToHEX(_app), ")");

    ::v0_1::org::neusoft::AppMgr::AppProperties appProperties;
    appProperties.setAmpid(_app);
    appProperties.setAppName(AppList::GetInstance()->GetAppName(_app));
    appProperties.setItemID(AppList::GetInstance()->GetItemID(_app));
    appProperties.setBigIconPath(AppList::GetInstance()->GetBigIconPath(_app));
    appProperties.setSmallIconPath(AppList::GetInstance()->GetSmallIconPath(_app));
    appProperties.setAppType(AppList::GetInstance()->GetAppType(_app));

    logInfoF(CAPI, "GetAppPropertiesReq(): ampid=%#x name=%s itemID=%s bIcon=%s sIcon=%s appType=%s ", appProperties.getAmpid(), appProperties.getAppName().c_str(), appProperties.getItemID().c_str(), appProperties.getBigIconPath().c_str(), appProperties.getSmallIconPath().c_str(), appProperties.getAppType().c_str());

    _reply(appProperties);
}

void NeusoftAppMgrStubImpl::PlayStartupAnimationReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::PlayStartupAnimationReq(", ToHEX(_ampid), ")--->IN");

    auto pTask = std::make_shared<HMICTask>();
    pTask->SetType(HMIC_NOTIFY_ANIMATION_STARTUP_PLAY);
    pTask->SetAMPID(_ampid);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI, "NeusoftAppMgrStubImpl::PlayStartupAnimationReq(", ToHEX(_ampid), ")--->OUT");
}

void NeusoftAppMgrStubImpl::StopStartupAnimationReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::StopStartupAnimationReq(", ToHEX(_ampid), ")--->IN");

    auto pTask = std::make_shared<HMICTask>();
    pTask->SetType(HMIC_NOTIFY_ANIMATION_STARTUP_STOP);
    pTask->SetAMPID(_ampid);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI, "NeusoftAppMgrStubImpl::StopStartupAnimationReq(", ToHEX(_ampid), ")--->OUT");
}

extern void StopSuccessReq(unsigned int ampid, unsigned long long id);
void NeusoftAppMgrStubImpl::StopSuccessReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint64_t _id)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::StopSuccessReq(", ToHEX(_ampid), ")--->IN");

    ::StopSuccessReq(_ampid, _id);

    logVerbose(CAPI, "NeusoftAppMgrStubImpl::StopSuccessReq(", ToHEX(_ampid), ")--->OUT");
}

void NeusoftAppMgrStubImpl::StopAnimationReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::StopAnimationReq(", ToHEX(_ampid), ")--->IN");

    AnimationOverNotify(_ampid);

    logVerbose(CAPI, "NeusoftAppMgrStubImpl::StopAnimationReq(", ToHEX(_ampid), ")--->OUT");
}

void NeusoftAppMgrStubImpl::GetStartupScreenAvailibilityReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::GetStartupScreenAvailibilityReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetStartupScreenAvailibilityReq(#%x)", _ampid));

    _reply(AppList::GetInstance()->IsPlayStartupAnimation(_ampid));
}

void NeusoftAppMgrStubImpl::GetAMPIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _itemID, v0_1::org::neusoft::AppMgrStub::GetAMPIDReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetAMPIDReq(%s)", _itemID.c_str()));

    _reply(AppList::GetInstance()->GetAMPID(_itemID));
}

void NeusoftAppMgrStubImpl::LightUpScreenReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::LightUpScreenReq(%d)", _seat));

    auto pmcTask = make_shared<PMCTask>();
    pmcTask->setPowerSignal(E_RSE_POWER_ON);
    pmcTask->SetType(HANDLE_LCD_SIGNAL);
    pmcTask->setSeatID(static_cast<E_SEAT >(_seat));
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);



    auto pmcTask1 = make_shared<PMCTask>();
    pmcTask1->setPowerSignal(E_RSE_SCREEN_ON);
    pmcTask1->SetType(HANDLE_LCD_SIGNAL);
    pmcTask1->setSeatID(static_cast<E_SEAT >(_seat));
    TaskDispatcher::GetInstance()->Dispatch(pmcTask1);

}

void NeusoftAppMgrStubImpl::ShutdownReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::ShutdownReq(%#x)", _ampid));

    AMPID app = _ampid;

    auto pTask = std::make_shared<PLCTask>();
    pTask->SetType(PLC_NOTIFY_APP_SHUTDOWN_OK);
    pTask->SetAMPID(app);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

void NeusoftAppMgrStubImpl::GetAnonymousAMPIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, GetAnonymousAMPIDReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, "NeusoftAppMgrStubImpl::GetAnonymousAMPIDReq()");

    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    AMPID ampid = procInfo->GetAnonymousAMPID();
    procInfo->Unlock();

    _reply(ampid);
}

void NeusoftAppMgrStubImpl::ForceMainSurfaceConfiguredReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::ForceMainSurfaceConfiguredReq(%#x)", _app));

    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetAMPID(_app);
    hmicTask->SetType(HMIC_NOTIFY_APP_START);
    hmicTask->SetPriority(GET_APPID(_app)==E_APPID_HOME?E_TASK_PRIORITY_HIGH:E_TASK_PRIORITY_NORMAL);

    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::EnableTouchAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, bool _enable)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::EnableTouchAppReq(%#x)", _app));

    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetAMPID(_app);
    hmicTask->SetUserData(_enable);
    hmicTask->SetType(HMIC_CMD_ENABLE_TOUCH_APP);

    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::EnableChildSafetyLockReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, bool _enable, std::string _pngPath)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::EnableChildSafetyLockReq(%d, %s, %s)", _seat, _enable?"true":"false", _pngPath.c_str()));

    SendEnableChildSafetyLock(static_cast<E_SEAT>(_seat), _enable, _pngPath);
}

void NeusoftAppMgrStubImpl::EnableTouchScreenReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, bool _enable, std::string _pngPath)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::EnableTouchScreenReq(%d, %s, %s)", _seat, _enable?"true":"false", _pngPath.c_str()));

    SendEnableTouchScreen(static_cast<E_SEAT>(_seat), _enable, _pngPath);
}

void NeusoftAppMgrStubImpl::SetUnderLayerCenterReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, bool _isCenter)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::SetUnderLayerCenterReq(%d, %s)", _ampid, _isCenter?"true":"false"));

    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appObj = procInfo->FindApp(_ampid);
    if(appObj)
    {
        auto HMIUI = dynamic_pointer_cast<VideoLikeUI>(appObj->GetUI());
        if(HMIUI)
        {
            HMIUI->SetUnderLayerCenter(_isCenter);
            logDebug(CAPI, "NeusoftAppMgrStubImpl::SetUnderLayerCenterReq(): app ", ToHEX(_ampid), "'s under-layer set to ", _isCenter?" ":"not", " center");
        }
    }
    procInfo->Unlock();
}

void NeusoftAppMgrStubImpl::IsUnderLayerCenterReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::IsUnderLayerCenterReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::IsUnderLayerCenterReq(%d)", _ampid));

    bool isFound = false;
    bool isCenter = false;
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appObj = procInfo->FindApp(_ampid);
    if(appObj)
    {
        auto HMIUI = dynamic_pointer_cast<VideoLikeUI>(appObj->GetUI());
        if(HMIUI)
        {
            isFound = true;
            isCenter = HMIUI->IsUnderLayerCenter();
            logDebug(CAPI, "NeusoftAppMgrStubImpl::IsUnderLayerCenterReq(): app ", ToHEX(_ampid), "'s under-layer is ", isCenter?" ":"not", " center");
        }
    }
    procInfo->Unlock();

    if(!isFound)
        logWarn(CAPI, "NeusoftAppMgrStubImpl::IsUnderLayerCenterReq(): app ", ToHEX(_ampid), " is not found!");

    _reply(isCenter);
}

void NeusoftAppMgrStubImpl::GetScreensReq(const std::shared_ptr<CommonAPI::ClientId> _client, v0_1::org::neusoft::AppMgrStub::GetScreensReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, "NeusoftAppMgrStubImpl::GetScreensReq()");

    GetAvailableScreens([_reply](std::vector<E_SEAT> screens){
        logInfo(CAPI, "NeusoftAppMgrStubImpl::GetScreensReq(): screen status is sent back.");
        std::vector<unsigned int> uintScreens;
        for(auto& screen: screens)
            uintScreens.push_back(static_cast<unsigned int>(screen));
        _reply(uintScreens);
    });
}

void NeusoftAppMgrStubImpl::GetScreenSizeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, GetScreenSizeReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%u)", _seat));

    unsigned int width = 0;
    unsigned int height = 0;
    auto data = ZIP_ARG(_seat, width, height);
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetType(HMIC_QUERY_GET_SCREEN_SIZE);
    hmicTask->SetUserData(TO_USERDATA(data));
    SendQueryTaskNReplyAtTaskDone(hmicTask, [_reply, data](std::shared_ptr<Task> task){
        logInfo(CAPI, LOG_HEAD, "replying: width: ", REF_ARG(task, decltype(data), 1), " height: ", REF_ARG(task, decltype(data), 2));

        _reply(REF_ARG(task, decltype(data), 1), REF_ARG(task, decltype(data), 2));
    });
}

void NeusoftAppMgrStubImpl::RemoveAppFromZOrderReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%#x)", _ampid));

    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetAMPID(_ampid);
    plcTask->SetType(PLC_CMD_REMOVE_APP_FROM_ZORDER);

    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}

void NeusoftAppMgrStubImpl::AddExtraSurfaceReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _surface, uint32_t _layer, v0_1::org::neusoft::AppMgrStub::AddExtraSurfaceReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%#x, %#x)", _surface, _layer));

    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetType(HMIC_CMD_ADD_EXTRA_SURFACE);
    hmicTask->SetSurfaceID(_surface);
    hmicTask->SetUserData(_layer);

    SendQueryTaskNReplyAtTaskDone(hmicTask, [_reply](std::shared_ptr<Task> task){
        logInfo(CAPI, LOG_HEAD, "replying...");
        _reply();
        logInfo(CAPI, LOG_HEAD, "done.");
    });
}

// apps call this function to initialize their local SM copy
void NeusoftAppMgrStubImpl::get_orgStateInfo(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, get_orgStateInfoReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%#x)", _ampid));

    auto replyer = new std::function<void(::v0_1::org::neusoft::AppMgr::key_value)>;
    *replyer = _reply;

    auto smTask = std::make_shared<SMTask>();
    smTask->SetType(E_SM_TASK_QUERY_SNAPSHOT_SM);
    smTask->SetUserData(reinterpret_cast<unsigned long long>(replyer), [](unsigned long long data){delete reinterpret_cast<std::function<void(::v0_1::org::neusoft::AppMgr::key_value)>*>(data);});

    TaskDispatcher::GetInstance()->Dispatch(smTask);
}

// apps call function to broadcast a set request.
void NeusoftAppMgrStubImpl::SM_SetReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid_req, std::string _key, std::string _value)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%#x, %s, %s)", _ampid_req, _key.c_str(), _value.c_str()));

    auto data = new std::tuple<unsigned int, std::string, std::string>(_ampid_req, _key, _value);

    auto smTask = std::make_shared<SMTask>();
    smTask->SetType(E_SM_TASK_CMD_BROADCAST_SET_REQUEST);
    smTask->SetUserData(reinterpret_cast<unsigned long long>(data), [](unsigned long long data){delete reinterpret_cast<std::tuple<unsigned int, std::string, std::string>*>(data);});

    TaskDispatcher::GetInstance()->Dispatch(smTask);
}

// value's maintainer call this function to broadcast a reply to "set request"
void NeusoftAppMgrStubImpl::SM_SetState(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid_req, std::string _key, std::string _value)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%#x, %s, %s)", _ampid_req, _key.c_str(), _value.c_str()));

    auto data = new std::tuple<unsigned int, std::string, std::string>(_ampid_req, _key, _value);

    auto smTask = std::make_shared<SMTask>();
    if(BROADCASTFLAG==_ampid_req){
        auto args = new std::tuple<std::string, std::string>(_key, _value);
        smTask->SetType(E_SM_TASK_REQUEST_BROADCAST);
        smTask->SetUserData(reinterpret_cast<unsigned long long>(args), [](unsigned long long args){delete reinterpret_cast<std::tuple<std::string, std::string>*>(args);});
    }else{
        smTask->SetType(E_SM_TASK_CMD_BROADCAST_SET_REPLY);
        smTask->SetUserData(reinterpret_cast<unsigned long long>(data), [](unsigned long long data){delete reinterpret_cast<std::tuple<unsigned int, std::string, std::string>*>(data);});
    }


    TaskDispatcher::GetInstance()->Dispatch(smTask);

    if(_key == SYS_LANG)
    {
#ifdef CODE_FOR_EV
        if(_value == "C")
		{
			mStatesMap[E_MODULE_EV_LANG] = "1";
		}
		else
		{
			mStatesMap[E_MODULE_EV_LANG] = "0";
		}
#else
        if(_value == "C")
        {
            mStatesMap[E_MODULE_LANG] = "1";
        }
        else
        {
            mStatesMap[E_MODULE_LANG] = "0";
        }
#endif
    }

}


#if 0
void NeusoftAppMgrStubImpl::SM_NotifyOnChange(const std::shared_ptr<CommonAPI::ClientId> _client, std::string _statMgr_Key)
{
	logVerbose(CAPI,"Capi_StubImpl.cpp::NeusoftAppMgrStubImpl::SM_NotifyOnChange:", _statMgr_Key,"--->IN");

	auto pTask = std::make_shared<SMTask>();
	pTask->SetType(SM_NOTIFYONCHANGE);
    pTask->SetKey(_statMgr_Key);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

	logVerbose(CAPI,"Capi_StubImpl.cpp::NeusoftAppMgrStubImpl::SM_NotifyOnChange:", _statMgr_Key,"--->OUT");
}
#endif
void NeusoftAppMgrStubImpl::requestAudioDeviceFocusReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _connectID)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::requestAudioDeviceFocusReq-->>in connectID=", ToHEX(_connectID) );

    //audioDeviceFocusGainNotify(_connectID);
    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(REQUEST_AUDIO_FOCUS);
	pTask->setConnectID(_connectID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::requestAudioDeviceFocusReq-->>out");
}

void NeusoftAppMgrStubImpl::releaseAudioDeviceFocusReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _connectID)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::releaseAudioDeviceFocusReq-->>in connectID=", ToHEX(_connectID));

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(RELEASE_AUDIO_FOCUS);
	pTask->setConnectID(_connectID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::releaseAudioDeviceFocusReq-->>out");
}

void NeusoftAppMgrStubImpl::setStreamMuteStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _streamID, bool _isMute)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::setStreamMuteStateReq-->>in streamID=", ToHEX(_streamID)," isMute:", _isMute);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(SET_STREAM_MUTE_STATE);
	pTask->setStreamID(_streamID);
    pTask->setMuteState(_isMute);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::setStreamMuteStateReq-->>out");
}

void NeusoftAppMgrStubImpl::requestToUseSpeakerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seatID)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::requestToUseSpeakerReq-->>in _connectID=", ToHEX(_seatID));

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(REQ_TO_USE_SPEAKER);
    pTask->setSeatID(_seatID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::requestToUseSpeakerReq-->>out");
}

void NeusoftAppMgrStubImpl::abandonToUseSpeakerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seatID)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::abandonToUseSpeakerReq-->>in _connectID=", ToHEX(_seatID));

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(ABN_TO_USE_SPEAKER);
    pTask->setSeatID(_seatID);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::abandonToUseSpeakerReq-->>out");
}

void NeusoftAppMgrStubImpl::getLastMediaAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _audioDevID, getLastMediaAppReqReply_t _reply)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::getLastMediaAppReq-->>in _audioDevID=", ToHEX(_audioDevID));

    _reply(AMClient::GetInstance()->getLastConnectMediaApp(_audioDevID));

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::getLastMediaAppReq-->>out");
}

void NeusoftAppMgrStubImpl::isVolumeTypeActive(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _volumeType, isVolumeTypeActiveReply_t _reply)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::isVolumeTypeActive-->>in _volumeType=", _volumeType);

    _reply(AudioSetting::GetInstance()->isVolumeTypeActive(_volumeType));

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::isVolumeTypeActive-->>out");
}

void NeusoftAppMgrStubImpl::requestDuckAudio(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _streamID, bool _isDuck, uint32_t _durationsms, uint32_t _volume)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::requestDuckAudio-->>in _streamID=", ToHEX(_streamID),"_isDuck",_isDuck,"_durationsms:",_durationsms,"_volume:",_volume);

    auto pTask = std::make_shared<AudioTask>();
    if(_isDuck){
        pTask->SetType(DUCK_AUDIO);
    }else{
        pTask->SetType(UNDUCK_AUDIO);
    }
    pTask->setStreamID(_streamID);
    pTask->setDuckDurations(_durationsms);
    pTask->setDuckVolume(_volume);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::requestDuckAudio-->>out");
}

void NeusoftAppMgrStubImpl::setByAppManagerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint16_t _amPID, uint32_t _key, uint32_t _value)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::setByAppManagerReq()-->>in _amPID=", ToHEX(_amPID),"_key", ToHEX(_key),"_value",ToHEX(_value));

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(SET_BY_APPMANAGER);
    pTask->setKey(_key);
    pTask->setValue(_value);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::setByAppManagerReq()-->>out");
}

void NeusoftAppMgrStubImpl::AMGRGetState(const std::shared_ptr<CommonAPI::ClientId> _client, uint16_t _amPID, uint32_t _state_type, AMGRGetStateReply_t _reply)
{
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::AMGRGetState()-->>in _amPID=", ToHEX(_amPID),"_state_type", ToHEX(_state_type));

	std::map<int, std::string>::iterator it;
    it = mStatesMap.find(_state_type);
	if (it != mStatesMap.end())
	{
		logVerbose(CAPI,"NeusoftAppMgrStubImpl::AMGRGetState()-->>out  value:",mStatesMap[_state_type]);
		_reply(mStatesMap[_state_type]);
	}
	else
	{
		logError(CAPI,"NeusoftAppMgrStubImpl::AMGRGetState()::state:",_state_type,"is not exist");
		_reply("null");
	}
}


void NeusoftAppMgrStubImpl::updateState(uint32_t state_type,std::string value)
{
    logVerbose(CAPI,"NeusoftAppMgrStubImpl::updateState -->> IN");

    mStatesMap[state_type] = value;

    logVerbose(CAPI,"NeusoftAppMgrStubImpl::updateState -->> OUT");
}

void NeusoftAppMgrStubImpl::sendPopupInfo(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint32_t _popupType, std::string _popupInfo, uint32_t _scrIdx, sendPopupInfoReply_t _reply)
{
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::sendPopupInfo -->> IN");
    logVerbose(CAPI, "CAPI PopScreen : ", _scrIdx);
    uint32_t popupId = gPopupMgr.getPopupId();
    auto csTask = std::make_shared<CSTask>();
    csTask->setAmpid(_ampid);
    csTask->SetType(E_CS_TASK_TYPE_POP_ADD);
    csTask->setPopInfo(PopInfo((PopupType)_popupType, _popupInfo, _ampid, _scrIdx));
    csTask->setPopupId(popupId);
    TaskDispatcher::GetInstance()->Dispatch(csTask);
    _reply(popupId);
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::sendPopupInfo -->> OUT");
}

void NeusoftAppMgrStubImpl::showPopup(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popupHandle, showPopupReply_t _reply)
{
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::showPopup -->> IN");
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_POP_SHOW);
    //csTask->setPopInfo(PopInfo((PopupType)_popupType, _popupInfo));
    csTask->setPopupId(_popupHandle);
    TaskDispatcher::GetInstance()->Dispatch(csTask);
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::showPopup -->> OUT");
	_reply(true);
}

void NeusoftAppMgrStubImpl::closePopup(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popupHandle, closePopupReply_t _reply)
{
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::closePopup -->> IN");
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_POP_DELETE);
    //csTask->setPopInfo(PopInfo((PopupType)_popupType, _popupInfo));
    csTask->setPopupId(_popupHandle);
    TaskDispatcher::GetInstance()->Dispatch(csTask);
	_reply(true);
	logVerbose(CAPI,"NeusoftAppMgrStubImpl::closePopup -->> OUT");
}

void NeusoftAppMgrStubImpl::popupRespond(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popinfoId, uint32_t _popRespondType, popupRespondReply_t _reply)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl popupRespond IN");
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_POP_RESPOND);
    csTask->setPopupRespondType(_popRespondType);
    csTask->setPopupId(_popinfoId);
    TaskDispatcher::GetInstance()->Dispatch(csTask);
    logVerbose(CAPI, "NeusoftAppMgrStubImpl popupRespond OUT");
    _reply();
}

void NeusoftAppMgrStubImpl::popupRespondEx(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _popinfoId, uint32_t _popRespondType, std::string _popRespondMessage, popupRespondExReply_t _reply)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl popupRespondEx IN");
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_POP_RESPOND_EX);
    csTask->setPopupRespondType(_popRespondType);
    csTask->setPopupRespondMessage(_popRespondMessage);
    csTask->setPopupId(_popinfoId);
    TaskDispatcher::GetInstance()->Dispatch(csTask);
    logVerbose(CAPI, "NeusoftAppMgrStubImpl popupRespondEx OUT");
    _reply();

}


void NeusoftAppMgrStubImpl::ExpandTitlebarReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebarID)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::ExpandTitlebarReq(", ToHEX(_titlebarID), ")--->IN");

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_titlebarID);
    hmicTask->SetType(HMIC_NOTIFY_TITLEBAR_EXPAND);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    logVerbose(CAPI, "NeusoftAppMgrStubImpl::ExpandTitlebarReq(", ToHEX(_titlebarID), ")--->OUT");
}

void NeusoftAppMgrStubImpl::ShrinkTitlebarReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebarID)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::ShrinkTitlebarReq(", ToHEX(_titlebarID), ")--->IN");

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_titlebarID);
    hmicTask->SetType(HMIC_NOTIFY_TITLEBAR_SHRINK);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    logVerbose(CAPI, "NeusoftAppMgrStubImpl::ShrinkTitlebarReq(", ToHEX(_titlebarID), ")--->OUT");
}



void NeusoftAppMgrStubImpl::Move2BottomLayerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, v0_1::org::neusoft::AppMgrStub::Move2BottomLayerReqReply_t _reply)
{
//    auto hmic = HMIController::GetInstance();
//    hmic->Move2BottomLayer(_requester);
    _reply();
}

void NeusoftAppMgrStubImpl::Return2HomeLayerReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, v0_1::org::neusoft::AppMgrStub::Return2HomeLayerReqReply_t _reply)
{
//    auto hmic = HMIController::GetInstance();
//    hmic->Return2HomeLayer(_requester);
    _reply();
}

void NeusoftAppMgrStubImpl::SetTitlebarVisibilityReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requesterID, bool _isVisible, int32_t _style)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::SetTitlebarVisibilityReq(%#x, %s, %d)", _requesterID, _isVisible?"true":"false", _style));

    typedef std::tuple<bool, int> TitlebarVisibilityProperties;
    auto property = new TitlebarVisibilityProperties(_isVisible, _style);

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_requesterID);
    hmicTask->SetType(HMIC_NOTIFY_APP_SWITCH_TITLEBAR_VISIBILITY);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(property), [](unsigned long long data){delete reinterpret_cast<TitlebarVisibilityProperties*>(data);});
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::GetTitlebarVisibilityReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, GetTitlebarVisibilityReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText(__STR_FUNCTION__+"(%#x)", _requester));

    // prepare argument list
    bool isVisible = true;
    auto argList = ZIP_ARG(isVisible);

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_requester);
    hmicTask->SetType(HMIC_QUERY_TITLEBAR_VISIBILITY_FOR_SCREEN);
    hmicTask->SetUserData(TO_USERDATA(argList));
    SendQueryTaskNReplyAtTaskDone(hmicTask, [_reply, argList](std::shared_ptr<Task> task){
        logInfo(CAPI, LOG_HEAD, "visibility: ", REF_ARG(task, decltype(argList), 0));

        _reply(REF_ARG(task, decltype(argList), 0));
    });
}

void NeusoftAppMgrStubImpl::MarkTitlebarDropDownStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _titlebar, bool _isDropDown)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::MarkTitlebarDropDownStateReq(%#x, %s)", _titlebar, _isDropDown?"true":"false"));

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_titlebar);
    hmicTask->SetType(HMIC_NOTIFY_TITLEBAR_DROPDOWNSTATECHANGE);
    hmicTask->SetUserData(static_cast<unsigned long long>(_isDropDown));
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::DropDownTitlebarReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, bool _isDropDown, uint32_t _style)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::DropDownTitlebarReq(%#x, %s, %u)", _requester, _isDropDown?"true":"false", _style));

    // make tuple
    std::tuple<bool, unsigned int> *param = new std::tuple<bool, unsigned int>(_isDropDown, _style);

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_requester);
    hmicTask->SetType(HMIC_CMD_TITLEBAR_DROPDOWN);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(param)
                         , [](unsigned long long param){delete reinterpret_cast<std::tuple<bool, unsigned int>*>(param);});
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::GetTitlebarDropDownStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _requester, v0_1::org::neusoft::AppMgrStub::GetTitlebarDropDownStateReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetTitlebarDropDownStateReq(%#x)", _requester));

    auto replyFunction = new std::function<void(unsigned int)>;
    *replyFunction = _reply;

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_requester);
    hmicTask->SetType(HMIC_QUERY_TITLEBAR_DROPDOWNSTATE);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(replyFunction)
                         , [](unsigned long long param){delete reinterpret_cast<std::function<void(unsigned int)>*>(param);});
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::DebugReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _debugCmd)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::DebugReq(", ToHEX(_debugCmd), ")--->IN");

    std::shared_ptr<Task> task = nullptr;
    auto hmicTask = std::make_shared<HMICTask>();
    switch(_debugCmd){
    //all ac debug tasks
    case E_SHOW_CONNECTIONS:
    case E_SHOW_AMSOURCES:
    case E_SHOW_AMSINKS:
    case E_SHOW_CONRECORD:
    case E_SHOW_ROUTINGMAP:
    case E_SHOW_FOCUSSTA:
    case E_SHOW_AUDIOSETTING:
    case E_SET_SEAT_IVI_USE_SPEAKER:
    case E_SET_SEAT_LRSE_USE_SPEAKER:
    case E_SET_SEAT_RRSE_USE_SPEAKER:
        task = std::make_shared<AudioTask>();
        task->SetType(DEBUG);
        dynamic_pointer_cast<AudioTask>(task)->setDebugCmd(_debugCmd);
        TaskDispatcher::GetInstance()->Dispatch(task);
        break;
    case E_SET_MASK_SURFACE_VISIBLE:
        hmicTask->SetAMPID(AMPID_AMGR(E_SEAT_IVI, E_APPID_MASK_APPS));
        hmicTask->SetUserData(true);
        hmicTask->SetType(HMIC_CMD_SET_MASK_SUR_VISIBLE);
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
        break;
    case E_SET_MASK_SURFACE_INVISIBLE:
        hmicTask->SetAMPID(AMPID_AMGR(E_SEAT_IVI, E_APPID_MASK_APPS));
        hmicTask->SetUserData(false);
        hmicTask->SetType(HMIC_CMD_SET_MASK_SUR_VISIBLE);
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
        break;
    //you can add your debug tasks here!
    case E_HMIC_RESTART:
        task = std::make_shared<HMICTask>();
        task->SetType(HMIC_NOTIFY_ILM_FATALERROR);
        TaskDispatcher::GetInstance()->Dispatch(task);
        break;
    default:
        logError(CAPI,"debug command don't have a handle method");
        break;
    }



    logVerbose(CAPI, "NeusoftAppMgrStubImpl::DebugReq(", ToHEX(_debugCmd), ")--->OUT");
}

void NeusoftAppMgrStubImpl::openIME(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, uint32_t _keyboardType, uint32_t _langType, std::string _paramStr, openIMEReply_t _reply)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::openIME --->IN");
    gImeMgr.createLink(_ampid, _keyboardType, _langType, _paramStr);
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::openIME --->OUT1");
    _reply();
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::openIME --->OUT");
}

void NeusoftAppMgrStubImpl::closeIME(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, closeIMEReply_t _reply)
{
    logVerbose(CAPI, "NeusoftAppMgrStubImpl::closeIME IN");
    gImeMgr.closeIME(_ampid);
    _reply();
}

void NeusoftAppMgrStubImpl::StartShareAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, uint32_t _seat, bool isMove)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::StartShareAppReq(%#x, %d, %s)", _app, _seat, isMove?"true":"false"));

    DispatchScreenShareTask(_app, static_cast<E_SEAT>(_seat), isMove, STH_START_SCREEN_SAHRE, nullptr);
}

void NeusoftAppMgrStubImpl::StopShareAppReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, uint32_t _seat, StopShareAppReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::StopShareAppReq(%#x, %d)", _app, _seat));

    DispatchScreenShareTask(_app, static_cast<E_SEAT>(_seat), false, STH_STOP_SCREEN_SAHRE, _reply);
}

void NeusoftAppMgrStubImpl::GetCurrentShareReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, GetCurrentShareReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetCurrentShareReq(%d)", _seat));

    auto replyFunction = new std::function<void(unsigned int)>;
    *replyFunction = _reply;

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(AMPID_APP(_seat, E_APPID_SYNC_APP));
    hmicTask->SetType(HMIC_QUERY_SYNC_GET_CURRENT_SHARE);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(replyFunction));
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::GetSourceSurfacesReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, GetSourceSurfacesReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetSourceSurfacesReq(%#x)", _app));

    auto replyFunction = new std::function<void(std::vector<SurfaceID>)>;
    *replyFunction = _reply;

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_app);
    hmicTask->SetType(HMIC_QUERY_SYNC_SOURCE_SURFACES);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(replyFunction));
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::GetWorkingLayersReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _app, GetWorkingLayersReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetWorkingLayersReq(%#x)", _app));

    auto replyFunction = new std::function<void(std::vector<LayerID>, bool)>;
    *replyFunction = _reply;

    // send task
    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetAMPID(_app);
    hmicTask->SetType(HMIC_QUERY_SYNC_WORKING_LAYERS);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(replyFunction));
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void NeusoftAppMgrStubImpl::KeepAppAlive(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t __ampid, bool __state)
{
    ProcessInfo* info = ProcessInfo::GetInstance();
    info->Lock();
    shared_ptr<Process> process = ProcessInfo::GetInstance()->FindProc(__ampid);
    if(process){
        logDebugF(CAPI,"KeepAppAlive update app(%x) status in ProcessInfo new KeepAppAlive(%d)",__ampid,__state);
        process->updateAlwaysAliveAppBPM(__ampid,__state);
    }
    info->Unlock();
}

/* desc:  set true: rear will keep LCD on until set state "FALSE"
 *        set false:rear will close LCD after 30S without any touch
 * param: uint32_t _seat, seatNumber
 * param: bool _state true means start timer  false means stop timer
 * return: none
*/
void NeusoftAppMgrStubImpl::SetRearTimerSTOP(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _ampid,bool _state)
{
    DomainVerboseLog chater(CAPI, formatText(__STR_FUNCTION__+"(%#x, %s)", _ampid, _state?"true":"false"));

    LCDManager::GetInstance()->SetRearTimerState(!_state,_ampid);

//    auto pmcTask = make_shared<PMCTask>();
//    if (_state)
//        pmcTask->setPowerSignal(E_RSE_TIMER_DISABLE);
//    else
//        pmcTask->setPowerSignal(E_RSE_TIMER_ENABLE);

//    pmcTask->SetType(HANDLE_LCD_SIGNAL);
//    pmcTask->setSeatID((E_SEAT)_seat);
//    TaskDispatcher::GetInstance()->Dispatch(pmcTask);

}

void NeusoftAppMgrStubImpl::StartHomeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::StartHomeReq(%#x)", _seat));

    StartService(AMPID_APP(_seat, E_APPID_HOME));
}

void NeusoftAppMgrStubImpl::SendBootLogReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgr::StringArray _bootLog)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::SendBootLogReq(%#x)", _ampid));

    AppProfileManager::GetInstance()->OnAppSDKBootLogReceived(_ampid, _bootLog);
}

void NeusoftAppMgrStubImpl::GetLogIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::GetLogIDReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetLogIDReq(%#x)", _ampid));

    _reply(GET_APPLOGID_FROM_AMPID(_ampid));
}

void NeusoftAppMgrStubImpl::GetActiveAppListReq(const std::shared_ptr<CommonAPI::ClientId> _client, v0_1::org::neusoft::AppMgrStub::GetActiveAppListReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetActiveAppListReq()"));

    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appList = procInfo->FindProc([](shared_ptr<Process> proc)->bool{
        return proc->IsOperating();
    });
    procInfo->Unlock();

    std::vector<AMPID> list;
    for(auto& proc: appList)
        list.push_back(proc->GetAMPID());

    _reply(list);
}

void NeusoftAppMgrStubImpl::GetAppStateReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::GetAppStateReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetAppStateReq(%#x)", _ampid));

    AppStatus status = 0;

    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto proc = procInfo->FindProc(_ampid);
    if(proc)
        status = proc->GetStatus();
    procInfo->Unlock();

    _reply(status);
}

void NeusoftAppMgrStubImpl::EarlyInitializeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::EarlyInitializeReq(%#x)", _ampid));

    EarlyInitializeNotify(_ampid);
}

void NeusoftAppMgrStubImpl::GetSurfaceIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::GetSurfaceIDReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetSurfaceIDReq(%#x)", _ampid));

    _reply(GetSurfaceID(_ampid));
}

void NeusoftAppMgrStubImpl::GetUnderLayerSurfaceIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::GetUnderLayerSurfaceIDReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetUnderLayerSurfaceIDReq(%#x)", _ampid));

    _reply(SURFACEID(_ampid, LAYERID(GET_SEAT(_ampid), E_LAYER_VIDEOPLAY)));
}

void NeusoftAppMgrStubImpl::GetBKGLayerSurfaceIDReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid, v0_1::org::neusoft::AppMgrStub::GetBKGLayerSurfaceIDReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, formatText("NeusoftAppMgrStubImpl::GetBKGLayerSurfaceIDReq(%#x)", _ampid));

    _reply(SURFACEID(_ampid, LAYERID(GET_SEAT(_ampid), E_LAYER_BACKGROUND)));
}
void NeusoftAppMgrStubImpl::SetScreenOnByApp(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _seat)
{
    DomainVerboseLog chatter (CAPI,formatText(" NeusoftAppMgrStubImpl SetScreenOnByApp _seat:%d",_seat));
    auto pmcTask = make_shared<PMCTask>();
    pmcTask->SetType(HANDLE_APP_REQ_SET_SCREENON);
    pmcTask->setSeatID((E_SEAT)_seat);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);
}
void NeusoftAppMgrStubImpl::SetRearPowerOff(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _seat)
{
    DomainVerboseLog chatter (CAPI,formatText(" NeusoftAppMgrStubImpl SetRearPowerOff _seat:%d ",_seat));

    //    auto fpmTask = std::make_shared<FPMTask>();
    //    fpmTask->SetType(REAR_POW_OFF);
    //    fpmTask->setDetail(_seat);
    //    TaskDispatcher::GetInstance()->Dispatch(fpmTask);

    auto pmcTask = make_shared<PMCTask>();
    pmcTask->setPowerSignal(E_RSE_POWER_OFF);
    pmcTask->SetType(HANDLE_LCD_SIGNAL);
    pmcTask->setSeatID((E_SEAT)_seat);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);

}
void NeusoftAppMgrStubImpl::AppAwakeRequest(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _ampid ,bool _isAwake)
{
    AppAwakeMgr::GetInstance()->handlePowerAwakeRequest(_ampid,_isAwake);
}

void NeusoftAppMgrStubImpl::PowerAwakeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid,bool _isPowerAwake)
{
    AppAwakeMgr::GetInstance()->handlePowerAwakeRequest(_ampid,_isPowerAwake);
}

void NeusoftAppMgrStubImpl::ScreenAwakeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid,bool _isScreenAwake)
{
    AppAwakeMgr::GetInstance()->handleScreenAwakeRequest(_ampid,_isScreenAwake);
}

void NeusoftAppMgrStubImpl::AbnormalAwakeReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _ampid,bool _isScreenAwake)
{
    AppAwakeMgr::GetInstance()->handleAbnormalAwakeRequest(_ampid,_isScreenAwake);
}

void NeusoftAppMgrStubImpl::SendPowerSignalReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _powerSignal)
{
    auto pmcTask = std::make_shared<PMCTask>();
    pmcTask->SetType(HANDLE_PM_SIGNAL);
    pmcTask->setPowerSignal((ama_PowerSignal_e)_powerSignal);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);
}

void NeusoftAppMgrStubImpl::requestAlwaysOn(const std::shared_ptr<CommonAPI::ClientId> _client,uint32_t _ampid ,bool _isAwaysOn )
{
    auto bpmTask = std::make_shared<BPMTask>();
    if (_isAwaysOn)
        bpmTask->SetType(BPM_CMD_NEVER_STOP);
    else
        bpmTask->SetType(BPM_CMD_CAN_BE_STOP);
    bpmTask->setApmid(_ampid);
    TaskDispatcher::GetInstance()->Dispatch(bpmTask);

}
void NeusoftAppMgrStubImpl::SetBrightness(const std::shared_ptr<CommonAPI::ClientId> _client,uint16_t _seatid ,uint16_t _brightness)
{
   // unsigned int seat=GET_SEAT(_ampid);
    LCDManager::GetInstance()->SetLCDBrightness(_brightness,(E_SEAT)_seatid);

}
void NeusoftAppMgrStubImpl::USBStateController(const std::shared_ptr<CommonAPI::ClientId> _client, uint16_t _op )
{
    logVerboseF(CAPI,"USBStateController  IN OP:%04x",_op);
    USBManager::GetInstance()->UsbOperate((ama_USBOperate) _op);
    logVerboseF(CAPI,"USBStateController OUT OP:%04x",_op);
}

void NeusoftAppMgrStubImpl::RequestLcdTemperature(const std::shared_ptr<CommonAPI::ClientId> _client ,RequestLcdTemperatureReply_t _reply)
{
    logVerboseF(CAPI,"RequestLcdTemperature  IN");
    _reply(LCDManager::GetInstance()->GetAllLCDTemperature());
    logVerboseF(CAPI,"RequestLcdTemperature OUT");
}
void NeusoftAppMgrStubImpl::GetPowerStateWhenSDKInitReq(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _seat, GetPowerStateWhenSDKInitReqReply_t _reply)
{
    _reply(PowerManagerController::GetInstance()->getPowerState(_seat));
}

void NeusoftAppMgrStubImpl::GetAllPowerStateWhenSDKInitReq(const std::shared_ptr<CommonAPI::ClientId> _client,GetAllPowerStateWhenSDKInitReqReply_t _reply)
{
    auto pmc = PowerManagerController::GetInstance();
    _reply(pmc->getPowerState(E_SEAT_IVI),pmc->getPowerState(E_SEAT_RSE1),pmc->getPowerState(E_SEAT_RSE2));
}

void NeusoftAppMgrStubImpl::getUSBDevState(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t __ampid)
{
    logVerboseF(CAPI,"NeusoftAppMgrStubImpl::getUSBDevState  IN");
    USBManager::GetInstance()->getUSBStateAsync();
    logVerboseF(CAPI,"NeusoftAppMgrStubImpl::getUSBDevState  OUT");
}

static IntentInfo GetIntentInfo(const v0_1::org::neusoft::AppMgr::IntentInfo& CAPIInfo)
{
    IntentInfo info;
    info.itemID = CAPIInfo.getItemID();
    info.action = CAPIInfo.getAction();
    info.data = CAPIInfo.getData();
    info.isQuiet = CAPIInfo.getIsQuiet();

    return std::move(info);
}

static std::vector<IntentInfo> GetIntentInfoVector(const v0_1::org::neusoft::AppMgr::IntentInfoArray& array)
{
    std::vector<IntentInfo> infoVector;
    for(auto& i: array)
        infoVector.push_back(GetIntentInfo(i));

    return std::move(infoVector);
}

static v0_1::org::neusoft::AppMgr::IntentInfo GetCAPIIntentInfo(const IntentInfo &info)
{
    v0_1::org::neusoft::AppMgr::IntentInfo intentInfo;
    intentInfo.setItemID(info.itemID);
    intentInfo.setAction(info.action);
    intentInfo.setData(info.data);
    intentInfo.setIsQuiet(info.isQuiet);

    return std::move(intentInfo);
}

static v0_1::org::neusoft::AppMgr::IntentInfoArray GetCAPIIntentInfoArray(const std::vector<IntentInfo>& intentInfoVector)
{
    v0_1::org::neusoft::AppMgr::IntentInfoArray array;

    for(auto& info: intentInfoVector)
        array.push_back(GetCAPIIntentInfo(info));

    return std::move(array);
}

void NeusoftAppMgrStubImpl::GetRawLastSourceReq(const std::shared_ptr<CommonAPI::ClientId> _client, v0_1::org::neusoft::AppMgrStub::GetRawLastSourceReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, __STR_FUNCTIONP__);

    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_GET_LAST_SOURCE);
    SendQueryTaskNReplyAtTaskDone(csTask, [_reply](std::shared_ptr<Task> pTask){
        logInfo(CAPI, LOG_HEAD, "replying...");

        auto csTask = std::dynamic_pointer_cast<CSTask>(pTask);
        if(!csTask)
            _reply(v0_1::org::neusoft::AppMgr::IntentInfoArray());
        else
            _reply(GetCAPIIntentInfoArray(csTask->getLastSourceSet()));
        logInfo(CAPI, LOG_HEAD, "done.");
    });
}

void NeusoftAppMgrStubImpl::GetLastSourceReq(const std::shared_ptr<CommonAPI::ClientId> _client, v0_1::org::neusoft::AppMgrStub::GetLastSourceReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, __STR_FUNCTIONP__);

    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_GET_CURRENT_SOURCE);
    SendQueryTaskNReplyAtTaskDone(csTask, [_reply](std::shared_ptr<Task> pTask){
        logInfo(CAPI, LOG_HEAD, "replying...");

        auto csTask = std::dynamic_pointer_cast<CSTask>(pTask);
        if(!csTask)
            _reply(v0_1::org::neusoft::AppMgr::IntentInfoArray());
        else
            _reply(GetCAPIIntentInfoArray(csTask->getLastSourceSet()));
        logInfo(CAPI, LOG_HEAD, "done.");
    });
}

void NeusoftAppMgrStubImpl::SetLastSourceReq(const std::shared_ptr<CommonAPI::ClientId> _client, v0_1::org::neusoft::AppMgr::IntentInfoArray _lastSourceSet, v0_1::org::neusoft::AppMgrStub::SetLastSourceReqReply_t _reply)
{
    DomainVerboseLog chatter(CAPI, __STR_FUNCTIONP__);

    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_SET_CURRENT_SOURCE);
    csTask->setLastSourceSet(GetIntentInfoVector(_lastSourceSet));
    TaskDispatcher::GetInstance()->Dispatch(csTask);

    _reply();
}
