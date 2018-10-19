/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "function.h"

#include "log.h"
#include "AMGRLogContext.h"
#include "ama_enum.h"
#include "COMMON/TimeStamp.h"
#include "Configuration.h"

#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <if.h>
#include <camera-0/camera_client.h>

#include "wd.h"
#include "cs_ctrl.h"
#include "UnitManager.h"
#include "AudioService.h"
#include "Capi.h"
#include "LastUserCtx.h"
#include "time_sync_util.h"
#include "AMClient.h"
#include "StartService.h"
#include "Configuration.h"
#include "DomainInvoker.h"
#include "FileSystem.h"
#include "AppAwakeMgr.h"

static const std::string signalRecordPath = "/tmp/ama-app-manager-signal-record.txt";
std::string complieDate = __DATE__;
std::string complieTime = __TIME__;
static std::string sCompileTime = complieDate + complieTime;
static bool sIsFirstRuning = false;

/*****************************************local function start*********************************************************/
static void RvcStateCallback(long unsigned int playstatus, ECameraStatus errcode)
{
    logInfo(RVC, "RvcStateCallback playstatus=", playstatus," errcode=",  errcode);

    if(CAM_SERVICE_NOT_STARTED==errcode || CAM_SERVER_DISCONNECTED==errcode)
    {
        logWarn(RVC, "camera service doesn't exist. retry in 200ms.");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        camera_client_reinit();
    }
}

void handleRVCSignal(ERverseSignalStatus signalStatus){
    logInfo(RVC, "handleRVCSignal()-->IN signalStatus:" ,signalStatus);

    if(REVERSE_SIGNAL_HIGH == signalStatus){
        AppAwakeMgr::GetInstance()->handlePowerAwakeRequest(AMPID_APP(E_SEAT_IVI,E_APPID_AVM),true);
    }else{//REVERSE_SIGNAL_LOW == rvcSignalSta
        AppAwakeMgr::GetInstance()->handlePowerAwakeRequest(AMPID_APP(E_SEAT_IVI,E_APPID_AVM),false);
    }

    //notify RVC State to APP
    BroadcastAMGRSetNotify(E_RVC_REVERSE_SATATE,to_string(signalStatus));

    logDebug(RVC, "handleRVCSignal()-->OUT");
}

static void RvcReverseCallback(ERverseSignalStatus signalStatus)
{
    DomainVerboseLog chatter(RVC, formatText("RvcReverseCallback(%d)" ,signalStatus));

    handleRVCSignal(signalStatus);
}

void checkCameraSignalStaAfterRegisterCB(void)
{
    ERverseSignalStatus rvcSignalSta;
    rvcSignalSta = camera_client_get_reversesignal();

    handleRVCSignal(rvcSignalSta);
}

