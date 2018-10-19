/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <COMMON/UniqueID.h>

#include "HMIC/HMICSpy.h"
#include "AppState.h"
#include "ProcessInfo.h"
#include "HMICCtrl.h"
#include "HMICTask.h"
#include "PLCTask.h"
#include "AppState.h"
#include "TASK/TaskDispatcher.h"
#include "TASK/TaskHandlerManager.h"
#include "ActiveApp.h"
#include "ILMException.h"
#include "ILMClient.h"
#include "Screen.h"
#include "Layer.h"
#include "ZOrder.h"
#include "TitlebarUI.h"
#include "VirtualScreenManager.h"
#include "AppShareManager.h"
#include "Capi.h"
#include "IMEUI.h"
#include "SurfaceCreater.h"
#include "DoubleTapRecognizer.h"
#include "TapRecognizer.h"
#include "ScreenSetting.h"
#include "SurfaceDestroyBroadcaster.h"
#include "PMCTask.h"
#include "PMCtrl.h"
#include "UnitManager.h"
#include "SMCtrl.h"
#include "Configuration.h"
#include "AppProfileManager.h"
#include "ServiceHMIC.h"
#include "ReleaseLayer.h"
#include "RebootFrequency.h"
#include "ReadXmlConfig.h"
#include "DataBase.h"
#include "COMMON/NotifyWaiter.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

HMIController* HMIController::instance = nullptr;
const std::map<E_SEAT, RedrawCallback> HMIController::touchForbidderRedrawCB = {
    {E_SEAT_IVI, &HMIController::drawTouchForbidderPictureIVI},
    {E_SEAT_RSE1, &HMIController::drawTouchForbidderPictureRSE1},
    {E_SEAT_RSE2, &HMIController::drawTouchForbidderPictureRSE2},
};

const std::map<E_SEAT, RedrawCallback> HMIController::animationPlayerRedrawCB = {
    {E_SEAT_IVI, &HMIController::drawAPPictureIVI},
    {E_SEAT_RSE1, &HMIController::drawAPPictureRSE1},
    {E_SEAT_RSE2, &HMIController::drawAPPictureRSE2},
};

HMIController::HMIController()
: screenMonitorHandle(0)
, objExistanceCBHandle(0)
{
    // //data initial
    // if(Initialize() != E_OK)
    // {
    //     logWarn("HMIController initialize failed!");
    // }
}

HMIController::~HMIController()
{
    DomainVerboseLog chatter(PROF, "HMIController::~HMIController()");

    if(screenMonitorHandle)
        Notify(screenMonitorHandle);
    logInfo(HMIC, LOG_HEAD, "waiting for screen monitor to exit...");
    if(screenMonitor)
        screenMonitor->join();
    logInfo(HMIC, LOG_HEAD, "screen monitor has exited.");

    Destroy();
    HMICSpy::isScreenStatusReady = false;
    HMICSpy::screens.clear();
}

HMIController* HMIController::GetInstance()
{
    return instance;
}

void HMIController::SetInstance(HMIController* hmic)
{
    instance = hmic;
}

void HMIController::doTask(shared_ptr<Task> task)
{
    assert(task.get());
    auto pTask = dynamic_pointer_cast<HMICTask>(task);

    // feed the dog
    if (pTask->IsWatchDotTask()) {
        wdSetState(E_TASK_ID_HMIC, WATCHDOG_SUCCESS);
        return ;
    }

    logVerbose(HMIC, "HMIController::doTask()-->IN");
    // do the task
    AMPID ampid = pTask->GetAMPID();
    logInfo(HMIC,"doTask==>"" taskType=", pTask->GetTaskTypeString()," AMPID=", ToHEX(ampid));
    switch(pTask->GetType())
    {
    case HMIC_NOTIFY_MODULE_INITIALIZED:
        OnModuleInitialized(pTask);
        break;
    case HMIC_NOTIFY_APP_START:
        OnAppStart(pTask);
        break;
    case HMIC_NOTIFY_APP_STOP:
        OnAppStop(ampid);
        break;
    case HMIC_NOTIFY_APP_SHOW:
        OnAppShow(ampid);
        break;
    case HMIC_NOTIFY_APP_HIDE:
        OnAppHide(ampid);
        break;
    case HMIC_NOTIFY_APP_CRUSH:
        OnUIProcessCrush(ampid);
        break;
    case HMIC_NOTIFY_APP_SWITCH_TITLEBAR_VISIBILITY:
        OnAppSwitchTitlebarVisibility(pTask);
        break;
    case HMIC_NOTIFY_TITLEBAR_DROPDOWNSTATECHANGE:
        OnTitlebarDropDownStateChange(pTask);
        break;
    case HMIC_NOTIFY_TITLEBAR_EXPAND:
        OnTitlebarExpand(ampid);
        break;
    case HMIC_NOTIFY_TITLEBAR_SHRINK:
        OnTitlebarShrink(ampid);
        break;
    case HMIC_NOTIFY_ILM_OBJ_CHANGED:
        OnILMOBJChanged(pTask);
        break;
    case HMIC_NOTIFY_SURFACE_AVAILABLE:
        OnSurfaceAvailable(pTask);
        break;
    case HMIC_NOTIFY_SURFACE_CREATED:
        OnSurfaceCreated(pTask);
        break;
    case HMIC_NOTIFY_SURFACE_DESTROYED:
        OnSurfaceDestroyed(pTask);
        break;
    case HMIC_NOTIFY_SURFACE_DESTINATION_RECTANGLE_CHANGED_ON_RELEASE_LAYER:
        OnSurfaceRectangleChangedOnReleaseLayer(pTask);
        break;
    case HMIC_NOTIFY_ANIMATION_STARTUP_PLAY:
        OnPlayStartupAnimation(ampid);
        break;
    case HMIC_NOTIFY_ANIMATION_STARTUP_STOP:
        OnStopStartupAnimation(ampid);
        break;
    case HMIC_NOTIFY_IME_SHOW:
        OnShowIME(pTask);
        break;
    case HMIC_NOTIFY_IME_HIDE:
        OnStopIME(pTask);
        break;
    case HMIC_NOTIFY_TOUCHDEVICE_DESTROYED:
    case HMIC_NOTIFY_TOUCHDEVICE_CREATED:
        OnTouchDeviceChanged(pTask);
        break;
    case HMIC_NOTIFY_ILM_FATALERROR:
        OnILMFatalError(pTask);
        break;
    case HMIC_CMD_SYNC_APP_START:
        OnSynchronizeAppStart(pTask);
        break;
    case HMIC_CMD_SYNC_APP_END:
        OnSynchronizeAppEnd(pTask);
        break;
    case HMIC_CMD_ENABLE_TOUCH_APP:
        OnEnableTouchApp(pTask);
        break;
    case HMIC_CMD_ENABLE_CHILD_SAFETY_LOCK:
        OnEnableChildSafetyLock(pTask);
        break;
    case HMIC_CMD_ENABLE_TOUCH_SCREEN:
        OnEnableTouchScreen(pTask);
        break;
    case HMIC_CMD_SET_MASK_SUR_VISIBLE:
        OnSetMaskSurfaceVisible(pTask);
        break;
    case HMIC_CMD_ENABLE_GESTURE_RECOGNIZER:
        OnEnableGR(pTask);
        break;
    case HMIC_CMD_TITLEBAR_DROPDOWN:
        OnDropDownTitlebar(pTask);
        break;
    case HMIC_CMD_REMOVE_APP_FROM_ZORDER:
        OnRemoveAppFromZOrder(pTask);
        break;
    case HMIC_CMD_ADD_EXTRA_SURFACE:
        OnAddExtraSurface(pTask);
        break;
    case HMIC_CMD_START_SCREEN_MONITOR:
        OnStartScreenMonitor(pTask);
        break;
    case HMIC_CMD_CHECK_SCREEN_STATUS:
        OnCheckScreenStatus(pTask);
        break;
    case HMIC_QUERY_TITLEBAR_VISIBILITY_FOR_SCREEN:
        OnTitlebarVisibilityQueryForScreen(pTask);
        break;
    case HMIC_QUERY_TITLEBAR_DROPDOWNSTATE:
        OnTitlebarDropDownStateQuery(pTask);
        break;
    case HMIC_QUERY_SYNC_GET_CURRENT_SHARE:
        OnGetCurrentShare(pTask);
        break;
    case HMIC_QUERY_SYNC_SOURCE_SURFACES:
        OnGetSourceSurfacesQuery(pTask);
        break;
    case HMIC_QUERY_SYNC_WORKING_LAYERS:
        OnGetWorkingLayersQuery(pTask);
        break;
    case HMIC_QUERY_GET_AVAILABLE_SCREENS:
        OnGetAvailableScreens(pTask);
        break;
    case HMIC_QUERY_GET_SCREEN_SIZE:
        OnGetScreenSize(pTask);
        break;
    case HMIC_REQUEST_ALL_SCREEN_SHOT:
        GetAllScreenShot();
        break;
    default:
        break;
    };
    logVerbose(HMIC, "HMIController::doTask()-->OUT");
}

int HMIController::HMICmkdir(const char* path)
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
void HMIController::CheckScreenShotNumber(const char* path)
{

    logVerbose(HMIC,"HMIController::CheckScreenShotNumber()--> IN  ");
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(path); //打开目录指针
    std::vector<char *> fileList;
    while ((dp = readdir(dirp)) != NULL) { //通过目录指针读目录
        if (0==strcmp(dp->d_name,".")||0==strcmp(dp->d_name,".."))
            continue;

        fileList.push_back(dp->d_name);
        // logDebugF(HMIC," HMIController::CheckScreenShotNumber %s", dp->d_name );
    }
    std::string minPath="";
    std::string begin="";
    if (fileList.size()>36){
        minPath=std::string (*fileList.begin());
        begin =std::string(*fileList.begin());
        begin=begin.substr (5,19);
    }
    if (fileList.size()>36)
    {
        for (std::vector<char *>::iterator it = fileList.begin() ; it != fileList.end(); ++it){

            std::string current=std::string(*it);
            current=current.substr (5,19);
            int retVal=begin.compare(current);
            if (retVal>0){
                minPath=std::string(*it);
                begin=minPath.substr (5,19);
                logDebugF(HMIC,"begin: %s  current: %s value: %d ",begin.c_str(), current.c_str(),retVal);
            }
        }
        extern int errno;
        minPath=std::string(path)+"/"+minPath;
        int errorCode = remove(minPath.c_str());
        if (0!=errorCode){
            logErrorF(HMIC,"delete file : %s failed with errno: %d" ,minPath.c_str(),errno);
        }else
            logDebugF(HMIC,"delete file : %s success" ,minPath.c_str());
    }

    //
    (void) closedir(dirp); //关闭目录

    logVerbose(HMIC,"HMIController::CheckScreenShotNumber()--> OUT  ");
}

void HMIController::GetAllScreenShot()
{
    logVerbose(HMIC,"HMIController::getAllScreenShot()--> IN  ");

    if (access("/tmp/ivi_srcshot_switch", F_OK) != 0)
    {
        logWarnF(HMIC,"/tmp/ivi_srcshot_switch is not exist! get screenshot failed! return !!");
        return;
    }
    int i=0;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int w = 1280;
    unsigned int h = 720;
    static std::string format="bmp";
    std::string screenDesp[4]={"CLST","_IVI","RSEL","RSER"};
    time_t timep;
    time(&timep);
    struct tm* p = gmtime(&timep);
    char *defaultname = NULL;
    const char *path = "/media/ivi-data/EMMC_Root/screenshot";
    HMICmkdir(path);
   // Screen screen;
    auto ilmScreens = GetAllScreens();
    for(auto& screen: ilmScreens)
    {
        CheckScreenShotNumber(path);
        unsigned int tempScreenID=screen.GetID();
        Section _section =screen.GetRectangle();
        switch (_section.width)
        {
            case 1920://cluster
                x=0;y=0;w=1920;h=720;i=0;
                break;
            case 1280://ivi
                x=0;y=0;w=1280;h=720;i=1;
                break;
            case 2560://rse1
                x=0;y=0;w=1280;h=720;i=2;
                break;
            default:
                logDebugF(HMIC," getAllScreenShot() create get unknown screen.witdh: %d ",_section.width);
                break;
        }
        asprintf(&defaultname, "%s/%s_%04d%02d%02d%02d%02d%02d.%s", path,screenDesp[i].c_str(),
                 p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, format.c_str());

        screen.TakeScreenShotEx(tempScreenID,defaultname,  x,y,w,h, format.c_str());
        logDebugF(HMIC," getAllScreenShot() create screen x:%d  y:%d  w:%d  h:%d  i:%d ",x,y,w,h,i);
        logDebugF(HMIC," getAllScreenShot() create screen :%d path:%s ok!",tempScreenID,defaultname);
        if(defaultname != NULL) {
            ama_free(defaultname);
        }
        if (2560==_section.width)//rse2 again
        {
            x=1280;y=0;w=1280;h=720;i=3;
            asprintf(&defaultname, "%s/%s_%04d%02d%02d%02d%02d%02d.%s", path,screenDesp[i].c_str(),
                     p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, format.c_str());

            screen.TakeScreenShotEx(tempScreenID,defaultname,  x,y,w,h, format.c_str());
            logDebugF(HMIC," getAllScreenShot() create screen x:%d  y:%d  w:%d  h:%d  i:%d ",x,y,w,h,i);
            logDebugF(HMIC," getAllScreenShot() create screen :%d path:%s ok!",tempScreenID,defaultname);
            if(defaultname != NULL) {
                ama_free(defaultname);
            }
        }
    }
//    for (int i=0;i<4;i++ )
//    {
//        CheckScreenShotNumber(path);
//        unsigned int tempScreenID=0;
//        w=(i==0)?1920:1280;
//        x=(i==3)?1280:0;
//        tempScreenID=(i==3)?2:i;
//        asprintf(&defaultname, "%s/%s_%04d%02d%02d%02d%02d%02d.%s", path,screenDesp[i].c_str(),
//                 p->tm_year+1900, p->tm_mon+1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, format.c_str());
//
//        screen.TakeScreenShotEx(tempScreenID,defaultname,  x,y,w,h, format.c_str());
//        logDebugF(HMIC," getAllScreenShot() create screen :%d path:%s ok!",i,defaultname);
//        if(defaultname != NULL) {
//            ama_free(defaultname);
//        }
//    }

    logVerbose(HMIC,"HMIController::getAllScreenShot()--> OUT ");
}
void HMIController::SetTopApp(AMPID app)
{
    auto iterZOrder = zOrder.find(GET_SEAT(app));
    if(iterZOrder!=zOrder.end())
        zOrder[GET_SEAT(app)]->SetTopApp(app);
}

