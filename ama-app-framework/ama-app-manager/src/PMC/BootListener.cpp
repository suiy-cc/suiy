/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "BootListener.h"
#include "PMCTask.h"
#include "ama_enum.h"
#include "UIProcess.h"
#include "PMCTask.h"
#include "TaskDispatcher.h"
#include "ID.h"
#include "FileSystem.h"
#include "ProcessInfo.h"
#include "Process.h"
#include "COMMON/NotifyWaiter.h"

#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <if.h>
#include <ivc_types.h>

#include "FILESYS/FileSystem.h"

enum ivc_welcomeState_e{
    E_WELCOME_NOT = 0x00,
    E_WELCOME_START = 0x01,
    E_WELCOME_END = 0x02
};

enum ivc_openingState_e{
    E_OPENING_NOT = 0x00,
    E_OPENING_START = 0x01,
    E_OPENING_END = 0x02
};

const std::string BootListener::mOpeningModeDevFile = "/dev/splash-logo-image";
const std::string BootListener::mWelcomeDeviceFile = "/dev/splash-logo";
const std::string BootListener::mFirstAppReadyFile = "/run/firstAppReady";
static bool sIsWelcomePlaying = false;
static bool sIsOpeningPlaying = false;
static bool sIsListening = false;
const unsigned long long mywaiterid = 22010419870414;

static void welcomeStateChanged(void *data,void* userdata){
    logVerbose(PMC,__FUNCTION__,"-->>IN welcomeSta:",*(int*)data);
    int welcomeSta=*(int*)data;
    switch(welcomeSta){
    case E_WELCOME_NOT:
        sIsWelcomePlaying = false;
        break;
    case E_WELCOME_START:
        sIsWelcomePlaying = true;
        if(sIsListening){
            auto pmcTask = make_shared<PMCTask>();
            pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_START);
            pmcTask->SetType(HANDLE_BOOT_SIGNAL);
            TaskDispatcher::GetInstance()->Dispatch(pmcTask);
        }
        break;
    case E_WELCOME_END:
        sIsWelcomePlaying = false;
        if(sIsListening){
            auto pmcTask = make_shared<PMCTask>();
            pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_OVER);
            pmcTask->SetType(HANDLE_BOOT_SIGNAL);
            TaskDispatcher::GetInstance()->Dispatch(pmcTask);
            BootListener::GetInstance()->setWelcomeOver(true);
        }
        break;
    default:
        logError(PMC,"welcomeSta is not defined!!!");
        break;
    }

    Notify(mywaiterid);
}


static void openingStateChanged(void *data,void* userdata){
    logVerbose(PMC,__FUNCTION__,"-->>IN openingSta:",*(int*)data);
    int openingSta=*(int*)data;
    switch(openingSta){
    case E_OPENING_NOT:
        sIsOpeningPlaying = false;
        break;
    case E_OPENING_START:
        sIsOpeningPlaying = true;
        // if(sIsListening){
        //     auto pmcTask = make_shared<PMCTask>();
        //     pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_START);
        //     pmcTask->SetType(HANDLE_BOOT_SIGNAL);
        //     TaskDispatcher::GetInstance()->Dispatch(pmcTask);
        // }
        break;
    case E_OPENING_END:
        sIsOpeningPlaying = false;
        // if(sIsListening){
        //     auto pmcTask = make_shared<PMCTask>();
        //     pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_OVER);
        //     pmcTask->SetType(HANDLE_BOOT_SIGNAL);
        //     TaskDispatcher::GetInstance()->Dispatch(pmcTask);
        //     BootListener::GetInstance()->setWelcomeOver(true);
        // }
        break;
    default:
        logError(PMC,"openingSta is not defined!!!");
        break;
    }

    Notify(mywaiterid);
}

static void ivcStartOk(void *data,void* userData)
{
    logDebug(PMC,"ivcStartOk()-->>");
    
    // if(BootListener::GetInstance()->mIsFirstAppReady)
    // {
    //     notice_ivi_ready(IVC_READY_ON);
    // }
#ifdef CODE_FOR_GAIA
    request_opening_mode();
    request_welcome_mode();
#endif
}

BootListener::BootListener()
:mIsBootOver(false)
,mIsWelcomeFileExist(false)
,mHomeOnShowHandle(0)
,mIsWelcomeOver(false)
,mIsFirstAppReady(false)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA
    // logDebug(PMC,"is CODE_FOR_GAIA");
    // char* name = "IVI_APP_MANAGER";
    // cluster_connection_init();
    // cluster_connection_start(name);
    // ivc_signal_connect("welcome_mode",welcomeStateChanged,NULL);
    // ivc_signal_connect("start_ok",ivcStartOk,NULL);
    // RegisterWaiterID(mywaiterid);
    // Wait(mywaiterid, 300);
    // UnregisterWaiterID(mywaiterid);
#endif
    logVerbose(PMC,"BootListener::BootListener()-->OUT");
}

