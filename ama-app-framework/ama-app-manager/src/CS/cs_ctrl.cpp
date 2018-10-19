/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "cs_ctrl.h"
#include "TaskDispatcher.h"
#include "rpc_proxy_def.h"
#include "cs_task.h"
#include "SMTask.h"
#include "rpc_proxy_def.h"
#include "PLCTask.h"
#include "HMICTask.h"
#include "wd.h"
#include "pers_config.h"
#include "SMCtrl.h"
#include "DataBase.h"

#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "time_sync_util.h"
#include "rpc_proxy.h"
#include "ime_mgr.h"
#include "AudioTask.h"
#include "AppList.h"
#include <tinyxml2.h>

extern PopupManager gPopupMgr;
extern RpcProxy rp;

static int copyFile(const char *sourceFileNameWithPath,
        const char *targetFileNameWithPath)
{
    FILE *fpR, *fpW;
    int BUFFER_SIZE = (1024*1024);
    char buffer[BUFFER_SIZE];
    int lenR, lenW;
    if ((fpR = fopen(sourceFileNameWithPath, "r")) == NULL)
    {
        logError(CS,"the source file can not be opened!");
        return E_UNKNOWN;
    }
    if ((fpW = fopen(targetFileNameWithPath, "w")) == NULL)
    {
        logError(CS,"the target file can not be opened!");
        fclose(fpR);
        return E_UNKNOWN;
    }

    memset(buffer, 0, BUFFER_SIZE);
    while ((lenR = fread(buffer, 1, BUFFER_SIZE, fpR)) > 0)
    {
        if ((lenW = fwrite(buffer, 1, lenR, fpW)) != lenR)
        {
            logError("Write to target file failed!");
            fclose(fpR);
            fclose(fpW);
            return E_UNKNOWN;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    fclose(fpR);
    fclose(fpW);
    return E_OK;
}

static bool isFileEmpty(const char *filepath)
{
    logError(CS, "Pers isFileEmpty  IN");
    FILE *fp;
    char ch;
    if ((fp = fopen(filepath,"r")) == NULL)
        return false;
    ch = fgetc(fp);
    fclose(fp);
    return ch == 0xff;
}

static bool isCompleteXmlFile(const char *filepath)
{
    logError(CS, "Pers isCompleteXmlFile  IN");
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filepath);
    return (doc.ErrorID() == 0);
}

void HardkeyFunCB(KeyID id, KeyEventID eid, void * data)
{
    auto csTask = std::make_shared<CSTask>();
    csTask->SetType(E_CS_TASK_TYPE_KEY);
    csTask->setKeyEvent(id, eid, data);
    TaskDispatcher::GetInstance()->Dispatch(csTask);
}

void RpcRevFunCB(RpcID id, void * data, int len)
{
    switch(id)
    {
        case RPC_SPEED_CHANGE:
        {
            auto csTask = std::make_shared<CSTask>();
            csTask->SetType(E_CS_TASK_TYPE_SPEED_CHANGE_GALA);
            csTask->SetUserData(*(static_cast<unsigned char *>(data)));
            TaskDispatcher::GetInstance()->Dispatch(csTask);
        }
        break;
        case RPC_EOL_CHANGED:
        {
            EOLInfo* pEOLInfo = new EOLInfo(*(static_cast<EOLInfo*>(data)));

            auto csTask = std::make_shared<CSTask>();
            csTask->SetType(E_CS_TASK_TYPE_EOL_CHANGED);
            csTask->SetUserData(reinterpret_cast<unsigned long long>(pEOLInfo), [](unsigned long long data){delete reinterpret_cast<EOLInfo*>(data);});
            TaskDispatcher::GetInstance()->Dispatch(csTask);
        }
        break;
        default:
            logVerbose(CS, LOG_HEAD, "unknown RPC callback: ", id);
    }
}

static void SetCameraMode(bool isAVM)
{
    DomainVerboseLog chatter(CS, formatText("SetCameraMode(%s)", isAVM?"AVM":"RVC"));

    // write camera mode
    std::string fCameraModePath = "/media/ivi-data/ama.d/cameraMode";
    std::ofstream fCameraMode(fCameraModePath.c_str());
    if(fCameraMode.is_open())
    {
        fCameraMode << (isAVM?1:0);
        fCameraMode.close();
    }
    else
    {
        logWarn(CS, "SetCameraMode(): unable to open configuration file: ", fCameraModePath);
    }

    AMPID rvc = AMPID_APP(E_SEAT_IVI, E_APPID_RVC);
    AMPID avm = AMPID_APP(E_SEAT_IVI, E_APPID_AVM);
    LayerID hmi = LAYERID(E_SEAT_IVI, E_LAYER_CAMERAHMI);
    LayerID guideLine = LAYERID(E_SEAT_IVI, E_LAYER_CAMERAGUIDLINE);
    static const std::map<bool, std::vector<SurfaceID>> keySurfaces = {
        // RVC
        {false, {/*SURFACEID(rvc, guideLine),*/ SURFACEID(rvc, hmi)}}, // guide-line is not controlled by ama-app-camera-service.service any longer.
        // AVM
        {true, {SURFACEID(avm, hmi)}},
    };

    // write key surfaces
    std::string fCameraKeySurfacesPath = "/media/ivi-data/ama.d/cameraKeySurfaces";
    std::ofstream fCameraKeySurfaces(fCameraKeySurfacesPath.c_str());
    if(fCameraKeySurfaces.is_open())
    {
        auto surfaces = keySurfaces.at(isAVM);
        for(auto& surface: surfaces)
            fCameraKeySurfaces << ToHEX(surface) << std::endl;
        fCameraKeySurfaces.close();
    }
    else
    {
        logWarn(CS, "SetCameraMode(): unable to open configuration file: ", fCameraKeySurfacesPath);
    }
}

CSController::CSController()
{
    DomainVerboseLog chatter(CS, "CSController::CSController()");
    //installPCL();

    auto screens = GetAvailableScreens();
    int retry = 2; // retry twice
    while(screens.empty() && (retry--)>0)
        screens = GetAvailableScreens();
    for(auto & screen: screens)
    {
        auto plcTask = make_shared<PLCTask>();
        plcTask->SetType(PLC_CMD_START);
        plcTask->SetAMPID(AMPID_SERVICE(screen, E_APPID_POPUP));
        TaskDispatcher::GetInstance()->Dispatch(plcTask);
        logDebug(CS, "AMPID  ", ToHEX(plcTask->GetAMPID()));
    }
    // using mcu-time to update systime
}

void CSController::doTask(std::shared_ptr<Task> task)
{
    //assert(task);
    auto csTask = dynamic_pointer_cast<CSTask>(task);

   if (csTask->IsWatchDotTask()) {
       wdSetState(E_TASK_ID_CS, WATCHDOG_SUCCESS);
       return ;
   }

    logVerbose(CS,"CSController::doTask() IN");
    switch (csTask->GetType())
    {
        case E_CS_TASK_TYPE_KEY:
        {
            KeyID keyid = csTask->getKeyId();
            KeyEventID keyEventId = csTask->getKeyEventId();
            void * data = csTask->getKeyData();
            hardkeyHandler(keyid, keyEventId, data);
            break;
        }
        case E_CS_TASK_TYPE_POP_ADD:
        {
            PopInfo pi = csTask->getPopInfo();
            // popupMgr_->putPopInfo(pi);
            logInfo(CS, "Popup Request ADD");
            gPopupMgr.putPopInfo(csTask->getPopupId(), pi);
            break;
        }
        case E_CS_TASK_TYPE_POP_DELETE:
        {
            logInfo(CS, "Popup Request Delete");
            gPopupMgr.removePopInfo(csTask->getPopupId());
            break;
        }
        case E_CS_TASK_INSTALL_PCL:
        {
            logInfo(CS, "install PCL Data");
            //installPCL();
            break;
        }
        case E_CS_TASK_TYPE_POP_RESPOND:
        {
            logInfo(CS, "Popup Respond");
            uint32_t popId = csTask->getPopupId();
            uint32_t reason = csTask->getPopupRespondType();
            gPopupMgr.processPopupRespond(popId, reason);
            break;
        }
        case E_CS_TASK_TYPE_POP_RESPOND_EX:
        {
            logInfo(CS, "Popup Respond");
            uint32_t popId = csTask->getPopupId();
            uint32_t reason = csTask->getPopupRespondType();
            std::string message = csTask->getPopupRespondMessage();
            gPopupMgr.processPopupRespond(popId, reason, message);
            break;
        }
        case E_CS_TASK_TYPE_IME_SHOW:
        {
            gImeMgr.updateAppShowStatus(csTask->getAmpid());
            break;
        }
        case E_CS_TASK_TYPE_IME_CLOSE:
        {

            break;
        }
        case E_CS_TASK_TYPE_SPEED_CHANGE_GALA:
        {
            unsigned char speedGala = csTask->GetUserData();
            onSpeedGalaChanged(speedGala);
            break;
        }
        case E_CS_TASK_TYPE_EOL_CHANGED:
        {
            EOLInfo* ei = reinterpret_cast<EOLInfo*>(csTask->GetUserData());
            if(ei)
                onEOLChanged(*ei);
            break;
        }
        case E_CS_TASK_TYPE_IME_HIDE:
        {
            logVerbose(CS, "IME TASK hide IN");
            uint32_t ampid = csTask->getAmpid();
            if (gImeMgr.getFocusAmpid(ampid) == ampid) {
                gImeMgr.closeIME(ampid);
            }
            logVerbose(CS, "IME TASK hide OUT ampid : ", ampid);
            break;
        }
        case E_CS_TASK_TYPE_ENABLE_CHILDRENLOCK:
        {
            setChildrenLock(true);
            break;
        }
        case E_CS_TASK_TYPE_DISABLE_CHILDRENLOCK:
        {
            setChildrenLock(false);
            break;
        }
        case E_CS_TASK_TYPE_ENABLE_RSELOCK:
        {
            setRSELock(true);
            break;
        }
        case E_CS_TASK_TYPE_DISABLE_RSELOCK:
        {
            setRSELock(false);
            break;
        }
        case E_CS_TASK_TYPE_SET_CURRENT_SOURCE:
        {
            setCurrentSource(csTask->getLastSourceSet());
            break;
        }
        case E_CS_TASK_TYPE_GET_CURRENT_SOURCE:
        {
            std::vector<IntentInfo> infoVector;
            getCurrentSource(infoVector);

            csTask->setLastSourceSet(std::move(infoVector));
            break;
        }
        case E_CS_TASK_TYPE_SAVE_CURRENT_SOURCE:
        {
            saveCurrentSource();
            break;
        }
        case E_CS_TASK_TYPE_GET_LAST_SOURCE:
        {
            std::vector<IntentInfo> infoVector;
            getLastSource(infoVector);

            csTask->setLastSourceSet(std::move(infoVector));
            break;
        }
        case E_CS_TASK_MODULE_STARTUP:
        {
            onModuleStartup();
            break;
        }
        default:
            logError(CS, "task type is not defined");
            break;
    }
    logVerbose(CS,"CSController::doTask()  OUT");
}

void CSController::installPCL()
{
    logVerbose(CS,"CSController::installPCL()-->IN");
    int ret = 0;
    // Remove it when Config File is empty
    if (0 == access("/media/ivi-data/ama.d/pers_config.xml",F_OK)) {
        if (/*isFileEmpty("/media/ivi-data/ama.d/pers_config.xml")  || */(!isCompleteXmlFile("/media/ivi-data/ama.d/pers_config.xml"))) {
            logError(CS, "remove pers config file");
            remove("/media/ivi-data/ama.d/pers_config.xml");
        }
    }
    if(0 == (access("/media/ivi-data/ama.d",F_OK))){
        logDebug(CS,"/media/ivi-data/ama.d already exist");
    }else{
        logDebug(CS,"/media/ivi-data/ama.d is not exist");
        ret = mkdir("/media/ivi-data/ama.d/",0777);
        if(E_OK == ret){
            logDebug(CS,"mkdir /media/ivi-data/ama.d/ successed");
        }else{
            logError(CS,"mkdir /media/ivi-data/ama.d/ failed");
            return;
        }
    }

    if(0 == (access("/media/ivi-data/ama.d/pers_config.xml",F_OK))){
//        //we want to reinstall PCL Data
//        ret = copyFile("/etc/ama.d/pers_config.xml","/media/ivi-data/ama.d/pers_config.xml");
        logDebug(CS,"persistance Config file exist");
    }else{
        logDebug(CS,"persistance Config file is not exist");
        if(0 == (access("/etc/ama.d/pers_config.xml",F_OK))){
            logDebug(CS,"/etc/ama.d/pers_config.xml has been installed");
            ret = copyFile("/etc/ama.d/pers_config.xml","/media/ivi-data/ama.d/pers_config.xml");
            if(ret != E_OK){
                logError(CS,"copy file failed");
                return;
            }else{
                logDebug(CS,"copy file successed");
            }
        }else{
            logError(CS,"/etc/ama.d/pers_config.xml has not been installed");
            return;
        }
    }

    PersConfig pc("/media/ivi-data/ama.d/pers_config.xml", "/usr/share/persistent");
    pc.installPersFiles();
    logVerbose(CS,"CSController::installPCL()-->OUT");
}

void CSController::hardkeyHandler(KeyID id, KeyEventID eid, void * data)
{
    logVerbose("CSController::hardkeyHandler IN");
    if ((id & E_KEY_HOME) && (eid & KEY_EVENT_DOWN)) {
        homeKeyHandler();
    }
    logVerbose("CSController::hardkeyHandler OUT");
}

int trriger=0;
void CSController::homeKeyHandler()
{
    logVerbose("CSController::homeKeyHandler IN");
     // <PLC> appStartReq
    auto plcTask = make_shared<PLCTask>();
    plcTask->SetType(PLC_CMD_START);
    plcTask->SetAMPID(AMPID_APP(E_SEAT_IVI,E_APPID_HOME));
    plcTask->SetUserData(1);
    TaskDispatcher::GetInstance()->Dispatch(plcTask);
    logVerbose("Home App Starting ...  ");
    logVerbose("CSController::homeKeyHandler  OUT");

}

void CSController::onSpeedGalaChanged(unsigned char speedGala)
{
    FUNCTION_SCOPE_LOG_C(CS);

    auto pTask = std::make_shared<AudioTask>();
    pTask->SetType(SET_GAINFORSPEED_BY_RPC);
    pTask->setGainForSpeed(speedGala);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}

void CSController::onEOLChanged(EOLInfo ei)
{
    FUNCTION_SCOPE_LOG_C(CS);

#ifdef CODE_FOR_EV
    // set AVM/RVC
    SetCameraMode(true);
    // RSE enable/disable
    EnableRSE(false);
#else
    // set AVM/RVC
    SetCameraMode(ei.RI_AVS);
    // RSE enable/disable
    EnableRSE(ei.RPE);
#endif
}

void CSController::setChildrenLock(bool isEnable)
{
    logVerbose(CS,"CSController::setChildrenLock()-->IN isEnable:",isEnable);

    if(isEnable){
        logInfo(CS,"CSController::enable Children Lock");
        SendEnableChildSafetyLock(E_SEAT_RSE1,isEnable);
        SendEnableChildSafetyLock(E_SEAT_RSE2,isEnable);
        stopAllRSEMediaApp();
        startRSEHomeApp();
    }else{
        logInfo(CS,"CSController::disable Children Lock");
        SendEnableChildSafetyLock(E_SEAT_RSE1,isEnable);
        SendEnableChildSafetyLock(E_SEAT_RSE2,isEnable);
    }

    logVerbose(CS,"CSController::setChildrenLock()-->OUT");
}

static void RSELockPNGObserver(const string& key, const SMValue& value, SMValueObserver::E_EVENT event)
{
    if(key!=SYS_LANG)
        return;

    DomainVerboseLog chatter(CS, formatText("RSELockPNGObserver(%s, %s, %d)", key.c_str(), value.value.c_str(), event));

    if(event==SMValueObserver::E_EVENT_VALUE_CHANGED || event==SMValueObserver::E_EVENT_KEY_CREATED)
    {
        std::string pngName = value.value=="C"?"RSELock_E.png":"RSELock_C.png";
        SendEnableTouchScreen(E_SEAT_RSE1,false,"/etc/ama.d/"+pngName);
        SendEnableTouchScreen(E_SEAT_RSE2,false,"/etc/ama.d/"+pngName);
        logInfo(CS, "RSELockPNGObserver(): system's language is changed: current language is \"", value.value, "\"");
    }

    if(event==SMValueObserver::E_EVENT_KEY_DELETED)
    {
        SendEnableTouchScreen(E_SEAT_RSE1,false, "/etc/ama.d/RSELock_C.png");
        SendEnableTouchScreen(E_SEAT_RSE2,false, "/etc/ama.d/RSELock_C.png");
        logWarn(CS, "RSELockPNGObserver(): system's language is deleted! use chinese RSE-lock picture.");
    }
}

void CSController::setRSELock(bool isEnable)
{
    logVerbose(CS,"CSController::setRSELock()-->IN isEnable:",isEnable);

    if(isEnable){
        logInfo(CS,"CSController::enable RSE Lock");
        logVerbose(CS,"sendEnableTouchScreen()-->IN");
        std::string pngName = sSMKeyValuePairObserver.GetValue(SYS_LANG).value=="C"?"RSELock_E.png":"RSELock_C.png";
        SendEnableTouchScreen(E_SEAT_RSE1,false,"/etc/ama.d/"+pngName);
        SendEnableTouchScreen(E_SEAT_RSE2,false,"/etc/ama.d/"+pngName);
        sSMKeyValuePairObserver.AddObserver(SYS_LANG, RSELockPNGObserver, SMValueObserver::E_EVENT_ALL);
        logVerbose(CS,"sendEnableTouchScreen()-->OUT");
        stopAllRSEMediaApp();
        startRSEHomeApp();
    }else{
        logInfo(CS,"CSController::disable RSE Lock");
        sSMKeyValuePairObserver.DeleteObserver(SYS_LANG, RSELockPNGObserver, SMValueObserver::E_EVENT_ALL);
        SendEnableTouchScreen(E_SEAT_RSE1,true);
        SendEnableTouchScreen(E_SEAT_RSE2,true);
    }

    logVerbose(CS,"CSController::setRSELock()-->OUT");
}

void CSController::stopAllRSEMediaApp(void)
{
    logVerbose(CS,"CSController::stopAllRSEMediaApp()-->IN ");

    ama_AMPID_t amPID;

    //stop LRSE music app
    auto pLRSEMusicTask = std::make_shared<PLCTask>();
    pLRSEMusicTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE1,E_APPID_MUSIC);
    pLRSEMusicTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pLRSEMusicTask);

    //stop LRSE video app
    auto pLRSEVideoTask = std::make_shared<PLCTask>();
    pLRSEVideoTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE1,E_APPID_VIDEO);
    pLRSEVideoTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pLRSEVideoTask);

    //stop LRSE image app
    auto pLRSEImageTask = std::make_shared<PLCTask>();
    pLRSEImageTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE1,E_APPID_IMAGE);
    pLRSEImageTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pLRSEImageTask);

    //stop LRSE tuner app
    auto pLRSETunerTask = std::make_shared<PLCTask>();
    pLRSETunerTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE1,E_APPID_RADIO);
    pLRSETunerTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pLRSETunerTask);

    //stop RRSE music app
    auto pRRSEMusicTask = std::make_shared<PLCTask>();
    pRRSEMusicTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE2,E_APPID_MUSIC);
    pRRSEMusicTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pRRSEMusicTask);

    //stop RRSE video app
    auto pRRSEVideoTask = std::make_shared<PLCTask>();
    pRRSEVideoTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE2,E_APPID_VIDEO);
    pRRSEVideoTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pRRSEVideoTask);

    //stop RRSE image app
    auto pRRSEImageTask = std::make_shared<PLCTask>();
    pRRSEImageTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE2,E_APPID_IMAGE);
    pRRSEImageTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pRRSEImageTask);

    //stop RRSE tuner app
    auto pRRSETunerTask = std::make_shared<PLCTask>();
    pRRSETunerTask->SetType(PLC_CMD_STOP);
    amPID = AMPID_APP(E_SEAT_RSE2,E_APPID_RADIO);
    pRRSETunerTask->SetAMPID(amPID);
    TaskDispatcher::GetInstance()->Dispatch(pRRSETunerTask);

    logVerbose(CS,"CSController::stopAllRSEMediaApp()-->OUT");
}