static int AMGR_CopyFile(const char *sourceFileNameWithPath,
                         const char *targetFileNameWithPath)
{
    FILE *fpR, *fpW;
    int BUFFER_SIZE = (1024*1024);
    char buffer[BUFFER_SIZE];
    int lenR, lenW;
    if ((fpR = fopen(sourceFileNameWithPath, "r")) == NULL)
    {
        logError("AMGR_CopyFile the source file can not be opened!");
        return E_UNKNOWN;
    }
    if ((fpW = fopen(targetFileNameWithPath, "w")) == NULL)
    {
        logError("AMGR_CopyFile the target file can not be opened!");
        fclose(fpR);
        return E_UNKNOWN;
    }

    memset(buffer, 0, BUFFER_SIZE);
    while ((lenR = fread(buffer, 1, BUFFER_SIZE, fpR)) > 0)
    {
        if ((lenW = fwrite(buffer, 1, lenR, fpW)) != lenR)
        {
            logError("AMGR_CopyFile Write to target file failed!");
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

void CopyDltConfigToSystem()
{
    if(access("/media/IVI-Log/dlt_logstorage.conf",F_OK)!=0){
        logDebug("/media/IVI-Log/dlt_logstorage.conf in not exist");
    }else{
        logDebug("AMGR_CopyFile /media/IVI-Log/dlt_logstorage.conf has already exist");
        if(0!=remove("/media/IVI-Log/dlt_logstorage.conf")){
            logWarn("remove old dlt_logstorage.conf failed!!! ");
            return;
        }
    }

    //copy config file to system
    int ret = AMGR_CopyFile("/etc/ama.d/dlt_logstorage.conf","/media/IVI-Log/dlt_logstorage.conf");
    if(ret != E_OK){
        logError("AMGR_CopyFile copy dlt_logstorage file failed");
        return;
    }else{
        logInfo("AMGR_CopyFile copy dlt_logstorage file successed");
    }
}

static void SignalHandler(int sig, siginfo_t* info, void*)
{
    DomainLog chatter(PROF, LOG_WARN, formatText("SignalHandler(%d)", sig));

    // dump signal info
    std::ofstream oFile(signalRecordPath.c_str(), std::ios::app);
    if(oFile.is_open())
    {
        oFile << "AMGR pid: " << getpid() << " timestamp: " << TimeStamp::GetString() << " s signal: " << sig << " sender pid: " << info->si_pid << std::endl;
    }
    oFile.close();
    logInfo(PROF, "SignalHandler(): signal = ", sig, " pid = ", info->si_pid);

    // handle signal
    switch(sig)
    {
        case SIGINT:
        case SIGTERM:
            logInfo(PROF, "SignalHandler(): SIGINT/SIGTERM is received.");
            isTerminateSignalReceived = true;
            break;
        case SIGKILL:
            logError(PROF, "SignalHandler(): SIGKILL is received.");
            isTerminateSignalReceived = true;
            break;
        case SIGQUIT:
            logError(PROF, "SignalHandler(): SIGQUIT is received.");
            isTerminateSignalReceived = true;
            break;
        case SIGSEGV:
        case SIGILL:
            logError(PROF, "SignalHandler(): signal ", sig, " is received, thers's nothing we can do but exit.");
            exit(sig);
            break;
        default:
            logError(PROF, "SignalHandler(): unknown signal ", sig, " is received.");
    }
}

/*****************************************local function end************************************************************/

/*****************************************extern function start*********************************************************/
bool isAppMangerFirstRuningInThisMachine()
{
    return sIsFirstRuning;
}


void setDltStorgeConfig()
{
    if(isAppMangerFirstRuningInThisMachine()){
        CopyDltConfigToSystem();
    }
}

void checkIsAppManagerFirstRuningInThisMachineAndRecordIt(void)
{
    if(0 == (access(DEF_RECORD_DIR_FOR_APPMANAGER_RUN_STA,F_OK))){
        try
        {
            std::ifstream ifs;
            ifs.open(DEF_RECORD_DIR_FOR_APPMANAGER_RUN_STA);
            if(ifs.is_open()){
                std::ostringstream temp;
                temp<<ifs.rdbuf();
                std::string getCompileTime=temp.str();
                ifs.close();

                if(getCompileTime==sCompileTime){
                    logDebug("/media/ivi-data/ama.d/appManagerComplieTimeRecord is right compile time");
                    sIsFirstRuning = false;
                }else{
                    sIsFirstRuning = true;
                }
            }else{
                sIsFirstRuning = true;
            }
        }
        catch(std::exception& e)
        {
            logError(LOG_HEAD, "exception is caught: ", (&e)->what());
        }
        catch(...)
        {
            logError(LOG_HEAD, "unexpected exception is caught!");
        }
    }else{
        logError("/media/ivi-data/ama.d/appManagerComplieTimeRecord is not exist");
        sIsFirstRuning = true;
    }

    if(sIsFirstRuning){
        try
        {
            logDebug("/media/ivi-data/ama.d/appManagerComplieTimeRecord is not right comple time");
            std::ofstream ofs;
            ofs.open(DEF_RECORD_DIR_FOR_APPMANAGER_RUN_STA);
            if(ofs.is_open()){
                ofs << sCompileTime;
                ofs.close();
            }else{
                logError("open /media/ivi-data/ama.d/appManagerComplieTimeRecord failed");
            }
        }
        catch(std::exception& e)
        {
            logError(LOG_HEAD, "exception is caught: ", (&e)->what());
        }
        catch(...)
        {
            logError(LOG_HEAD, "unexpected exception is caught!");
        }
    }
}

void setSingalHandler(void)
{
    struct sigaction signalAction;
    memset(&signalAction, '\0', sizeof(signalAction));
    signalAction.sa_sigaction = &SignalHandler;
    signalAction.sa_flags = SA_SIGINFO;
    for(int i = 1; i <= SIGUNUSED; ++i)
    {
        if(i==SIGSEGV)
            continue;
        sigaction(i, &signalAction, nullptr);
    }
}

void cameraSignalInit()
{
    unsigned int cameraTimeout = 2*1000; // in milliseconds
    if(!Inotify::GetInstance()->waitAndCheckIsFileCreated("/run/early-init/", "camera.ready", cameraTimeout))
    {
        logError(PROF, "camera.service exceeds timeout.");
    }
    camera_client_init();
    camera_client_register_callback(RvcStateCallback);
    camera_client_register_reversecb(RvcReverseCallback);
    checkCameraSignalStaAfterRegisterCB();
}

void cameraSignalDeinit()
{
    camera_client_deregister_reversecb(RvcReverseCallback);
    camera_client_deregister_callback(RvcStateCallback);
    camera_client_destroy();
}

/**
 * the out of memory handler
 */
void OutOfMemoryHandler()
{
    logError("No more memory - bye");
    throw std::runtime_error(std::string("SocketHandler::start_listenting ppoll returned with error."));
}