AMPID HMIController::GetTopApp(E_SEAT seat) const
{
    auto iterZOrder = zOrder.find(seat);
    if(iterZOrder!=zOrder.end())
        return iterZOrder->second->GetTopApp();
    else
        return INVALID_AMPID;
}

void HMIController::RemoveApp(AMPID app)
{
    auto iterZOrder = zOrder.find(GET_SEAT(app));
    if(iterZOrder!=zOrder.end())
        zOrder[GET_SEAT(app)]->RemoveApp(app);
}

void HMIController::ClearZOrder(E_SEAT seat)
{
    auto iterZOrder = zOrder.find(seat);
    if(iterZOrder!=zOrder.end())
        zOrder[seat]->Clear();
}

void HMIController::OnModuleInitialized(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnModuleInitialized()");

    //data initial
    if(Initialize() != E_OK)
    {
        logWarn("HMIController initialize failed!");
    }

    //CreateBackGround();
    CreateTouchForbidder();
    CreateIVIMaskSurface();
    if(IsSysUIMemOptmz())
    {
        logDebug(HMIC, "HMIController::OnModuleInitialized(): use GR for DCS");
        CreateAPPicture();
        CreateAPSurface();
        CreateGRSurface();
        CreateGestureRecognizer();
        RegisterGestureCallback();
    }
    else
    {
        logDebug(HMIC, "HMIController::OnModuleInitialized(): use KANZI for DCS");
        StartDCS();
    }
    APManager.SetSwitcher([=](AMPID from, AMPID to){
        SwitchAP(from, to);
    });
}

void HMIController::Show(AMPID uiProc)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::Show(%#x)", uiProc));

    auto UIProcObj = GetUIProc(uiProc);
    auto UI = GetHMI(uiProc);

    if(UIProcObj && UI)
    {
        UI->AddSurfaces2PreShowLayer();
        UI->SetVisible();
#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->OnPreshowIn(uiProc);
#endif // ENABLE_DEBUG_MODE
        UIProcObj->PreShowNotify();
#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->OnPreshowOut(uiProc);
#endif // ENABLE_DEBUG_MODE
        UI->RemoveSurfacesFromPreShowLayer();
        EnableTouchApp(uiProc, UI->IsTouchable());
        UI->AddSurfaces2Layer();
        UIProcObj->StopStartupAnimationNotify();
        UIProcObj->ShowNotify();
        UIProcObj->SetState(make_shared<ShowState>());

        AppStatusChangeNotify(uiProc, UIProcObj->GetStatus());
    }
    else
        logError(HMIC, "HMIController::Show(): specified process/UI is not found!");
}

void HMIController::Hide(AMPID uiProc)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::Hide(%#x)", uiProc));

    auto UIProcObj = GetUIProc(uiProc);
    auto UI = GetHMI(uiProc);

    if(UIProcObj && UI)
    {
        UI->RemoveSurfacesFromLayer();
        UI->AddSurfaces2PreHideLayer();
//        UI->SetVisible();
        UIProcObj->PreHideNotify();
        //UI->SetVisible(false);
        //UI->RemoveSurfacesFromPreHideLayer();
        UIProcObj->HideNotify();
        UIProcObj->SetState(make_shared<HideState>());

        AppStatusChangeNotify(uiProc, UIProcObj->GetStatus());
    }
    else
        logError(HMIC, "HMIController::Hide(): specified process/UI is not found!");
}

void HMIController::OnAppShow(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnAppShow(%#x)", app));

    auto appObj = GetApp(app);
    auto appUIHMI = GetHMI(app);
    E_SEAT seat = GET_SEAT(app);

    AMPID topApp = GetTopApp(seat);
    auto topAppObj = GetApp(topApp);
    auto topAppUIHMI = GetHMI(topApp);

    // show app
    if(appObj)
    {
        if(GET_APPID(app)==E_APPID_HOME)
            ClearZOrder(seat);
        SetTopApp(app);
        // OK to show
        if((GetTopApp(seat)==app)&&(appUIHMI))
        {
            // show specified APP
            appObj->ResumeNotify();
            Show(app);
            appObj->SwitchTitlebarVisibilityNotify(appUIHMI->IsTitlebarVisible(), appUIHMI->GetTitlebarVisibilitySwitchStyle());

            // hide original APP
            if(topAppObj && !topAppObj->IsTheSame(appObj))
            {
                Hide(topApp);

                // show home APP means you "shut down"
                // every background APP
                if(GET_APPID(app)==E_APPID_HOME)
                {
                    ClearZOrder(seat);
                    SetTopApp(app);
                }
            }
        }
    }
    // show UI service
    else
    {
        Show(app);
        if(GET_APPID(app)==E_APPID_TITLEBAR)
        {
            std::string log = "echo \"titlebar " + std::to_string(GET_SEAT(app)) + " is operational\" > /dev/kmsg";
            system(log.c_str());
        }
    }
}

void HMIController::OnAppHide(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnAppHide(%#x)", app));

    auto appObj = GetApp(app);
    auto appUIHMI = GetHMI(app);
    E_SEAT seat = GET_SEAT(app);

    AMPID topApp = GetTopApp(seat);

    // hide app
    if(appObj)
    {
        // if specified APP is visible, hide it
        if(app==topApp)
        {
            RemoveApp(app);
            AMPID nextApp = GetTopApp(seat);
            auto nextAppObj = GetApp(nextApp);
            auto nextAppUIHMI = GetHMI(nextApp);

            // the only APP
            if(!nextAppObj)
            {
                // HOME is not existing, start it.
                if(GET_APPID(app) != E_APPID_HOME)
                {
                    // start home
                    auto plcTask = make_shared<PLCTask>();
                    plcTask->SetType(PLC_CMD_START);
                    plcTask->SetAMPID(HOME_AMPID(seat));
                    TaskDispatcher::GetInstance()->Dispatch(plcTask);
                }
                // HOME will not hide if there's no other APP
                else
                {
                    SetTopApp(app);
                    return;
                }
            }

            if(nextAppUIHMI)
            {
                // show next APP
                nextAppObj->ResumeNotify();
                Show(nextApp);
                nextAppObj->SwitchTitlebarVisibilityNotify(nextAppUIHMI->IsTitlebarVisible(), nextAppUIHMI
                    ->GetTitlebarVisibilitySwitchStyle());

                // hide specified APP
                Hide(app);
            }
        }
    }
    // hide UI-service
    else
        Hide(app);
}

void HMIController::OnAppStart(shared_ptr<HMICTask> pTask)
{
    AMPID app = pTask->GetAMPID();
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnAppStart(%#x)", app));

    auto appObj = GetApp(app);
    auto appUIHMI = GetHMI(app);
    E_SEAT seat = GET_SEAT(app);

    AMPID topApp = GetTopApp(seat);
    auto topAppObj = GetApp(topApp);
    auto topAppUIHMI = GetHMI(topApp);

    // start app
    if(appObj&&appUIHMI)
    {
        // don't do anything if user tries to
        // start current app
        if(app==topApp)
            return;

        bool isBKGBoot = static_cast<bool>(pTask->GetUserData());
        bool isVisible = false;

        if(!isBKGBoot)
        {
            if(GET_APPID(app)==E_APPID_HOME)
                ClearZOrder(seat);
            SetTopApp(app);
            isVisible = GetTopApp(seat) == app;
        }

        // visible start
        if(isVisible && !isBKGBoot)
        {
            // start specified APP
            std::string keyApp;
            GetConfigFromString("KeyApp", keyApp);
            if(app==AppList::GetInstance()->GetAMPID(keyApp))
                logDebug(PROF, "HMIController::OnAppStart(): showing app: ", ToHEX(app)," ...");
            logDebug(HMIC, "HMIController::OnAppStart(): start app ", ToHEX(app));
            appUIHMI->ConfigureExistedSurfaces();
            appObj->InitializeNotify(isVisible);
            Show(app);
            appObj->StopStartupAnimationNotify();
            appObj->SwitchTitlebarVisibilityNotify(appUIHMI->IsTitlebarVisible(), appUIHMI->GetTitlebarVisibilitySwitchStyle());
            if(app==AppList::GetInstance()->GetAMPID(keyApp))
                logDebug(PROF, "HMIController::OnAppStart(): app: ", ToHEX(app)," is visible!");

            // hide original APP
            Hide(topApp);
        }
        // invisible start
        else
        {
            logDebug(HMIC, "HMIController::OnAppStart(): start invisible/background app ", ToHEX(app));
            appUIHMI->ConfigureExistedSurfaces();
            appObj->InitializeNotify(isVisible);
            Hide(app);
        }
    }
    // start UI service
    else
    {
        auto UIProcObj = GetUIProc(app);
        auto UIProcHMI = GetHMI(app);
        if(UIProcObj && UIProcHMI)
        {
            UIProcHMI->ConfigureExistedSurfaces();
            UIProcObj->InitializeNotify(1, false);
            Hide(app);
        }
    }
}

void HMIController::OnAppStop(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnAppStop(%#x)", app));

    auto appObj = GetApp(app);
    auto appUIHMI = GetHMI(app);
    E_SEAT seat = GET_SEAT(app);

    // stop app
    if(appObj)
    {
        // stop visible APP
        if( app==GetTopApp(seat) )
        {
            RemoveApp(app);
            AMPID nextApp = GetTopApp(seat);

            // only one APP is visible
            if(nextApp==INVALID_AMPID)
            {
                // start home
                Surface home(SURFACEID(AMPID_APP(seat, E_APPID_HOME), LAYERID(seat, E_LAYER_NORMALAPP)));
                auto plcTask = make_shared<PLCTask>();
                plcTask->SetType(PLC_CMD_START);
                plcTask->SetAMPID(HOME_AMPID(seat));
                TaskDispatcher::GetInstance()->Dispatch(plcTask);
                nextApp = plcTask->GetAMPID();
            }

            // show next APP
            if(nextApp!=INVALID_AMPID)
            {
                auto nextAppObj = GetApp(nextApp);
                auto nextAppUIHMI = GetHMI(nextApp);

                if(nextAppObj&&nextAppUIHMI)
                {
                    nextAppObj->ResumeNotify();
                    Show(nextApp);
                    nextAppObj->SwitchTitlebarVisibilityNotify(nextAppUIHMI->IsTitlebarVisible(), nextAppUIHMI->GetTitlebarVisibilitySwitchStyle());
                }
            }

            // stop specified APP
            if(appUIHMI){
                appUIHMI->SetVisible(false);
                appUIHMI->RemoveSurfacesFromLayer();
            }
            appObj->SetState(make_shared<StoppingState>());
            appObj->StopNotify();
        }
        // stop invisible APP
        else
        {
            RemoveApp(app);
            if(appUIHMI){
                appUIHMI->SetVisible(false);
                appUIHMI->RemoveSurfacesFromLayer();
            }
            appObj->SetState(make_shared<StoppingState>());
            appObj->StopNotify();
        }
    }
    // stop UI service
    else
    {
        auto procObj = GetUIProc(app);
        if(procObj)
        {
            procObj->SetState(make_shared<StoppingState>());
            procObj->StopNotify();
        }
    }
}

void HMIController::OnUIProcessCrush(AMPID ampid)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnUIProcessCrush(%#x)", ampid));

    // get app state
    bool isForeground = true;
    auto appInfo = ProcessInfo::GetInstance();
    appInfo->Lock();
    auto procObj = appInfo->FindUIProc(ampid);
    if(procObj && typeid(*(procObj->GetState().get()))== typeid(HideState))
        isForeground = false;
    appInfo->Unlock();

    // show next app
    // as the surface of specified app is destroyed,
    // there would surely be exception thrown out.
    // just ignore them, it's ok.
    try
    {
        OnAppStop(ampid);
    }
    catch (ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in HMIController::OnUIProcessCrush().");
    }

    // clear app information
    appInfo->Lock();
    appInfo->RemoveProc(ampid);
    appInfo->Unlock();

    RebootFrequency::GetInstance()->SetCrushed(ampid);

    if(!RebootFrequency::GetInstance()->IsTooFrequent(ampid))
    {
        // prepare intent information.
        IntentInfo intentInfo = {"", "", !isForeground};

        // restart crushed app
        DispatchIntentTask(ampid, intentInfo);
    }
}

void HMIController::OnTitlebarExpand(AMPID titlebar)
{
    logVerbose(HMIC, "HMIController::OnTitlebarExpand(", ToHEX(titlebar), ")--->IN");

    // auto appObj = GetApp(titlebar);
    auto appUIHMI = GetHMI(titlebar);
    E_SEAT seat = GET_SEAT(titlebar);

    auto titlebarUI = dynamic_pointer_cast<TitlebarUI>(appUIHMI);
    if(titlebarUI)
    {
        titlebarUI->OnExpand();
    }
    else
    {
        logError(HMIC, "no active tool bar app!");
    }

    logVerbose(HMIC, "HMIController::OnTitlebarExpand(", ToHEX(titlebar), ")--->OUT");
}

void HMIController::OnTitlebarShrink(AMPID titlebar)
{
    logVerbose(HMIC, "HMIController::OnTitlebarShrink(", ToHEX(titlebar), ")--->IN");

    // auto appObj = GetApp(titlebar);
    auto appUIHMI = GetHMI(titlebar);
    E_SEAT seat = GET_SEAT(titlebar);

    auto titlebarUI = dynamic_pointer_cast<TitlebarUI>(appUIHMI);
    if(titlebarUI)
    {
        titlebarUI->OnShrink();
    }
    else
    {
        logError(HMIC, "no active tool bar app!");
    }

    logVerbose(HMIC, "HMIController::OnTitlebarShrink(", ToHEX(titlebar), ")--->OUT");
}

void HMIController::SwitchTitlebarVisibility(AMPID app, bool isShow, int style)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x, %s, %d)", app, isShow?"true":"false", style));

    auto appObj = GetApp(app);
    auto appUIHMI = GetHMI(app);

    if(appUIHMI)
    {
        appUIHMI->SetTitlebarVisible(isShow);
        appUIHMI->SetTitlebarVisibilitySwitchStyle(style);
    }

    AMPID topApp = GetTopApp(GET_SEAT(app));
    logDebug(HMIC, "HMIController::SwitchTitlebarVisibility(): app = ", ToHEX(app));
    logDebug(HMIC, "HMIController::SwitchTitlebarVisibility(): topApp = ", topApp);

    if(appObj && app==topApp && appUIHMI)
        appObj->SwitchTitlebarVisibilityNotify(appUIHMI->IsTitlebarVisible(), appUIHMI->GetTitlebarVisibilitySwitchStyle());
}

void HMIController::OnAppSwitchTitlebarVisibility(shared_ptr<HMICTask> pTask)
{
    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);
    AMPID app = hmicTask?hmicTask->GetAMPID():0;
    auto property = reinterpret_cast<std::tuple<bool, int>*>(hmicTask?hmicTask->GetUserData():0);
    bool isShow = property?std::get<0>(*property):true;
    int style = property?std::get<1>(*property):0;
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x, %s, %d)", app, isShow?"true":"false", style));

    if(hmicTask)
        SwitchTitlebarVisibility(app, isShow, style);
}