void CSController::startRSEHomeApp(void)
{
    logVerbose(CS,"CSController::startRSEHomeApp()-->IN");

    ama_AMPID_t amPID;

    auto pLRSEHomeTask = std::make_shared<PLCTask>();
    pLRSEHomeTask->SetType(PLC_CMD_START);
    amPID = AMPID_APP(E_SEAT_RSE1,E_APPID_HOME);
    pLRSEHomeTask->SetAMPID(amPID);
    pLRSEHomeTask->SetUserData(1);
    TaskDispatcher::GetInstance()->Dispatch(pLRSEHomeTask);

    auto pRRSEHomeTask = std::make_shared<PLCTask>();
    pRRSEHomeTask->SetType(PLC_CMD_START);
    amPID = AMPID_APP(E_SEAT_RSE2,E_APPID_HOME);
    pRRSEHomeTask->SetAMPID(amPID);
    pRRSEHomeTask->SetUserData(1);
    TaskDispatcher::GetInstance()->Dispatch(pRRSEHomeTask);

    logVerbose(CS,"CSController::startRSEHomeApp()-->OUT");
}

void CSController::setCurrentSource(const std::vector<IntentInfo> &infoVector)
{
    FUNCTION_SCOPE_LOG_C(CS);

    for(auto& source: infoVector)
        logInfo(CS, LOG_HEAD, "new source: ", source.itemID, "__", source.action, "__", source.data, "__", source.isQuiet);

    lastSourceManager.SetCurrentSource(infoVector);
}