BootListener::~BootListener()
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA  
    notice_pm_ready_status(IVC_POWER_MANAGER_NOT_READY);
#endif
    logVerbose(PMC,"BootListener::~BootListener()-->OUT");
}



void BootListener::IVCInit()
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA
    logDebug(PMC,"is CODE_FOR_GAIA");
    char* name = "IVI_APP_MANAGER";
    cluster_connection_init();
    cluster_connection_start(name);
    ivc_signal_connect("welcome_mode",welcomeStateChanged,NULL);
    ivc_signal_connect("opening_mode",openingStateChanged,NULL);
    ivc_signal_connect("start_ok",ivcStartOk,NULL);
    RegisterWaiterID(mywaiterid);
    Wait(mywaiterid, 500);
    UnregisterWaiterID(mywaiterid);
#endif
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>OUT");
}

int BootListener::setLCDBrightness(int screen_type , int brightness)
{
#ifdef CODE_FOR_GAIA
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN  screen_type == ",screen_type,"  brightness == ",brightness);
    notice_ivi_brightness(screen_type -1, brightness);
#endif
}

int BootListener::setLCDPower(int screen_type , int bl_power)
{
#ifdef CODE_FOR_GAIA
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN  screen_type == ",screen_type,"  bl_power == ",bl_power);
   // _IVCBl_PowerStatus e_powerstatus = (bl_power == 1 ? IVC_BLPOWER_ON : IVC_BLPOWER_OFF);
    if(screen_type -1 == IVC_IVI_SCREEN_IVI)
    {
        notice_ivi_blpower(IVI_POWERMANAGER, bl_power);
    }else
    {
        notice_ivi_rse_blpower(screen_type - 1, bl_power);
    }
#endif
}

bool BootListener::checkIsWelcomePlaying()
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA
    return sIsWelcomePlaying;
#else   //CODE_FOR_AIO
    if(0 == (access(mWelcomeDeviceFile.c_str(),F_OK))){
        mIsWelcomeFileExist = true;
        std::ifstream ifs;
        ifs.open(mWelcomeDeviceFile.c_str());
        if(ifs.is_open()){
            std::ostringstream temp;
            temp<<ifs.rdbuf();
            std::string res=temp.str();
            ifs.close();

            if(std::string("1")==res.substr(0,1)){
                logDebug(PMC,"welcome is playing");
                return true;
            }else{
                return false;
            }
        }
    }else{
        logError(PMC,"mWelcomeDeviceFile is not exist");
        return false;
    }
#endif
}

bool BootListener::checkIsOpeningPlaying(void)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA
    return sIsOpeningPlaying;
#else   
    if(0 == (access(mOpeningModeDevFile.c_str(),F_OK))){
        std::ifstream ifs;
        ifs.open(mOpeningModeDevFile.c_str());
        if(ifs.is_open()){
            std::ostringstream temp;
            temp<<ifs.rdbuf();
            std::string res=temp.str();
            ifs.close();

            if(std::string("1")==res.substr(0,1)){
                logDebug(PMC,"Opening is playing");
                return true;
            }else{
                return false;
            }
        }
    }else{
        logError(PMC,"mOpeningModeDevFile is not exist");
        return false;
    }
#endif
}

void BootListener::startListenBootSta(void)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
    mHomeOnShowHandle=UIProcess::beforePreShowBroadcaster.Register(cb_AppOnShow);
#ifdef CODE_FOR_GAIA
    sIsListening = true;
    request_opening_mode();
    request_welcome_mode();
#else
    mListeningThread = std::thread(std::bind(&BootListener::listeningRun, this));
#endif
    if(checkIsFirstAppAlreadyOk()){
        setFirstAppReady();
        auto pmcTask = make_shared<PMCTask>();
        pmcTask->setPowerSignal(E_BOOT_FIRST_APP_READY);
        pmcTask->SetType(HANDLE_BOOT_SIGNAL);
        TaskDispatcher::GetInstance()->Dispatch(pmcTask);
    }
    logVerbose(PMC,"BootListener::startListenBootSta()-->OUT");
}