void HMIController::OnTitlebarDropDownStateChange(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnTitlebarDropDownStateChange(%#x)", pTask->GetAMPID()));

    auto titlebar = pTask->GetAMPID();
    auto seat = GET_SEAT(titlebar);
    bool isDropDown = static_cast<bool>(pTask->GetUserData());
    logDebug(HMIC, "HMIController::OnTitlebarDropDownStateChange(): titlebar[", ToHEX(titlebar), "]'s dropdown state is: ", isDropDown?"drop-down":"withdraw");
    if(screens.find(seat)!=screens.end())
        titlebarDropDownState[seat] = isDropDown;
}

typedef std::tuple<ilmObjectType, t_ilm_uint, t_ilm_bool> ILMOBJChangeArgs;
void HMIController::OnILMOBJChanged(shared_ptr<HMICTask> pTask)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    auto args = reinterpret_cast<ILMOBJChangeArgs*>(pTask->GetUserData());
    ilmObjectType object = std::get<0>(*args);
    t_ilm_uint id = std::get<1>(*args);
    t_ilm_bool created = std::get<2>(*args);

    try
    {
        if(object == ILM_SURFACE)
        {
            Surface surface(id);

            if(created)
            {
                logDebug(HMIC, "surface", ToHEX(id), "created");

                std::string keyApp;
                GetConfigFromString("KeyApp", keyApp);
                if(GET_AMPID(id)==AppList::GetInstance()->GetAMPID(keyApp))
                    logDebug(PROF, "HMIController::callbackFunction(): app:", ToHEX(GET_AMPID(id)), "'s surface is created.");

#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->OnSurfaceCreated(GET_AMPID(id));
#endif // ENABLE_DEBUG_MODE

                Dimension dimension = {0, 0};
                try
                {
                    dimension = surface.GetDimension();
                }
                catch(ILMException& e)
                {
                    logWarn(HMIC, e.what(), " is caught in HMIController::callbackFunction.");
                }

                try
                {
                    surface.SetNotifyCallback(&HMIController::surfaceCallback, true);
                }
                catch(ILMException& e)
                {
                    logWarn(HMIC, e.what(), " is caught in HMIController::callbackFunction.");
                }
                if(dimension.width == 0 && dimension.height == 0)
                {
                    logDebug(HMIC, "HMIController::callbackFunction(): surface", ToHEX(id), "dimension.width==0 && dimension.height==0 !");
                    logDebug(HMIC, "HMIController::callbackFunction(): wait for configuration notify of surface ", ToHEX(id));
                    return;
                }

                auto hmicTask = make_shared<HMICTask>();
                hmicTask->SetSurfaceID(id);
                hmicTask->SetType(HMIC_NOTIFY_SURFACE_CREATED);
                hmicTask->SetPriority(GET_APPID(GET_AMPID(id))==E_APPID_HOME?E_TASK_PRIORITY_HIGH:E_TASK_PRIORITY_NORMAL);
                TaskDispatcher::GetInstance()->Dispatch(hmicTask);
            }
            else if(!created)
            {
                logDebug(HMIC, "surface", ToHEX(id), "destroyed");

                // main surface's destroy means that APP is crushed.
                auto proc = GetAMPID(surface);
                auto procObj = GetUIProc(proc);
                auto procUI = GetHMI(proc);
                if(procObj && procObj->IsOperating() && procUI && procUI->IsMainSurface(surface))
                    SendCrushNotify(proc);

                auto hmicTask = make_shared<HMICTask>();
                hmicTask->SetSurfaceID(id);
                hmicTask->SetType(HMIC_NOTIFY_SURFACE_DESTROYED);
                TaskDispatcher::GetInstance()->Dispatch(hmicTask);

                // broadcast that specified surface is destroyed
                surfaceDestroyBroadcaster.NotifyAll([&](SurfaceDestroyCallback callback) {
                    if(callback)
                        callback(surface);
                });
            }
        }
    }
    catch(...)
    {
        logWarn(HMIC, "unexpected exception is caught in HMIController::callbackFunction()");
    }
}

void HMIController::OnSurfaceAvailable(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnSurfaceAvailable");

    auto hmicTask = std::dynamic_pointer_cast<HMICTask>(pTask);
    if(hmicTask)
    {
        SurfaceID surfaceID = hmicTask->GetSurfaceID();
        Surface surface(surfaceID);
        SetSurfaceOriginalSize(surface);
    }
}

void HMIController::OnSurfaceCreated(shared_ptr<HMICTask> pTask)
{
    logVerbose(HMIC, "HMIController::OnSurfaceCreated()--->IN");
    auto hmicTask = std::dynamic_pointer_cast<HMICTask>(pTask);
    if(hmicTask)
    {
        SurfaceID surfaceID = hmicTask->GetSurfaceID();
        Surface surface(surfaceID);
        ConfigureSurface(surface);
    }
    logVerbose(HMIC, "HMIController::OnSurfaceCreated()--->OUT");
}

void HMIController::OnSurfaceDestroyed(shared_ptr<HMICTask> pTask)
{
    logVerbose(HMIC, "HMIController::OnSurfaceDestroyed()--->IN");
    auto hmicTask = std::dynamic_pointer_cast<HMICTask>(pTask);
    if(hmicTask)
    {
        SurfaceID surfaceID = hmicTask->GetSurfaceID();
        Surface surface(surfaceID);
        DestroySurface(surface);
    }
    logVerbose(HMIC, "HMIController::OnSurfaceDestroyed()--->OUT");
}

void HMIController::OnSurfaceRectangleChangedOnReleaseLayer(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnSurfaceRectangleChangedOnReleaseLayer(%#x)", pTask->GetSurfaceID()));

    try
    {
        // check if current surface is on a release layer.
        bool isOnReleaseLayer = false;
        Surface surface(pTask->GetSurfaceID());
        LayerID layerID = surface.GetContainerLayer();
        E_LAYER layerEnum = GET_LAYER(layerID);
        isOnReleaseLayer = (layerEnum==E_LAYER_RELEASE_HMI || layerEnum==E_LAYER_RELEASE_PLAY);
        if(!isOnReleaseLayer)
        {
            logWarn(HMIC, "HMIController::OnSurfaceRectangleChangedOnReleaseLayer(): surface[", ToHEX(surface.GetID()), "] is not on a release layer while it is in Rectangle cache! Does any one change this surface manually? If not, this might be a hint to some bug.");
        }

        auto properties = reinterpret_cast<SurfaceProperties*>(pTask->GetUserData());
        if(isOnReleaseLayer && properties)
        {
            //logWarn(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " 's destination rectangle is changed on release layer.");
            ReleaseLayer releaseLayer(layerID);
            releaseLayer.FixSurfaceDestinationRectangle(surface);

            logInfoF(HMIC, "HMIController::OnSurfaceRectangleChangedOnReleaseLayer(): rectangle cache is updated: Section{%d, %d, %u, %u)", properties->destX, properties->destY, properties->destWidth, properties->destHeight);
        }
    }
    catch(ILMException& e)
    {
        logError(HMIC, "HMIController::OnSurfaceRectangleChangedOnReleaseLayer(): exception: ", (&e)->what());
    }
}

void HMIController::OnPlayStartupAnimation(AMPID ampid)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", ampid));

    APManager.ShowAP(ampid);
}

void HMIController::OnStopStartupAnimation(AMPID ampid)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", ampid));

    APManager.HideAP(ampid);

#ifdef ENABLE_DEBUG_MODE
    AppProfileManager::GetInstance()->OnLoadingScreenEnd(ampid);
#endif // ENABLE_DEBUG_MODE
}

void HMIController::OnShowIME(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnShowIME()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnShowIME(): null task pointer");
        return;
    }

    // get IME's AMPID
    AMPID target = hmicTask->GetAMPID();
    E_SEAT seat = GET_SEAT(target);
    AMPID IME = AMPID_SERVICE(seat, E_APPID_IME);

    auto IMEObj = GetUIProc(IME);
    auto IMEUIHMI = GetHMI(IME);
    auto rawIMEUIHMI = dynamic_pointer_cast<IMEUI>(IMEUIHMI);

    if(IMEObj && IMEUIHMI && rawIMEUIHMI)
    {
        rawIMEUIHMI->SetTargetApp(target);

        OnAppShow(IME);
    }
}

void HMIController::OnStopIME(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnStopIME()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnStopIME(): null task pointer");
        return;
    }

    // get IME's AMPID
    AMPID target = hmicTask->GetAMPID();
    E_SEAT seat = GET_SEAT(target);
    AMPID IME = AMPID_SERVICE(seat, E_APPID_IME);

    auto IMEObj = GetUIProc(IME);
    auto IMEUIHMI = GetHMI(IME);
    auto rawIMEUIHMI = dynamic_pointer_cast<IMEUI>(IMEUIHMI);

    if(IMEObj && IMEUIHMI && rawIMEUIHMI)
    {
        OnAppHide(IME);

        rawIMEUIHMI->SetTargetApp(INVALID_AMPID);
    }
}

void HMIController::OnTouchDeviceChanged(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnTouchDeviceChanged()");
    try
    {
        // update touch panels
        UpdateTouchDevice();

        // associate surfaces to touch panel
        E_SEAT seat = static_cast<E_SEAT>(pTask->GetAMPID());
        if(touchPanels.find(seat)!=touchPanels.end())
        {
            logDebug(HMIC, "HMIController::OnTouchDeviceChanged(): set input focus for surfaces on screen ", seat);

            auto surfaces = GetAllSurfaces();
            for(auto& surface: surfaces)
                if(GetSeat(surface)==seat)
                    SetInputDevice(surface);
        }
    }
    catch(ILMException& e)
    {
        logError(HMIC, "HMIController::OnTouchDeviceChanged(): exception: ", (&e)->what());
    }
}