void CSController::getCurrentSource(std::vector<IntentInfo> &infoVector)
{
    FUNCTION_SCOPE_LOG_C(CS);

    infoVector = lastSourceManager.GetCurrentSource();
}

void CSController::saveCurrentSource()
{
    FUNCTION_SCOPE_LOG_C(CS);

    lastSourceManager.Serialize();
}

void CSController::getLastSource(std::vector<IntentInfo> &infoVector)
{
    DomainVerboseLog chatter(CS, __STR_FUNCTIONP__);

    infoVector = lastSourceManager.GetLastSource();
}

void CSController::onModuleStartup()
{
    DomainVerboseLog chatter(CS, __STR_FUNCTIONP__);

    // update EOL setting to filesystem
    {
        auto eolInfo = rp.getEOLInfo();
#ifdef CODE_FOR_EV
        // set AVM/RVC
        SetCameraMode(true);
        // RSE enable/disable
        EnableRSE(false);
#else
        // set AVM/RVC
        SetCameraMode(eolInfo.RI_AVS);
        // RSE enable/disable
        EnableRSE(eolInfo.RPE);
#endif
    }

    // send last source
    {
        // get last source
        auto lastSourceSet = lastSourceManager.GetLastSource();

        // make sure AppList is ready
        auto plcTask = std::make_shared<PLCTask>();
        plcTask->SetPriority(E_TASK_PRIORITY_HIGH);
        plcTask->SetType(PLC_QUERY_IS_MODULE_INITIALIZED);
        bool isIntime = SendQueryTaskNWait(plcTask, 4000); // because that the timeout of Loading applist is 3000ms
        if(!isIntime)
            logError(CS, LOG_HEAD, "waiting for PLC to finish its intialization is timeout!");

        // send last source intent
        bool isScreenOccupied = false; // IVI screen occupation
        for(auto &source: lastSourceSet)
        {
            logInfo(CS, LOG_HEAD, "new source: ", source.itemID, "__", source.action, "__", source.data, "__", source
                .isQuiet);
            auto app = AppList::GetInstance()->GetAMPID(source.itemID);
            if(app == INVALID_AMPID)
            {
                logError(CS, LOG_HEAD, "invalid last source: ", source.itemID);
                continue;
            }
            DispatchIntentTask(app, source);

            if(IS_APP(app) && source.isQuiet == false)
                isScreenOccupied = true;
        }

        // start home if IVI screen is not occupied
        if(!isScreenOccupied)
        {
            logWarn(CS, LOG_HEAD, "no visible last source is found! start home.");
            DispatchIntentTask(HOME_AMPID(E_SEAT_IVI), IntentInfo());
        }
    }
}