void BootListener::listeningRun()
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
    try
    {
        sIsListening = true;
        while(sIsListening){
            if(!mIsWelcomeFileExist){
                if(Inotify::GetInstance()->waitAndCheckIsFileCreated("/dev/","splash-logo",200)){
                    mIsWelcomeFileExist = true;
                    logInfo(PMC,"welcome play start!");
                    //send a boot signal to PMC
                    auto pmcTask = make_shared<PMCTask>();
                    pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_START);
                    pmcTask->SetType(HANDLE_BOOT_SIGNAL);
                    TaskDispatcher::GetInstance()->Dispatch(pmcTask);
                }
            }else{//mIsWelcomeFileExist
                if(!checkIsWelcomePlaying()){
                    logInfo(PMC,"welcome play over!");
                    //send a boot signal to PMC
                    auto pmcTask = make_shared<PMCTask>();
                    pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_OVER);
                    pmcTask->SetType(HANDLE_BOOT_SIGNAL);
                    TaskDispatcher::GetInstance()->Dispatch(pmcTask);
                    mIsWelcomeOver = true;
                    sIsListening = false;
                }

                if(Inotify::GetInstance()->waitAndCheckIsFileWrited(mWelcomeDeviceFile,200)){
                    logInfo(PMC,mWelcomeDeviceFile," was writed");
                    if(!checkIsWelcomePlaying()){
                        logInfo(PMC,"welcome play over!");
                        //send a boot signal to PMC
                        auto pmcTask = make_shared<PMCTask>();
                        pmcTask->setPowerSignal(E_BOOT_WELCOME_PLAY_OVER);
                        pmcTask->SetType(HANDLE_BOOT_SIGNAL);
                        TaskDispatcher::GetInstance()->Dispatch(pmcTask);
                        mIsWelcomeOver = true;
                        sIsListening = false;
                    }
                }
            }
        }
    }
    catch(std::exception& e)
    {
        logError(PMC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
    catch(...)
    {
        logError(PMC, LOG_HEAD, "unexpected exception is caught!");
    }

    logVerbose(PMC,"BootListener::listeningRun()-->OUT");
}

void BootListener::stopListen(void)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
    UIProcess::beforePreShowBroadcaster.Unregister(mHomeOnShowHandle);
    mIsBootOver=true;
    if(sIsListening){
        sIsListening = false;
#ifdef CODE_FOR_GAIA     
    notice_pm_ready_status(IVC_POWER_MANAGER_READY);        
#else
    mListeningThread.join();
#endif
    }
    
    logVerbose(PMC,"BootListener::stopListen()-->OUT");
}

void BootListener::stopOpeningMode(void)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA
    notice_exit_opening_mode();
#endif
    std::ofstream ofs;

    ofs.open(mOpeningModeDevFile.c_str());
    if(ofs.is_open())
    {
        ofs << std::string("0");
        ofs.close();
    }
    logVerbose(PMC,"BootListener::stopOpeningMode()-->OUT");
}

void BootListener::setFirstAppReady(void)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
    mIsFirstAppReady = true;
#ifdef CODE_FOR_GAIA
    notice_ivi_ready(IVC_READY_ON);  
    notice_exit_opening_mode();
#endif
    std::ofstream ofs;
    ofs.open(mFirstAppReadyFile.c_str());
    if(ofs.is_open()){
        ofs << "1";
        ofs.close();
    }else{
        logError(PMC,"open mPowerStateFileByRAM failed");
    }

    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>OUT");
}

void BootListener::reqWelcomeFlg()
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
#ifdef CODE_FOR_GAIA
    request_welcome_mode();
    RegisterWaiterID(mywaiterid);
    Wait(mywaiterid, 500);
    UnregisterWaiterID(mywaiterid);
#endif
}

bool BootListener::checkIsFirstAppAlreadyOk(void)
{
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>IN");
    logInfo(PMC,"check home as the first app");
    AMPID homeAmpid = AMPID_APP(E_SEAT_IVI,E_APPID_HOME);
    ProcessInfo::GetInstance()->Lock();
    auto process = ProcessInfo::GetInstance()->FindProc(homeAmpid);
    ProcessInfo::GetInstance()->Unlock();
    if(nullptr==process){
        return false;
    }else{
        return process->IsOperating();
    }
    logVerbose(PMC,"BootListener::",__FUNCTION__,"-->>OUT");
}

void BootListener::cb_AppOnShow(AMPID ampid)
{
    logVerbose(PMC,"BootListener::cb_homeOnShow()-->IN ampid:",ToHEX(ampid));
    logInfo(PMC,"check (home/radio/music/video) as the first app");
    AMPID homeAmpid = AMPID_APP(E_SEAT_IVI,E_APPID_HOME);
    AMPID radioAmpid = AMPID_APP(E_SEAT_IVI,E_APPID_RADIO);
    AMPID musicAmpid = AMPID_APP(E_SEAT_IVI,E_APPID_MUSIC);
    AMPID videoAmpid = AMPID_APP(E_SEAT_IVI,E_APPID_VIDEO);
    if(!BootListener::GetInstance()->isBootOver()){
        if((homeAmpid==ampid)||(radioAmpid==ampid)||(musicAmpid==ampid)||(videoAmpid==ampid)){
            logInfo(PMC,"first app(home/radio/music/video) is ready!");
            BootListener::GetInstance()->setFirstAppReady();
            //send a boot signal to PMC
            auto pmcTask = make_shared<PMCTask>();
            pmcTask->setPowerSignal(E_BOOT_FIRST_APP_READY);
            pmcTask->SetType(HANDLE_BOOT_SIGNAL);
            TaskDispatcher::GetInstance()->Dispatch(pmcTask);
        }
    }
    logVerbose(PMC,"BootListener::cb_homeOnShow()-->OUT");
    // LCDManager::GetInstance()->SetIsLCDEnabled(true);

}