void HMIController::OnSynchronizeAppStart(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnSynchronizeAppStart()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnSynchronizeAppStart(): null task pointer");
        return;
    }

    // source surfaces
    auto sourceApp = hmicTask->GetAMPID();

    if(sourceApp==INVALID_AMPID)
    {
        logWarn(HMIC, "HMIController::OnSynchronizeAppStart(): source app is invalid: ", ToHEX(sourceApp));
        return;
    }

    auto tuple = reinterpret_cast<std::tuple<E_SEAT, bool>*>(hmicTask->GetUserData());
    if(!tuple)
    {
        logWarn(HMIC, "HMIController::OnSynchronizeAppStart(): null tuple pointer");
        return;
    }

    E_SEAT destinationSeat = std::get<0>(*tuple);
    bool isMove = std::get<1>(*tuple);
    delete tuple;

    AMPID sync = AMPID_APP(destinationSeat, E_APPID_SYNC_APP);

    if(AppShareManager::GetInstance()->GetContext(sync))
    {
        logWarn(HMIC, "HMIController::OnSynchronizeAppStart(): already existed synchronization!");
        return;
    }

    auto context = make_shared<AppShareContext>();
    context->destinationScreen = destinationSeat;
    context->sourceApp = sourceApp;
    context->isMove = isMove;
    AppShareManager::GetInstance()->AddContext(sync, context);

    auto plcTask = make_shared<PLCTask>();
    plcTask->SetAMPID(sync);
    plcTask->SetType(PLC_CMD_START);
    TaskDispatcher::GetInstance()->Dispatch(plcTask);
}

void HMIController::OnSynchronizeAppEnd(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnSynchronizeAppEnd()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnSynchronizeAppEnd(): null task pointer");
        return;
    }

    AMPID app = hmicTask->GetAMPID();
    E_SEAT seat = static_cast<E_SEAT>(hmicTask->GetUserData());

    // stop sync
    auto contexts = AppShareManager::GetInstance()->GetContext([app, seat](std::shared_ptr<AppShareContext> context)->bool{
        return context->sourceApp==app && context->destinationScreen==seat;
    });
    if(!contexts.empty())
    {
        AMPID sync = AMPID_APP(seat, E_APPID_SYNC_APP);

        auto syncObj = GetApp(sync);
        if(syncObj && syncObj->IsStarting())
        {
            SendCrushNotify(syncObj);
        }
        else
        {
            auto plcTask = make_shared<PLCTask>();
            plcTask->SetAMPID(sync);
            plcTask->SetType(PLC_CMD_STOP);
            TaskDispatcher::GetInstance()->Dispatch(plcTask);
        }

        AppShareManager::GetInstance()->RemoveContext(sync);
    }
    else
        logWarn(HMIC, "HMIController::OnSynchronizeAppEnd(): share context is not found!");
}

void HMIController::OnDropDownTitlebar(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnDropDownTitlebar(%#x)", pTask->GetAMPID()));

    auto app = pTask->GetAMPID();
    auto seat = GET_SEAT(app);
    std::tuple<bool, unsigned int>* param = reinterpret_cast<std::tuple<bool, unsigned int>*>(pTask->GetUserData());
    auto isDropDown = std::get<0>(*param);
    auto style = std::get<1>(*param);
    logDebug(HMIC, "HMIController::OnDropDownTitlebar(): requester = ", ToHEX(app), "isDropDown = ", isDropDown, ", style = ", style);
    if(titlebarDropDownState.find(seat)!=titlebarDropDownState.end() && (isDropDown!=titlebarDropDownState[seat]) )
    {
        logDebug(HMIC, "HMIController::OnDropDownTitlebar(): sending notify...");
        SwitchTitlebarDropDownStateNotify(AMPID_SERVICE(seat, E_APPID_TITLEBAR), isDropDown, style);
        logDebug(HMIC, "HMIController::OnDropDownTitlebar(): done.");
    }
}

void HMIController::OnRemoveAppFromZOrder(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", pTask->GetAMPID()));

    AMPID toBeRemovedApp = pTask->GetAMPID();
    auto topApp = GetTopApp(GET_SEAT(toBeRemovedApp));
    if(toBeRemovedApp == topApp)
    {
        logWarn(HMIC, LOG_HEAD, "current app is visible! hide it.");
        OnAppHide(topApp);
    }
    else
    {
        logInfo(HMIC, LOG_HEAD, "remove app ", ToHEX(toBeRemovedApp), " from zorder.");
        RemoveApp(pTask->GetAMPID());
    }
}

// TODO: attach this extra surface to its owner-systemd-unit/owner-app
// so that we are able to handle the situation that the requester
// forgets to unset its extra surface.
void HMIController::OnAddExtraSurface(shared_ptr<HMICTask> pTask)
{
    auto surfaceID = pTask->GetSurfaceID();
    auto layerID = pTask->GetUserData();

    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x, %#x)", surfaceID, layerID));

    if(layerID==INVALID_ID)
    {
        logInfo(HMIC, LOG_HEAD, "delete extra surface mapping: ", ToHEX(surfaceID));
        extraSurfaceMapping.erase(Surface(surfaceID));
    }
    else
        extraSurfaceMapping[Surface(surfaceID)] = Layer(layerID);
}

void HMIController::OnStartScreenMonitor(shared_ptr<HMICTask> pTask)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    screenMonitorHandle = GetUniqueID();
    RegisterWaiterID(screenMonitorHandle);

    screenMonitor = std::make_shared<std::thread>([this](){
        do
        {
            auto hmicTask = std::make_shared<HMICTask>();
            hmicTask->SetType(HMIC_CMD_CHECK_SCREEN_STATUS);
            TaskDispatcher::GetInstance()->Dispatch(hmicTask);
        }while(!Wait(screenMonitorHandle, 5000));
        logInfo(HMIC, "screenMonitor: screen monitor exits.");
    });
}

void HMIController::OnCheckScreenStatus(shared_ptr<HMICTask> pTask)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    for(auto screen: screens)
    {
        Layer hmiLayer(LAYERID(screen.first, E_LAYER_NORMALAPP));
        auto surfaces = hmiLayer.GetAllSurfaces();
        if(surfaces.empty())
        {
            logWarn(HMIC, LOG_HEAD, "screen ", screen.first, " is empty! start home.");
            DispatchIntentTask(HOME_AMPID(screen.first), IntentInfo());
        }
    }
}

void HMIController::OnTitlebarVisibilityQueryForScreen(shared_ptr<HMICTask> pTask)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);
    typedef std::tuple<bool>* TuplePTR;

    auto procObj = GetUIProc(hmicTask->GetAMPID());
    if(procObj)
    {
        auto topApp = GetTopApp(procObj->GetSeat());
        if(topApp!=INVALID_AMPID)
        {
            auto topAppUIHMI = GetHMI(topApp);
            if(topAppUIHMI)
            {
                REF_ARG(hmicTask, TuplePTR, 0) = topAppUIHMI->IsTitlebarVisible();
                logInfo(HMIC, LOG_HEAD, "titlebar's visibility of app ", ToHEX(hmicTask->GetAMPID()), " is: ", topAppUIHMI->IsTitlebarVisible());
            }
            else
                logError(HMIC, LOG_HEAD, "UI object not found!");
        }
        else
            logError(HMIC, LOG_HEAD, "failed getting top app!");
    }
    else
        logError(HMIC, LOG_HEAD, "no such process: ", ToHEX(hmicTask->GetAMPID()));
}

void HMIController::OnTitlebarDropDownStateQuery(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnTitlebarDropDownStateQuery()");

    auto requester = pTask->GetAMPID();
    auto seat = GET_SEAT(requester);
    auto reply = reinterpret_cast<std::function<void(unsigned int)>*>(pTask->GetUserData());

    if(!reply)
        return;

    if(titlebarDropDownState.find(seat)!=titlebarDropDownState.end())
        (*reply)(titlebarDropDownState[seat]);
    else
        (*reply)(false);
}

void HMIController::OnGetCurrentShare(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnGetCurrentShare()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnGetCurrentShare(): null task pointer");
        return;
    }

    // source surfaces
    auto sync = hmicTask->GetAMPID();
    auto reply = reinterpret_cast<std::function<void(unsigned int)>*>(hmicTask->GetUserData());

    if(sync)
    {
        if(reply)
        {
            auto context = AppShareManager::GetInstance()->GetContext(sync);
            if(context)
                (*reply)(context->sourceApp);
            else
                (*reply)(INVALID_AMPID);
        }
    }
    else if(reply)
        (*reply)(INVALID_AMPID);

    delete reply;
}

void HMIController::OnGetSourceSurfacesQuery(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnGetSourceSurfacesQuery()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnGetSourceSurfacesQuery(): null task pointer");
        return;
    }

    // source surfaces
    auto sourceApp = hmicTask->GetAMPID();
    auto reply = reinterpret_cast<std::function<void(std::vector<SurfaceID>)>*>(hmicTask->GetUserData());

    std::vector<SurfaceID> rtv;
    if(sourceApp)
    {
        if(reply)
        {
            auto surfaces = AppShareManager::GetInstance()->GetSourceSurfaces(sourceApp);
            for(auto &surface: surfaces)
                rtv.push_back(surface.GetID());
            (*reply)(rtv);
        }
    }
    else if(reply)
        (*reply)(rtv);

    delete reply;
}

void HMIController::OnGetWorkingLayersQuery(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnGetWorkingLayersQuery()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(!hmicTask)
    {
        logWarn(HMIC, "HMIController::OnGetWorkingLayersQuery(): null task pointer");
        return;
    }

    // source surfaces
    auto sourceApp = hmicTask->GetAMPID();
    auto reply = reinterpret_cast<std::function<void(std::vector<LayerID>, bool)>*>(hmicTask->GetUserData());

    std::vector<LayerID> rtv;
    if(reply)
    {
        if(sourceApp)
        {
            bool rtvb = false;
            auto context = AppShareManager::GetInstance()->GetContext(sourceApp);
            if(context)
                rtvb = context->isMove;

            auto layers = AppShareManager::GetInstance()->GetWorkingLayers(sourceApp);
            for(auto& layer: layers)
                rtv.push_back(layer.GetID());

            (*reply)(rtv, rtvb);
        }
        else
            (*reply)(rtv, false);
    }

    delete reply;
}

void HMIController::OnGetAvailableScreens(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::OnGetAvailableScreens()"));

    auto hmicTask = std::dynamic_pointer_cast<HMICTask>(pTask);
    auto seats = reinterpret_cast<vector<E_SEAT>*>(hmicTask->GetUserData());

    if(seats)
        *seats = GetAvailableScreens();
}

void HMIController::OnGetScreenSize(shared_ptr<HMICTask> pTask)
{
    FUNCTION_SCOPE_LOG_C(HMIC);
    typedef std::tuple<unsigned int, unsigned int, unsigned int>* TuplePTR;

    logDebug(HMIC, LOG_HEAD, "arg[0] = ", REF_ARG(pTask, TuplePTR, 0));
    auto seat = static_cast<E_SEAT>(REF_ARG(pTask, TuplePTR, 0));
    if(screens.find(seat)!=screens.end())
    {
        auto size = screens[seat]->GetSize();
        REF_ARG(pTask, TuplePTR, 1) = size.width;
        REF_ARG(pTask, TuplePTR, 2) = size.height;
        logInfo(HMIC, LOG_HEAD, "seat id: ", seat, " width: ", size.width, " height: ", size.height);
    }
    else
        logWarn(HMIC, LOG_HEAD, "invalid seat id: ", seat);
}

void HMIController::EnableTouchApp(AMPID app, bool enable)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::EnableTouchApp(%#x, %s)", app, enable?"true":"false"));

    //EnableTouch(GET_SEAT(app), E_LAYER_TOUCH_FORBIDDEN_NORMALAPP, enable);
}

void HMIController::EnableTouch(E_SEAT seat, E_LAYER layer, bool enable)
{
    Surface sc_surface(SURFACEID_TF(seat, layer));
    sc_surface.SetVisible(!enable);
}

void HMIController::OnEnableTouchApp(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnEnableTouchApp()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(hmicTask)
    {
        AMPID app = hmicTask->GetAMPID();
        bool enable = static_cast<bool>(hmicTask->GetUserData());

        auto appObj = GetApp(app);
        auto appUIHMI = GetHMI(app);

        if(appUIHMI)
        {
            appUIHMI->SetTouchable(enable);
        }

        AMPID topApp = GetTopApp(GET_SEAT(app));
        logDebug(HMIC, "HMIController::OnEnableTouchApp(): app = ", ToHEX(app));
        logDebug(HMIC, "HMIController::OnEnableTouchApp(): topApp = ", topApp);

        if(appObj && app==topApp)
            EnableTouchApp(app, enable);
    }
}

void HMIController::OnEnableChildSafetyLock(shared_ptr<HMICTask> hmicTask)
{
    DomainVerboseLog chatter(HMIC, __STR_FUNCTIONP__);

    E_SEAT seat = GET_SEAT(hmicTask->GetAMPID());
    auto paraTuple = reinterpret_cast<BSTuple*>(hmicTask->GetUserData());
    if(paraTuple)
    {
        bool enable = std::get<0>(*paraTuple);
        //        std::string png = std::get<1>(*paraTuple);

        logDebug(HMIC, LOG_HEAD, "enable = ", enable?"true":"false");
        auto startCL = std::make_shared<PLCTask>();
        startCL->SetType(enable?PLC_CMD_START:PLC_CMD_HIDE);
        startCL->SetAMPID(AMPID_SERVICE(seat, E_APPID_CHILD_LOCK));
        TaskDispatcher::GetInstance()->Dispatch(startCL);
    }
}

void HMIController::OnSetMaskSurfaceVisible(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnSetMaskSurfaceVisible()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(hmicTask)
    {
        E_SEAT seat   = GET_SEAT(hmicTask->GetAMPID());
        bool   enable = static_cast<bool>(hmicTask->GetUserData());

        logDebug(HMIC, "HMIController::OnSetMaskSurfaceVisible(): enable=",enable);
        Surface MaskSurface(SURFACEID_MASK(seat));

        MaskSurface.SetVisible(enable);
    }
}

void HMIController::OnEnableTouchScreen(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnEnableTouchScreen()");

    auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

    if(hmicTask)
    {
        E_SEAT seat   = GET_SEAT(hmicTask->GetAMPID());
        if(screens.find(seat)==screens.end())
        {
            logWarn(HMIC, LOG_HEAD, "invalid seat: ", seat);
            return;
        }

        auto paraTuple = reinterpret_cast<BSTuple*>(hmicTask->GetUserData());
        if(paraTuple)
        {
            bool enable = std::get<0>(*paraTuple);
            std::string png = std::get<1>(*paraTuple);

            SetPicture4TouchForbidder(seat, png);
            EnableTouch(seat, E_LAYER_TOUCH_FORBIDDEN_EVERYTHING, enable);
        }
    }
}

void HMIController::OnEnableGR(shared_ptr<HMICTask> pTask)
{
    DomainVerboseLog chatter(HMIC, "HMIController::OnEnableGR()");

    try
    {
        auto hmicTask = dynamic_pointer_cast<HMICTask>(pTask);

        if(hmicTask)
        {
            E_SEAT seat   = GET_SEAT(hmicTask->GetAMPID());
            bool   enable = static_cast<bool>(hmicTask->GetUserData());

            if(screens.find(seat)==screens.end())
            {
                logWarn(HMIC, LOG_HEAD, "invalid seat: ", seat);
                return;
            }

            logDebug(HMIC, "HMIController::OnEnableGR(): enable = ", enable?"true":"false");
            Surface GRSurface(SURFACEID_GR(seat));
            DCSConfigureTimeVisibility[seat] = enable;// WARNING: if you do this inside the "if"-branch below, you may set wrong visibility in HMIController::ConfigureAMGRSurface() because you didn't update this value when it goes "else"-branch.
            if(!GRSurface.IsConfigured())
            {
                logWarn(HMIC, "HMIController::OnEnableGR(): surface ", ToHEX(GRSurface.GetID()), " is not configured yet.");
                logWarn(HMIC, "HMIController::OnEnableGR(): the visibility is recorded, it will be applyed after the surface is configured.");
            }
            else
                GRSurface.SetVisible(enable);
        }
    }
    catch(std::exception& e)
    {
        logError(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
}

void HMIController::OnILMFatalError(shared_ptr<HMICTask> pTask)
{
    DomainLog chatter(HMIC, LOG_WARN, formatText("HMIController::OnILMFatalError()"));

    //not ready. HMIC restart will crush AMGR for now. we are working on this.
    logError(HMIC, "HMIController::OnILMFatalError(): restartint HMIC will crush AMGR for now. we are working on this.");
    return;

    // restart HMIC
    std::thread hmicRestarter([](){
        DomainVerboseLog chatter(HMIC, "hmicRestarter()");
        try
        {
            auto globalObj = TaskHandlerManager::GetInstance();
            if(globalObj)
            {
                globalObj->RestartHandler("HMIC", [](){return std::make_shared<ServiceHMIC>();});
            }
            else
                logError(HMIC, "hmicRestarter(): global object is not found!");
        }
        catch(std::exception& e)
        {
            logError(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
        }
        catch(...)
        {
            logError(HMIC, LOG_HEAD, "unexpected exception is caught!");
        }
    });
    hmicRestarter.detach();
}

void HMIController::PlayStartupAnimation(AMPID proc)
{
    auto appObj = GetApp(proc);
    if(appObj)
    {
        // check animation-player's state
        E_SEAT seat = GET_SEAT(proc);
        if(!IsSysUIMemOptmz())
        {
            AMPID animationPlayer = AP_AMPID(seat);
            auto  procInfo        = ProcessInfo::GetInstance();
            procInfo->Lock();
            auto animationPlayerObj = procInfo->FindProc(animationPlayer);
            procInfo->Unlock();
            if(!animationPlayerObj
               || animationPlayerObj->IsStopping()
               || animationPlayerObj->IsStarting())
            {
                logWarn(HMIC
                        , "HMIController::OnPlayStartupAnimation(): animation player is not running, no animation then.");
                return;
            }
        }

        // validate seat
        if(screens.find(seat)==screens.end())
        {
            logWarn(HMIC, LOG_HEAD, "invalid seat: ", seat);
            return;
        }

        // if animation-player is OK, play the animation.
        Surface apSurface;
        if(IsSysUIMemOptmz())
            apSurface.SetID(GetSurfaceID(AMPID_AMGR(seat, E_APPID_AP)));
        else
            apSurface.SetID(GetSurfaceID(AP_AMPID(seat)));
        try
        {
            LayerID layerID = appObj->GetUI()->GetLayerID();
            E_LAYER eLayer = GET_LAYER(layerID);
            eLayer = static_cast<E_LAYER >(static_cast<unsigned int>(eLayer) + 2);
            Layer layer(LAYERID(seat, eLayer));
            layer.AddSurface(apSurface);
            ILMClient::Commit();
        }
        catch (ILMException& e)
        {
            logWarn(HMIC, (&e)->what(), "caught in HMIController::OnPlayStartupAnimation()");
        }

        try
        {
            apSurface.SetVisible();
            ILMClient::Commit();
        }
        catch (ILMException& e)
        {
            logWarn(HMIC, (&e)->what(), "caught in HMIController::OnPlayStartupAnimation()");
        }

        // send start playing notify
        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto ap = procInfo->FindUIProc(AP_AMPID(seat));
        if(ap && ap->IsOperating())
            ::PlayStartupAnimationNotify(proc);
        procInfo->Unlock();
    }
}

void HMIController::StopStartupAnimation(AMPID proc)
{
    // WARN: when you try to stop the animation, its app information
    // might have gone already. so you can't assume that there STILL
    // is any app object.
    // let me make it short for you: DON'T DO THIS: "GetApp(ampid)" !!
    // you'll get nothing.
    // auto appObj = GetApp(ampid);
    // if(appObj)
    // {
    try
    {
        E_SEAT seat = GET_SEAT(proc);

        // send stop playing notify
        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto ap = procInfo->FindUIProc(AP_AMPID(seat));
        if(ap && ap->IsOperating())
            ::StopStartupAnimationNotify(proc);
        procInfo->Unlock();

        // hide surface
        Surface apSurface;
        if(IsSysUIMemOptmz())
            apSurface.SetID(GetSurfaceID(AMPID_AMGR(seat, E_APPID_AP)));
        else
            apSurface.SetID(GetSurfaceID(AP_AMPID(seat)));

        if(screens.find(seat)==screens.end())
        {
            logWarn(HMIC, LOG_HEAD, "invalid seat: ", seat);
            return;
        }

        apSurface.SetVisible(false);

        E_LAYER eLayer = GET_LAYER(GetSurfaceID(proc));
        eLayer = static_cast<E_LAYER >(static_cast<unsigned int>(eLayer) + 2);
        Layer layer(LAYERID(seat, eLayer));
        layer.RemoveSurface(apSurface);
        ILMClient::Commit();
    }
    catch (ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in HMIController::OnStopStartupAnimation()");
    }
    // }
}

void HMIController::SwitchAP(AMPID old, AMPID neW)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::SwitchAP(%#x, %#x)", old, neW));

    // single operation
    if(INVALID_AMPID == (old&neW))
    {
        logDebug(HMIC, "HMIController::SwitchAP(): single operation.");

        if(INVALID_AMPID!=old)
            StopStartupAnimation(old);

        if(INVALID_AMPID!=neW)
            PlayStartupAnimation(neW);

        return;
    }
    else // switch
    {
        logDebug(HMIC, "HMIController::SwitchAP(): AP switch.");
        // remove from the old
        try
        {
            ::StopStartupAnimationNotify(old);
            E_SEAT  seat = GET_SEAT(old);
            Surface apSurface;
            if(IsSysUIMemOptmz())
                apSurface.SetID(GetSurfaceID(AMPID_AMGR(seat, E_APPID_AP)));
            else
                apSurface.SetID(GetSurfaceID(AP_AMPID(seat)));

            E_LAYER eLayer = GET_LAYER(GetSurfaceID(old));
            eLayer = static_cast<E_LAYER >(static_cast<unsigned int>(eLayer) + 2);
            Layer layer(LAYERID(seat, eLayer));
            layer.RemoveSurface(apSurface);
        }
        catch(ILMException& e)
        {
            logWarn(HMIC, e.what(), " caught in HMIController::OnStopStartupAnimation()");
        }

        // add to the new
        PlayStartupAnimation(neW);

        // commit
        ILMClient::Commit();
    }
}

void HMIController::SetInputDevice(Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", surface.GetID()));

    E_SEAT seat = GetSeat(surface);
    if(touchPanels.find(seat)!=touchPanels.end())
        if(surface.IsConfigured())
            surface.SetInputDevice(touchPanels[seat]);
        else
            logError(HMIC, LOG_HEAD, "unconfigured surface: ", ToHEX(surface.GetID()), " !");
}

shared_ptr<ActiveApp> HMIController::GetApp(AMPID ampid)
{
    return dynamic_pointer_cast<ActiveApp>(GetUIProc(ampid));
}

shared_ptr<UIProcess> HMIController::GetUIProc(AMPID ampid)
{
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto procObj = procInfo->FindProc(ampid);
    procInfo->Unlock();

    return dynamic_pointer_cast<UIProcess>(procObj);
}

shared_ptr<AppUIHMI> HMIController::GetHMI(AMPID ampid)
{
    auto app = GetUIProc(ampid);

    if(app)
        return dynamic_pointer_cast<AppUIHMI>(app->GetUI());
    else
        return nullptr;
}

std::vector<E_SEAT> HMIController::GetAvailableScreens()
{
    std::vector<E_SEAT> seats;
    for(auto& iter: screens)
        seats.push_back(iter.first);

    return std::move(seats);
}

void HMIController::SetPicture4TouchForbidder(E_SEAT seat, string png)
{
    if(screens.find(seat)==screens.end())
    {
        logWarn(HMIC, "HMIController::SetPicture4TouchForbidder(): invalid seat: ", seat);
        return;
    }

    std::shared_ptr<PNG> pic = nullptr;
    if(!png.empty())
        pic = std::make_shared<PNG>(png);
    touchForbidderPicture.Operate([&](PngMap &tfpMap){
        tfpMap[seat] = pic;
    });
    ForceRedrawSurface(SURFACEID_TF(seat, E_LAYER_TOUCH_FORBIDDEN_EVERYTHING));
}

void HMIController::CreateTouchForbidder()
{
    DomainVerboseLog chatter(HMIC, "HMIController::CreateTouchForbidder()");

//    for(auto& screen: screens)
//        normalAppTouchForbidder[screen.first] = std::make_shared<std::thread>(CreateSurface, SURFACEID_TF(screen.first, E_LAYER_TOUCH_FORBIDDEN_NORMALAPP), Transparent, nullptr);

//    for(auto& screen: screens)
//        childSafetyLockTF[screen.first] = std::make_shared<std::thread>(CreateSurface, SURFACEID_TF(screen.first, E_LAYER_TOUCH_FORBIDDEN_CHILD_SAFETY_LOCK), Transparent, nullptr);

    for(auto& screen: screens)
        globalTouchForbidder[screen.first] = std::make_shared<std::thread>(CreateSurface, SURFACEID_TF(screen.first, E_LAYER_TOUCH_FORBIDDEN_EVERYTHING), screen.second->GetSize().width, screen.second->GetSize().height, touchForbidderRedrawCB.at(screen.first), nullptr);

}

void HMIController::DeleteTouchForbidder()
{
    DomainVerboseLog chatter(PROF, "HMIController::DeleteTouchForbidder()");

//    for(auto& forbidder: normalAppTouchForbidder)
//        ::DestroySurface(SURFACEID_TF(forbidder.first, E_LAYER_TOUCH_FORBIDDEN_NORMALAPP));

//    for(auto& forbidder: childSafetyLockTF)
//        ::DestroySurface(SURFACEID_TF(forbidder.first, E_LAYER_TOUCH_FORBIDDEN_CHILD_SAFETY_LOCK));

    for(auto& forbidder: globalTouchForbidder)
        ::DestroySurface(SURFACEID_TF(forbidder.first, E_LAYER_TOUCH_FORBIDDEN_EVERYTHING));

//    for(auto& forbidder: normalAppTouchForbidder)
//    {
//        E_SEAT seat = forbidder.first;
//        forbidder.second->join();
//        logDebug(PROF, "HMIController::DeleteTouchForbidder(): touch forbidder ", ToHEX(SURFACEID_TF(seat, E_LAYER_TOUCH_FORBIDDEN_NORMALAPP)), " has exited.");
//    }
//    normalAppTouchForbidder.clear();

//    for(auto& forbidder: childSafetyLockTF)
//    {
//        E_SEAT seat = forbidder.first;
//        forbidder.second->join();
//        logDebug(PROF, "HMIController::DeleteTouchForbidder(): touch forbidder ", ToHEX(SURFACEID_TF(seat, E_LAYER_TOUCH_FORBIDDEN_CHILD_SAFETY_LOCK)), " has exited.");
//    }
//    childSafetyLockTF.clear();

    for(auto& forbidder: globalTouchForbidder)
    {
        E_SEAT seat = forbidder.first;
        forbidder.second->join();
        logDebug(PROF, "HMIController::DeleteTouchForbidder(): touch forbidder ", ToHEX(SURFACEID_TF(seat, E_LAYER_TOUCH_FORBIDDEN_EVERYTHING)), " has exited.");
    }
    globalTouchForbidder.clear();
}

void HMIController::CreateBackGround()
{
    DomainVerboseLog chatter(HMIC, "HMIController::CreateBackGround()");

    for(auto& screen: screens)
        background[screen.first] = std::make_shared<std::thread>(CreateSurface, SURFACEID_BKG(screen.first), screen.second->GetSize().width, screen.second->GetSize().height, Black, nullptr);
}

void HMIController::DeleteBackGround()
{
    DomainVerboseLog chatter(PROF, "HMIController::DeleteBackGround()");

    for(auto& bkg: background)
        ::DestroySurface(SURFACEID_BKG(bkg.first));

    for(auto& bkg: background)
    {
        E_SEAT seat = bkg.first;
        bkg.second->join();
        logDebug(PROF, "HMIController::DeleteBackGround(): background ", ToHEX(SURFACEID_BKG(seat)), " has exited.");
    }
    background.clear();
}

void HMIController::StartDCS()
{
    DomainVerboseLog chatter(HMIC, "HMIController::StartDCS()");

    // start DCS processes
    std::string name = "ama-app-double-click-scanner";
    std::string postfix = ".service";
    for(auto& screen: screens)
    {
        std::string argument = "@AMPID" + ToHEX(AMPID_AMGR(screen.first, E_APPID_GR));
        std::string unitName = name + argument + postfix;
        UnitManager::GetInstance()->StartUnit(unitName, nullptr, nullptr);
    }

    // initialize DCS visibility
    for(auto& screen: screens)
    {
        DCSConfigureTimeVisibility[screen.first] = !IsRearAlwaysON();
    }
}

void HMIController::StopDCS()
{
    DomainVerboseLog chatter(HMIC, "HMIController::StopDCS()");

    std::string name = "ama-app-double-click-scanner";
    std::string postfix = ".service";
    for(auto& screen: screens)
    {
        std::string argument = "@AMPID" + ToHEX(AMPID_AMGR(screen.first, E_APPID_GR));
        std::string unitName = name + argument + postfix;
        UnitManager::GetInstance()->StopUnit(unitName);
    }
}

void HMIController::CreateGRSurface()
{
    DomainVerboseLog chatter(HMIC, "HMIController::CreateGRSurface()");

    for(auto& screen: screens)
        gestureRecognizer[screen.first] = std::make_shared<std::thread>(CreateSurface, SURFACEID_GR(screen.first), screen.second->GetSize().width, screen.second->GetSize().height, Transparent, &HMIController::touchCallback);
}

void HMIController::DeleteGRSurface()
{
    DomainVerboseLog chatter(PROF, "HMIController::DeleteGRSurface()");

    for(auto& gr: gestureRecognizer)
        ::DestroySurface(SURFACEID_GR(gr.first));

    for(auto& gr: gestureRecognizer)
    {
        E_SEAT seat = gr.first;
        gr.second->join();
        logDebug(PROF, "HMIController::DeleteGRSurface(): gesture recognizer surface ", ToHEX(SURFACEID_GR(seat)), " has exited.");
    }
    gestureRecognizer.clear();
}

void HMIController::CreateIVIMaskSurface()
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    // get screen size
    Size screenSize = {1280, 720};
    if(screens.find(E_SEAT_IVI)!=screens.end())
    {
        screenSize = screens[E_SEAT_IVI]->GetSize();
    }
    else
        logError(HMIC, LOG_HEAD, "IVI screen is not found!");

    if(nullptr==mIVIMaskSurfaceThread)
        mIVIMaskSurfaceThread = std::make_shared<std::thread>(CreateSurface, SURFACEID_MASK(E_SEAT_IVI), screenSize.width, screenSize.height, Black, nullptr);
}

void HMIController::DeleteIVIMaskSurface()
{
    DomainVerboseLog chatter(PROF, "HMIController::DeleteIVIMaskSurface()");
    if(mIVIMaskSurfaceThread){
        ::DestroySurface(SURFACEID_MASK(E_SEAT_IVI));
        mIVIMaskSurfaceThread->join();
        mIVIMaskSurfaceThread = nullptr;
    }
}

void HMIController::CreateAPSurface()
{
    DomainVerboseLog chatter(HMIC, "HMIController::CreateAPSurface()");

    for(auto& screen: screens)
        animationPlayer[screen.first] = std::make_shared<std::thread>(CreateSurface, SURFACEID_AP_AMGR(screen.first), screen.second->GetSize().width, screen.second->GetSize().height, animationPlayerRedrawCB.at(screen.first), nullptr);
}

void HMIController::DeleteAPSurface()
{
    DomainVerboseLog chatter(PROF, "HMIController::DeleteAPSurface()");

    for(auto& ap: animationPlayer)
        ::DestroySurface(SURFACEID_AP_AMGR(ap.first));

    for(auto& ap: animationPlayer)
    {
        E_SEAT seat = ap.first;
        ap.second->join();
        logDebug(PROF, "HMIController::DeleteAPSurface(): animation player surface ", ToHEX(SURFACEID_AP_AMGR(seat)), " has exited.");
    }
    animationPlayer.clear();
}

void HMIController::SetRenderOrder(E_SEAT seat, shared_ptr<Screen> screen)
{
    logDebug(HMIC, "HMIController::SetRenderOrder()-->IN");
    logDebug(HMIC, "screen ID: ", screen->GetID());

    if(seat==E_SEAT_IVI || seat==E_SEAT_RSE1 || seat==E_SEAT_RSE2 || seat==E_SEAT_TEST)
    {
        std::vector<Layer> renderOrder;
        for(E_LAYER layerid = E_LAYER_BACKGROUND; layerid < E_LAYER_MAX; layerid=static_cast<E_LAYER>(static_cast<int>(layerid)+1))
        {
            renderOrder.push_back(Layer(LAYERID(seat, layerid)));
        }
        screen->SetRenderOrder(renderOrder);
    }
    else
        logError(HMIC, "HMIController::SetRenderOrder(): unexpected screen!");

    ILMClient::Commit();

    logDebug(HMIC, "HMIController::SetRenderOrder()-->OUT");
}

void HMIController::CreateGestureRecognizer()
{
    for(auto& gr: gestureRecognizer)
        doubleTapRecognizer[gr.first] = make_shared<DoubleTapRecognizer>();

    for(auto& gr: gestureRecognizer)
        singleTapRecognizer[gr.first] = make_shared<TapRecognizer>();
}

void HMIController::DeleteGestureRecognizer()
{
    DomainVerboseLog chatter(HMIC, "HMIController::DeleteGestureRecognizer()");

    doubleTapRecognizer.clear();
    singleTapRecognizer.clear();
}

void HMIController::CreateAPPicture()
{
    DomainVerboseLog chatter(HMIC, "HMIController::CreateAPPicture()");

    bool isEnglish = false;
    if(sSMKeyValuePairObserver.IsKeyExisting(SYS_LANG))
    {
        auto value = sSMKeyValuePairObserver.GetValue(SYS_LANG);
        isEnglish = value.value=="C";
    }

    auto apPicture = std::make_shared<PNG>(isEnglish?"/etc/ama.d/LoadingScreen_E.png":"/etc/ama.d/LoadingScreen_C.png");
    if(!apPicture->IsOK())
    {
        logWarn(HMIC, "HMIController::CreateAPPicture(): picture read failed.");
        return;
    }

    // initialize picture map
    for(auto& screen: screens)
        animationPlayerPicture.Operate([&screen, apPicture](PngMap& pngMap){
            pngMap[screen.first] = apPicture;
        });

    // handler language change
    sSMKeyValuePairObserver.AddObserver(SYS_LANG, &HMIController::SystemLanguageChangedCB, SMValueObserver::E_EVENT_ALL);
}

void HMIController::DeleteAPPicture()
{
    DomainVerboseLog chatter(HMIC, "HMIController::DeleteAPPicture()");

    // clear pictures
    animationPlayerPicture.Operate([](PngMap& pngMap){
        pngMap.clear();
    });
}

using namespace std::placeholders;
ama_Error_e HMIController::Initialize()
{
    ama_Error_e ret = E_UNKNOWN;
    try
    {
        logDebug(HMIC, "HMIController::initialize start");
        //ilm initial
        logDebug(PROF, "ILM init...");
        ILMClient::Initialize();
        logDebug(PROF, "ILM init OK");
        ILMClient::SetFatalErrorCB([this](unsigned int error){ILMFatalErrorCB(error);});
        UpdateTouchDevice();
        ScreenCreate();
        TitlebarDropDownStateInit();
//#ifdef CREATE_LAYERS_BY_AMGR
//        LayerCreate();
//        for(auto& iter :screens)
//        {
//            SetRenderOrder(iter.first, iter.second);
//        }
//#endif
        RegisterSurfaceChangeCallback();
        RegisterInputDeviceChangeCallback();
        logDebug(HMIC, "HMIController::initialize complete.");
        ret = E_OK;
    }
    catch (ILMException& e)
    {
        logError(HMIC, e.what(), "in", e.GetWhere(), "Caught in HMIController::initialize()!");
    }
    catch (...)
    {
        logError(HMIC, "Unexpected exception caught in HMIController::initialize()!");
    }
    return ret;
}

void HMIController::Destroy()
{
    if(IsSysUIMemOptmz())
    {
        // delete gesture recognizer
        DeleteGestureRecognizer();

        // delete gesture recognizer surface
        DeleteGRSurface();

        // delete animation player's surface
        DeleteAPSurface();

        // delete animation player's picture
        DeleteAPPicture();

        // delete the surface that mask all apps
        DeleteIVIMaskSurface();
    }
    else
        StopDCS();

    // delete touch forbidder
    DeleteTouchForbidder();

    // delete background
    //DeleteBackGround();

    // release callback
    UnregisterObjExistanceNotification(objExistanceCBHandle);

    //ilm deinitial
    ILMClient::Destroy();
}

bool IsClusterScreen(shared_ptr<const Screen> screen)
{
    // use cluster as screen 2, configure by a file.
    std::ifstream useClusterAsSRC2Config("/media/ivi-data/ama.d/useClusterAsSRC.txt");
    bool isUseClusterAsSRC2;

    isUseClusterAsSRC2 = useClusterAsSRC2Config.is_open();
    useClusterAsSRC2Config.close();

    if(isUseClusterAsSRC2)
        return false;

    // normal judgement
    Size size = screen->GetSize();

    return size.width==1920;
}

void HMIController::UpdateTouchDevice()
{
    DomainVerboseLog chatter(HMIC, "HMIController::UpdateTouchDevice()");

    if (!ILMClient::IsInitialized())
    {
        logError(HMIC, "HMIController::UpdateTouchDevice(): ILM is not initialized!");
        return;
    }

    touchPanels.clear();

    auto inputs = InputPeripheral::GetInputPeripheral(E_INPUTPERIPHERAL_TYPE_ALL);
    logDebug(HMIC, "HMIController::UpdateTouchDevice(): total inputs: ", inputs.size());
    for(auto& inputDevice: inputs)
    {
        if(inputDevice.GetName()=="ivi")
        {
            logInfo(HMIC, "HMIController::UpdateTouchDevice(): IVI touch device is found.");
            touchPanels[E_SEAT_IVI] = inputDevice;
        }
        if(inputDevice.GetName()=="rse_left")
        {
            logInfo(HMIC, "HMIController::UpdateTouchDevice(): RSE1 touch device is found.");
            touchPanels[E_SEAT_RSE1] = inputDevice;
        }
        if(inputDevice.GetName()=="rse_right")
        {
            logInfo(HMIC, "HMIController::UpdateTouchDevice(): RSE2 touch device is found.");
            touchPanels[E_SEAT_RSE2] = inputDevice;
        }
    }
}

void HMIController::TitlebarDropDownStateInit()
{
    DomainVerboseLog chatter(HMIC, "TitlebarDropDownStateInit()");

    Process::appStateChangeBroadcaster.Register([](AMPID app, E_PROC_STATE state){

        if(GET_APPID(app)!=E_APPID_TITLEBAR)
            return;
        if(state!=E_PROC_STATE_STARTING
            && state!=E_PROC_STATE_STOPPING
            && state!=E_PROC_STATE_DEAD)
            return;

        logInfo(HMIC, "HMIController::TitlebarDropDownStateInit(): app = ", ToHEX(app), " state = ", state);
        logDebug(HMIC, "HMIController::TitlebarDropDownStateInit(): initialize titlebar drop-down state.");
        auto hmicTask = std::make_shared<HMICTask>();
        hmicTask->SetType(HMIC_NOTIFY_TITLEBAR_DROPDOWNSTATECHANGE);
        hmicTask->SetAMPID(app);
        hmicTask->SetUserData(static_cast<unsigned long long>(false));
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);
    });
}

void HMIController::ScreenCreate()
{
    if (!ILMClient::IsInitialized())
    {
        logError(HMIC, "HMIController::ScreenCreate() ilm not initial!");
        return ;
    }
    std::vector<shared_ptr<Screen>> allScreens = VirtualScreenManager::GetInstance()->GetVirtualScreens();
    logDebug(HMIC, "HMIController::ScreenCreate() total screens: ",allScreens.size());
    for(int i=0;i<allScreens.size();i++)
    {
        Section section = allScreens[i]->GetSection();
        logDebugF(HMIC, "HMIController::ScreenCreate(): screen%d's section = {%d, %d, %d, %d}.", i, section.left, section.top, section.width, section.height);
        logDebug(HMIC, "HMIController::ScreenCreate(): screen ID: ", allScreens[i]->GetID());
        for(auto setting: ScreenSetting)
        {
            if(allScreens[i]->GetSection()!=setting.second)
                continue;
            if(IsScreenDisabled(setting.first))
            {
                logDebug(HMIC, "HMIController::ScreenCreate(): current screen is disabled! ignore.");
                continue;
            }
            if(!IsRSEEnabled() && (setting.first==E_SEAT_RSE1 || setting.first==E_SEAT_RSE2))
            {
                logDebug(HMIC, "HMIController::ScreenCreate(): RSE is disabled! current screen is ignored.");
                continue;
            }
            HMICSpy::screens.push_back(setting.first); // screen spy is collecting screen info.
            screens[setting.first] = allScreens[i];
            zOrder[setting.first] = make_shared<ZOrderWithAPPPriority>();
            logDebug(HMIC, "HMIController::ScreenCreate(): ", ScreenName.at(setting.first), " is created.");
        }
    }

    HMICSpy::isScreenStatusReady = true;
    logInfo(HMIC, LOG_HEAD, "screen spy is ready.");
}

void HMIController::LayerCreate()
{
    logDebug(HMIC, "HMIController::layerCreate()-->IN");

    //when AppMgr restart,all layers it create maybe already exist.
    map<Layer,bool> layerAlreadyExist;
    map<Layer,bool>::iterator itr;

    // collect existing layers
    for(auto& screen: screens)
    {
        auto allLayers = screen.second->GetAllLayers();
        for(auto& layer: allLayers)
            layerAlreadyExist[layer] = true;
    }

    for(E_SEAT seat = E_SEAT_TEST; seat < E_SEAT_NONE; seat=static_cast<E_SEAT>(static_cast<int>(seat)+1))
    {
        // create layers
        if (screens.find(seat) != screens.end())
        {
            logDebug(HMIC, "create layers for screen ", seat);
            Dimension dimension = screens[seat]->GetSize();
            Section section = screens[seat]->GetSection();
            Section rectangle = screens[seat]->GetRectangle();
            Section destinanRectangle;
#ifdef CREATE_LAYERS_BY_AMGR
            destinanRectangle = rectangle;
#else
            destinanRectangle = section;
#endif // CREATE_LAYERS_BY_AMGR
            logDebug(HMIC, "HMIController::layerCreate(): screen", seat, "'s dimension is ", dimension.width, "*", dimension.height);
            logDebug(HMIC, "HMIController::layerCreate(): screen", seat, "'s o is (",destinanRectangle.left, ",", destinanRectangle.top, ")");
            for (E_LAYER layerid = E_LAYER_BACKGROUND; layerid < E_LAYER_MAX; layerid=static_cast<E_LAYER>(static_cast<int>(layerid)+1) )
            {
                Layer layer(LAYERID(seat, layerid));
                itr = layerAlreadyExist.find(layer);
                if (itr != layerAlreadyExist.end())
                {
                    logDebug(HMIC, "layerCreate already exist Layer-", ToHEX(layerid));
                    continue;
                }

                try
                {
                    layer.Create(dimension);
                    layer.SetDestinationRectangle(destinanRectangle);
                    layer.SetVisible();
                }
                catch(ILMException& e)
                {
                    logWarn(HMIC, e.what(), " is caught in HMIController::layerCreate()");
                }
            }
            ILMClient::Commit();
        }
    }
    logDebug(HMIC, "HMIController::layerCreate()-->OUT");
}

void HMIController::ILMFatalErrorCB(unsigned int error)
{
    DomainLog chatter(HMIC, LOG_ERRO, formatText("HMIController::ILMFatalErrorCB(%d)", error));

    // print error message
    static std::map<unsigned int, std::string> errorMessage = {
        {ILM_EXCEPTION_NONE, "none"},
        {ILM_EXCEPTION_WL_UNKNOWN, "unknown error"},
        {ILM_EXCEPTION_WL_FLUSH, "weston display flush fail"},
        {ILM_EXCEPTION_WL_DISPATCH, "weston display dispatch fail"},
    };
    if(errorMessage.find(error)!=errorMessage.end())
        logError(HMIC, "HMIController::ILMFatalErrorCB(): error message: ", errorMessage[error]);

    // notify HMIC that ILM encounters fatal error.
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_ILM_FATALERROR);
    hmicTask->SetPriority(E_TASK_PRIORITY_HIGH);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void HMIController::callbackFunction(ilmObjectType object, t_ilm_uint id, t_ilm_bool created)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%u, %#x, %s)", object, id, created?"true":"false"));

    // create argument-container
    ILMOBJChangeArgs* args = new ILMOBJChangeArgs(object, id, created);

    // send task
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_ILM_OBJ_CHANGED);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(args)
                          , [](unsigned long long data){delete reinterpret_cast<ILMOBJChangeArgs*>(data);});
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void HMIController::surfaceCallback(SurfaceID surfaceID, SurfaceProperties* properties, NotificationMask mask)
{
    logVerbose(HMIC, "HMIController::surfaceCallback(", ToHEX(surfaceID), ")--->IN");

    vector<string> maskTypeList = {"0", "ILM_NOTIFICATION_VISIBILITY",
                                   "ILM_NOTIFICATION_OPACITY ",
                                   "ILM_NOTIFICATION_ORIENTATION ",
                                   "ILM_NOTIFICATION_SOURCE_RECT ",
                                   "ILM_NOTIFICATION_DEST_RECT ",
                                   "ILM_NOTIFICATION_CONTENT_AVAILABLE ",
                                   "ILM_NOTIFICATION_CONTENT_REMOVED ",
                                   "ILM_NOTIFICATION_CONFIGURED ",
                                   "ILM_NOTIFICATION_ALL "};

    //logDebug(HMIC, "HMIController::surfaceCallback(): mask = ", maskTypeList[mask]);
    logDebug(HMIC, "HMIController::surfaceCallback(): mask = ", mask);

    if(mask & ILM_NOTIFICATION_CONTENT_AVAILABLE)
    {
        logDebug(HMIC, "HMIController::surfaceCallback(): mask = ILM_NOTIFICATION_CONTENT_AVAILABLE");
        auto hmicTask = make_shared<HMICTask>();
        hmicTask->SetSurfaceID(surfaceID);
        hmicTask->SetType(HMIC_NOTIFY_SURFACE_AVAILABLE);
        hmicTask->SetPriority(GET_APPID(GET_AMPID(surfaceID))==E_APPID_HOME?E_TASK_PRIORITY_HIGH:E_TASK_PRIORITY_NORMAL);
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);

        std::string keyApp;
        GetConfigFromString("KeyApp", keyApp);
        if(GET_AMPID(surfaceID)==AppList::GetInstance()->GetAMPID(keyApp))
            logDebug(PROF, "HMIController::surfaceCallback(): app: ", ToHEX(GET_AMPID(surfaceID)), " surface content is available.");
    }
    if(mask & ILM_NOTIFICATION_CONFIGURED)
    {
        logDebug(HMIC, "HMIController::surfaceCallback(): mask = ILM_NOTIFICATION_CONFIGURED");

#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->OnSurfaceConfigured(GET_AMPID(surfaceID));
#endif // ENABLE_DEBUG_MODE

        auto hmicTask = make_shared<HMICTask>();
        hmicTask->SetSurfaceID(surfaceID);
        hmicTask->SetType(HMIC_NOTIFY_SURFACE_CREATED);
        hmicTask->SetPriority(GET_APPID(GET_AMPID(surfaceID))==E_APPID_HOME?E_TASK_PRIORITY_HIGH:E_TASK_PRIORITY_NORMAL);
        TaskDispatcher::GetInstance()->Dispatch(hmicTask);

        std::string keyApp;
        GetConfigFromString("KeyApp", keyApp);
        if(GET_AMPID(surfaceID)==AppList::GetInstance()->GetAMPID(keyApp))
            logDebug(PROF, "HMIController::surfaceCallback(): app: ", ToHEX(GET_AMPID(surfaceID)), " surface is configured.");
    }

    logVerbose(HMIC, "HMIController::surfaceCallback(", ToHEX(surfaceID), ")--->OUT");
}

void HMIController::touchCallback(SurfaceID surfaceID, E_TOUCH touchType, float x, float y, int fingerID, unsigned int id)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::touchCallback(surface=%#x, type=%d, x=%f, y=%f, fingure=%d, %u)", surfaceID, touchType, x, y, fingerID, id));

    auto This = HMIController::GetInstance();
    AMPID app = GET_AMPID(surfaceID);
    E_SEAT seat = GET_SEAT(app);

//    if(GET_APPID(app)!=E_APPID_TOUCH_FORBIDDER)
//        return;

    // double tap recognizer
    if(This->doubleTapRecognizer.find(seat)!=HMIController::GetInstance()->doubleTapRecognizer.end())
    {

        double tmpX = 0.0, tmpY = 0.0;
        This->doubleTapRecognizer[seat]->GetLastPosition(tmpX, tmpY);

        logDebug(HMIC, "HMIController::touchCallback(): double tap recognizer's state before touch: ", This->doubleTapRecognizer[seat]->GetStateString());
        if(touchType == E_TOUCH_DOWN)
            This->doubleTapRecognizer[seat]->TouchDown(x, y);
        else if(touchType == E_TOUCH_UP)
            This->doubleTapRecognizer[seat]->TouchUp(tmpX, tmpY);
        else if(touchType == E_TOUCH_MOVE)
            This->doubleTapRecognizer[seat]->TouchMove(x, y);
        else
            return;
        logDebug(HMIC, "HMIController::touchCallback(): double tap recognizer's state after touch: ", This->doubleTapRecognizer[seat]->GetStateString());
    }

    // single tap recognizer
    if(This->singleTapRecognizer.find(seat)!=HMIController::GetInstance()->singleTapRecognizer.end())
    {

        double tmpX = 0.0, tmpY = 0.0;
        This->singleTapRecognizer[seat]->GetLastPosition(tmpX, tmpY);

        logDebug(HMIC, "HMIController::touchCallback(): single tap recognizer's state before touch: ", This->singleTapRecognizer[seat]
            ->GetStateString());
        if(touchType == E_TOUCH_DOWN)
            This->singleTapRecognizer[seat]->TouchDown(x, y);
        else if(touchType == E_TOUCH_UP)
            This->singleTapRecognizer[seat]->TouchUp(tmpX, tmpY);
        else if(touchType == E_TOUCH_MOVE)
            This->singleTapRecognizer[seat]->TouchMove(x, y);
        else
            return;
        logDebug(HMIC, "HMIController::touchCallback(): single tap recognizer's state after touch: ", This->singleTapRecognizer[seat]
            ->GetStateString());
    }
}

void HMIController::doubleTapCallback(SurfaceID surfaceID)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::doubleTouchCallback(%#x)", surfaceID));

    E_SEAT seat = GET_SEAT(GET_AMPID(surfaceID));

    auto pmcTask = make_shared<PMCTask>();
    pmcTask->setPowerSignal(E_RSE_POWER_ON);
    pmcTask->SetType(HANDLE_LCD_SIGNAL);
    pmcTask->setSeatID(seat);
    pmcTask->setPowerSignal(E_RSE_POWER_ON);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);

    pmcTask = make_shared<PMCTask>();
    pmcTask->SetType(HANDLE_LCD_SIGNAL);
    pmcTask->setSeatID(seat);
    pmcTask->setPowerSignal(E_RSE_SCREEN_ON);
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);
}

void HMIController::singleTapCallback(SurfaceID surfaceID)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::singleTapCallback(%#x)", surfaceID));

    E_SEAT seat = GET_SEAT(GET_AMPID(surfaceID));
//    fpm deal it itself
//    auto fpmTask = make_shared<FPMTask>();
//    fpmTask->SetType(PWRMGR_SCREEN_ON);
//    fpmTask->setDetail(seat);
//    TaskDispatcher::GetInstance()->Dispatch(fpmTask);
}

void HMIController::drawPicture(E_SEAT seat, unsigned int *pixels, unsigned int width, unsigned int height, const ThreadSafeValueKeeper<PngMap> &pngMap)
{
    std::shared_ptr<PNG> png = nullptr;
    pngMap.OperateConst([&png, seat](const PngMap& tfpMap){
        if(tfpMap.find(seat)!=tfpMap.end())
            png = tfpMap.at(seat);
    });
    if(png && png->IsOK())
    {
        unsigned int pWidth  = 0;
        unsigned int pHeight = 0;
        png->GetDimension(pWidth, pHeight);
        for(int x = 0; x < pHeight && x < height; ++x)
        {
            for(int y = 0; y < pWidth && y < width; ++y)
            {
                pixels[x*width+y] = png->GetPixel(y, x).GetRawPixel(ARGB);
            }
        }
    }
    else
        Transparent(pixels, width, height);
}

void HMIController::drawTouchForbidderPictureIVI(unsigned int *pixels, unsigned int width, unsigned int height)
{
    drawPicture(E_SEAT_IVI, pixels, width, height, GetInstance()->touchForbidderPicture);
}

void HMIController::drawTouchForbidderPictureRSE1(unsigned int *pixels, unsigned int width, unsigned int height)
{
    drawPicture(E_SEAT_RSE1, pixels, width, height, GetInstance()->touchForbidderPicture);
}

void HMIController::drawTouchForbidderPictureRSE2(unsigned int *pixels, unsigned int width, unsigned int height)
{
    drawPicture(E_SEAT_RSE2, pixels, width, height, GetInstance()->touchForbidderPicture);
}

void HMIController::drawAPPictureIVI(unsigned int *pixels, unsigned int width, unsigned int height)
{
    drawPicture(E_SEAT_IVI, pixels, width, height, GetInstance()->animationPlayerPicture);
}

void HMIController::drawAPPictureRSE1(unsigned int *pixels, unsigned int width, unsigned int height)
{
    drawPicture(E_SEAT_RSE1, pixels, width, height, GetInstance()->animationPlayerPicture);
}

void HMIController::drawAPPictureRSE2(unsigned int *pixels, unsigned int width, unsigned int height)
{
    drawPicture(E_SEAT_RSE2, pixels, width, height, GetInstance()->animationPlayerPicture);
}

void HMIController::SystemLanguageChangedCB(const std::string& key, const SMValue& value, const SMValueObserver::E_EVENT event)
{
    if(key!=SYS_LANG)
        return;

    DomainVerboseLog chatter(HMIC, formatText("HMIController::SystemLanguageChangedCB(%s, %s, %d)", key.c_str(), value.value.c_str(), event));

    std::thread changeLoadingScreen([key, value, event](){
        DomainVerboseLog chatter(HMIC, formatText("changeLoadingScreenOnSystemLanguageChange(tid=%d)", gettid()));
        try
        {
            std::string threadID;
            std::stringstream sstream;
            sstream << std::this_thread::get_id();
            sstream >> threadID;
            logInfo(HMIC, "changeLoadingScreenOnSystemLanguageChange(): tid=", threadID);
            if(event==SMValueObserver::E_EVENT_VALUE_CHANGED || event==SMValueObserver::E_EVENT_KEY_CREATED)
            {
                std::string pngName = value.value=="C"?"LoadingScreen_E.png":"LoadingScreen_C.png";
                auto apPicture = std::make_shared<PNG>("/etc/ama.d/" + pngName);
                GetInstance()->animationPlayerPicture.Operate([apPicture](PngMap& map){
                    for(auto &pic: map)
                        pic.second = apPicture;
                });
                logInfo(HMIC, "HMIController::SystemLanguageChangedCB(): system's language is changed: current language is \"", value.value, "\"");
            }

            if(event==SMValueObserver::E_EVENT_KEY_DELETED)
            {
                std::string pngName = "LoadingScreen_C.png";
                auto apPicture = std::make_shared<PNG>("/etc/ama.d/" + pngName);
                GetInstance()->animationPlayerPicture.Operate([apPicture](PngMap& map){
                    for(auto &pic: map)
                        pic.second = apPicture;
                });
                logWarn(HMIC, "HMIController::SystemLanguageChangedCB(): system's language is deleted! use chinese loading screen.");
            }

            for(auto& ap: GetInstance()->animationPlayer)
                ForceRedrawSurface(SURFACEID_AP_AMGR(ap.first));
        }
        catch(std::exception& e)
        {
            logError(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
        }
        catch(...)
        {
            logError(HMIC, LOG_HEAD, "unexpected exception is caught!");
        }
    });

    std::string threadID;
    std::stringstream sstream;
    sstream << changeLoadingScreen.get_id();
    sstream >> threadID;
    logInfo(HMIC, LOG_HEAD, " task thead id: ", threadID);

    changeLoadingScreen.detach();
}

void HMIController::TouchDeviceCreatedCB(const std::string &deviceName, bool isCreate)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::TouchDeviceCreatedCB(%s, %s)", deviceName.c_str(), isCreate?"true":"false"));

    static std::map<std::string, E_SEAT> str2seat = {
        {"ivi", E_SEAT_IVI},
        {"rse_left", E_SEAT_RSE1},
        {"rse_right", E_SEAT_RSE2},
    };

    if(str2seat.find(deviceName)==str2seat.end())
    {
        logWarn(HMIC, "HMIController::TouchDeviceCreatedCB(): unknown seat id: ", deviceName);
        return;
    }

    // refresh touch device table
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetType(isCreate?HMIC_NOTIFY_TOUCHDEVICE_CREATED:HMIC_NOTIFY_TOUCHDEVICE_DESTROYED);
    hmicTask->SetAMPID(str2seat[deviceName]);

    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

void HMIController::SetExtraSurfaceOriginalSize(Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", surface.GetID()));

    try
    {
        SetInputDevice(surface);

        if(extraSurfaceMapping.find(surface)==extraSurfaceMapping.end())
        {
            logWarn(HMIC, LOG_HEAD, "invalid surface!");
            return;
        }

        auto layer = extraSurfaceMapping[surface];
        if(layer.GetID()==INVALID_ID)
        {
            logWarn(HMIC, LOG_HEAD, "invalid layer: ", ToHEX(layer.GetID()));
            return;
        }

        auto size = layer.GetDimension();

        if(size.width==0 || size.height==0)
        {
            logWarn(HMIC, LOG_HEAD, "invalid layer size! use default surface size.");
            size = Size{1920,720};
        }

        surface.SetConfiguration(size);
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in ", __STR_FUNCTIONP__);
    }
}

void HMIController::SetAMGRSurfaceOriginalSize(Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::SetAMGRSurfaceOriginalSize(%#x)", surface.GetID()));

    try
    {
        E_SEAT seat = GetSeat(surface);
        if(screens.find(seat)!=screens.end())
        {
            logDebug(HMIC, "HMIController::SetAMGRSurfaceOriginalSize(): specified surface's dependent screen is found.");
            Size size = screens[seat]->GetSize();
            surface.SetConfiguration(size);
        }
        else
            logWarn(HMIC, "HMIController::SetAMGRSurfaceOriginalSize(): specified surface's dependent screen is NOT found!");
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in HMIController::SetAMGRSurfaceOriginalSize()");
    }
}

void HMIController::SetSurfaceOriginalSize(Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::SetSurfaceOriginalSize(%#x)", surface.GetID()));

    // get app's AMPID
    AMPID proc = GetAMPID(surface);

    // AMGR surface
    if(IS_AMGR(proc))
    {
        SetAMGRSurfaceOriginalSize(surface);
    }
    // extra surface-layer mapping from app/systemd unit
    else if(extraSurfaceMapping.find(surface)!=extraSurfaceMapping.end())
    {
        SetExtraSurfaceOriginalSize(surface);
    }
    // app surface
    else
    {
        auto UIProcHMI = HMIController::GetInstance()->GetHMI(proc);

        // configure related surface
        if(UIProcHMI)
        {
            logDebug(HMIC, "HMIController::SetSurfaceOriginalSize(", ToHEX(surface.GetID()), "): app ", ToHEX(proc), " found.");

            if(screens.find(GetSeat(surface))!=screens.end())
            {
                auto screen = HMIController::GetInstance()->screens[GetSeat(surface)];
                UIProcHMI->OnSurfaceAvailable(surface, screen);
            }
            else
            {
                logWarn(HMIC, "HMIController::SetSurfaceOriginalSize(): screen ", GetSeat(surface), " is not available.");
                logWarn(HMIC, "HMIController::SetSurfaceOriginalSize(): HMIC is unable to configure surface ", ToHEX(surface.GetID()));
            }
        }
    }
}

void HMIController::ConfigureExtraSurface(Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", surface.GetID()));

    try
    {
        SetInputDevice(surface);

        if(extraSurfaceMapping.find(surface)==extraSurfaceMapping.end())
        {
            logWarn(HMIC, LOG_HEAD, "invalid surface!");
            return;
        }

        auto layer = extraSurfaceMapping[surface];
        if(layer.GetID()==INVALID_ID)
        {
            logWarn(HMIC, LOG_HEAD, "invalid layer: ", ToHEX(layer.GetID()));
            return;
        }

        auto section = layer.GetRectangle();

        if(section.width==0 || section.height==0)
        {
            logWarn(HMIC, LOG_HEAD, "invalid layer size! use default surface size.");
            section = Section{0,0,1920,720};
        }

        unsigned int x = 0;
        unsigned int y = 0;
        const auto dimension = surface.GetDimension();

        logInfo(HMIC, LOG_HEAD, "surface size is ", dimension.width, "*", dimension.height);
        if(dimension.width <= section.width && dimension.height <= section.height)
        {
            x = section.left + (section.width - dimension.width)/2;
            y = section.top + (section.height - dimension.height)/2;
        }
        else
            logWarn(HMIC, LOG_HEAD, "surface size is bigger than screen!");

        // get soruce rectangle & destination rectangle
        Section sourceRect = {0,0,dimension.width,dimension.height};
        Section destinationRect = {x,y,dimension.width,dimension.height};
        logDebug(HMIC, LOG_HEAD, "surface position: (", x, ",", y, ")");

        // set soruce rectangle & destination rectangle
        surface.SetSourceRectangle(sourceRect);
        surface.SetDestinationRectangle(destinationRect);

        layer.AddSurface(surface);

        surface.SetVisible(true);

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in ", __STR_FUNCTIONP__);
    }
}

void HMIController::ConfigureAMGRSurface(Surface& surface)
{
    DomainVerboseLog chatter(HMIC, formatText("HMIController::ConfigureAMGRSurface(%#x)", surface.GetID()));

    try
    {
        SetInputDevice(surface);

        if(GetLayer(surface)==E_LAYER_NONE)
        {
            Layer layer(LAYERID(GetSeat(surface), E_LAYER_NORMALAPP));
            if(screens.find(GetSeat(surface))!=screens.end())
                surface.SetBothRectangle(layer.GetRectangle());
        }
        else
        {
            Layer layer(surface.GetLayerID());

            surface.SetBothRectangle(layer.GetRectangle());

            layer.AddSurface(surface);

            // show background surface
            if(GetAppID(surface) == E_APPID_BKG)
                surface.SetVisible();

            // show RSE GR
            if(GetAppID(surface) == E_APPID_GR && (GetSeat(surface) == E_SEAT_RSE1 || GetSeat(surface) == E_SEAT_RSE2))
                surface.SetVisible(DCSConfigureTimeVisibility[GetSeat(surface)]);
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in HMIController::ConfigureAMGRSurface()");
    }
}

void HMIController::ConfigureSurface(Surface& surface)
{
    logVerbose(HMIC, "HMIController::ConfigureSurface(", ToHEX(surface.GetID()), ")--->IN");

    // get app's AMPID
    AMPID proc = GetAMPID(surface);

    // AVM/RVC
    if(GetAppID(surface)==E_APPID_AVM || GetAppID(surface)==E_APPID_RVC)
    {
        logDebug(HMIC, "HMIController::ConfigureSurface(): set input device for surface ", ToHEX(surface.GetID()));
        SetInputDevice(surface);
    }
    // AMGR surface
    else if(IS_AMGR(proc))
    {
        ConfigureAMGRSurface(surface);
    }
    // extra surface-layer mapping from app/systemd unit
    else if(extraSurfaceMapping.find(surface)!=extraSurfaceMapping.end())
    {
        ConfigureExtraSurface(surface);
    }
    // app surface
    else
    {
        auto UIProcHMI = HMIController::GetInstance()->GetHMI(proc);

        // configure related surface
        if(UIProcHMI)
        {
            logDebug(HMIC, "HMIController::ConfigureSurface(", ToHEX(surface.GetID()), "): app", ToHEX(proc), " found.");
            UIProcHMI->OnSurfaceCreate(surface);
            SetInputDevice(surface);
        }
    }

    logVerbose(HMIC, "HMIController::ConfigureSurface(", ToHEX(surface.GetID()), ")--->OUT");
}

void HMIController::DestroySurface(Surface& surface)
{
    logVerbose(HMIC, "HMIController::DestroySurface(", ToHEX(surface.GetID()), ")--->IN");

    // get app's object
    AMPID proc = GetAMPID(surface);
    auto ui = HMIController::GetInstance()->GetHMI(proc);

    if(ui)
        ui->OnSurfaceDestroy(surface);

    logVerbose(HMIC, "HMIController::DestroySurface(", ToHEX(surface.GetID()), ")--->OUT");
}

void HMIController::RegisterInputDeviceChangeCallback()
{
    DomainVerboseLog chatter(HMIC, "HMIController::RegisterInputDeviceChangeCallback()");

    try
    {
        InputPeripheral::AddInputPeripheralChangedCallback(std::bind(&HMIController::TouchDeviceCreatedCB, this, _1, _2));
    }
    catch(ILMException& e)
    {
        logError(HMIC, "HMIController::RegisterInputDeviceChangeCallback(): exception: ", (&e)->what());
    }
}

void HMIController::RegisterSurfaceChangeCallback()
{
    FUNCTION_SCOPE_LOG_C(HMIC);
    //register all surfaces property callback(it can notify surface's create and destroy)

    try
    {
        objExistanceCBHandle = RegisterObjExistanceNotification(callbackFunction);
    }
    catch (ILMException& e)
    {
        logError(HMIC, LOG_HEAD, "exception: ", (&e)->what());
    }
    catch(...)
    {
        logError(HMIC, "Unexpected exception caught in HMIController::RegisterSurfaceChangeCallback()!");
    }
}

void HMIController::RegisterGestureCallback()
{
    for(auto& recognizer: doubleTapRecognizer)
        recognizer.second->RegisterHandler(std::bind(&HMIController::doubleTapCallback, SURFACEID_GR(recognizer.first)));

    for(auto& recognizer: singleTapRecognizer)
        recognizer.second->RegisterHandler(std::bind(&HMIController::singleTapCallback, SURFACEID_GR(recognizer.first)));
}
