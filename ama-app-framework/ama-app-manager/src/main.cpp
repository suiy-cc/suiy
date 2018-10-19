/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/** @mainpage ama-hmi-controller - None
 *
 * @author anonymous <anony@mo.us>
 * @version 1.0.0
**/


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
#include <if.h>

#include "ProcessInfo.h"
#include "AMGRLogContext.h"
#include "cs_task.h"
#include "cs_ctrl.h"
#include "HMICTask.h"
#include "PMCService.h"
#include "BPMService.h"
#include "ServicePLC.h"
#include "ServiceHMIC.h"
#include "ServiceSM.h"
#include "service_cs.h"
#include "ServiceSTH.h"
#include "TaskDispatcher.h"
#include "TaskHandlerManager.h"

#include "wd.h"
#include "cs_ctrl.h"
#include "UnitManager.h"
#include "AudioService.h"
#include "Capi.h"
#include "LastUserCtx.h"
#include "time_sync_util.h"
#include "AMClient.h"
#include "Configuration.h"
#include "DomainInvoker.h"
#include "FileSystem.h"
#include "AppAwakeMgr.h"
#include "AMGRLogContext.h"
#include "function.h"
#include "Const.h"

#ifdef ENABLE_DEBUG_MODE
#include "AppProfileManager.h"
#endif // ENABLE_DEBUG_MODE



/**
 * Main class of project ama-hmi-controller
 *
 * @param argc the number of arguments
 * @param argv the arguments from the commandline
 * @returns exit code of the application
 */
int mainProgram(int argc, char **argv)
{
    // initialize log and log contexts
    LogInitialize("AMGR", "ama-app-manager, app-framwork's main service.", IsEnableDLTLocalPrint());
    DomainInvoker domainInvoker1([&](){RegisterContext(CAPI,"CAPI");}, [&](){UnregisterContext(CAPI);});
    DomainInvoker domainInvoker2([&](){RegisterContext(FPM,"FPM");}, [&](){UnregisterContext(FPM);});
    DomainInvoker domainInvoker3([&](){RegisterContext(PMC,"PMC");}, [&](){UnregisterContext(PMC);});
    DomainInvoker domainInvoker4([&](){RegisterContext(PLC,"PLC");}, [&](){UnregisterContext(PLC);});
    DomainInvoker domainInvoker5([&](){RegisterContext(HMIC,"HMIC");}, [&](){UnregisterContext(HMIC);});
    DomainInvoker domainInvoker6([&](){RegisterContext(SM,"SM");}, [&](){UnregisterContext(SM);});
    DomainInvoker domainInvoker7([&](){RegisterContext(CS,"CS");}, [&](){UnregisterContext(CS);});
    DomainInvoker domainInvoker8([&](){RegisterContext(AC,"AC");}, [&](){UnregisterContext(AC);});
    DomainInvoker domainInvoker9([&](){RegisterContext(RPCW,"RPCW");}, [&](){UnregisterContext(RPCW);});
    DomainInvoker domainInvoker11([&](){RegisterContext(PROF, "PROF");}, [&](){UnregisterContext(PROF);});
    DomainInvoker domainInvoker13([&](){RegisterContext(DBG,"DBG");}, [&](){UnregisterContext(DBG);});
    DomainInvoker domainInvoker14([&](){RegisterContext(RVC,"RVC");}, [&](){UnregisterContext(RVC);});
    DomainInvoker domainInvoker15([&](){RegisterContext(STH,"STH");}, [&](){UnregisterContext(STH);});
    DomainInvoker domainInvoker16([&](){RegisterContext(WD,"WD");}, [&](){UnregisterContext(WD);});
    DomainInvoker domainInvoker17([&](){RegisterContext(LCDM,"LCDM");}, [&](){UnregisterContext(LCDM);});
    DomainInvoker domainInvoker18([&](){RegisterContext(USBM,"USBM");}, [&](){UnregisterContext(USBM);});
    DomainInvoker domainInvoker19([&](){RegisterContext(BPM,"BPM");}, [&](){UnregisterContext(BPM);});
    DomainInvoker domainInvoker21([&](){RegisterContext(ADBG,"ADBG");}, [&](){UnregisterContext(ADBG);});
    DomainInvoker domainInvoker22([&](){RegisterContext(DB,"DB");}, [&](){UnregisterContext(DB);});

    // first local variable. important exit flag.
    // NEVER put other objects before it.
    DomainVerboseLog chatter(PROF, "main()");

    logInfo("Hello, Wrold!");
    #ifdef CODE_FOR_EV
    logInfo("I'm compile for EV target");
    logInfo("I'm compile time:",__DATE__," ",__TIME__);
    #else
    logInfo("I'm compile for HS7 target");
    logInfo("I'm compile time:",__DATE__," ",__TIME__);
    #endif

    //register new out of memory handler
    std::set_new_handler(&OutOfMemoryHandler);

    // generate random number seed
    srand(time(nullptr));

    // create writable folder in case it doesn't exist.
    mkdir(WRITABLE_FOLDER, 0777);
	mkdir(WRITABLE_APP_FOLDER, 0777);

    logInfo(PROF, "setDltStorgeConfig() start");
    checkIsAppManagerFirstRuningInThisMachineAndRecordIt();
    setDltStorgeConfig();
    logInfo(PROF, "setDltStorgeConfig() end");

    logInfo(PROF, "setSingalHandler() start");
    setSingalHandler();
    logInfo(PROF, "setSingalHandler() end");

    // Task-Handler manager
    logInfo(PROF, "Task-Handler manager start");
    auto moduleManger = std::make_shared<TaskHandlerManager>();
    TaskHandlerManager::RegisterGlobalObjet(moduleManger);
    logInfo(PROF, "Task-Handler manager end");

    // start SM module
    logInfo(PROF, "SM start");
    moduleManger->StartHandler("SM", std::make_shared<ServiceSM>());
    sendInitTaskToSM();
    logInfo(PROF, "SM end");

    // start HMIC module
    logInfo(PROF, "HMIC start");
    moduleManger->StartHandler("HMIC", std::make_shared<ServiceHMIC>());
    logInfo(PROF, "HMIC end");

    // start PLC module
    logInfo(PROF, "PLC start");
    moduleManger->StartHandler("PLC", std::make_shared<ServicePLC>());
    logInfo(PROF, "PLC end");

    // start AC module
    logInfo(PROF, "AC start");
    moduleManger->StartHandler("AC", std::make_shared<AudioService>());
    sendInitTaskToAC();
    logInfo(PROF, "AC end");

    // start FPM module
    logInfo(PROF, "PMC start");
    moduleManger->StartHandler("PMC", std::make_shared<PMCService>());
    //we must register powerManager before notify systemD
    //because powerManager will send ACC-ON signal before unfocussed.target
    PowerManagerController::GetInstance()->earlyInit();
    sendInitTaskToPMC();
    logInfo(PROF, "PMC end");

    logInfo(PROF, "appManager::notify systemD");
    wdNotifySystemdReady();

    // register CAPI server
    logInfo(PROF, "CAPI start");
    InitializeCAPI();
    logInfo(PROF, "CAPI end");

#ifdef ENABLE_DEBUG_MODE
    logInfo(PROF, "APM start");
    AppProfileManager::GetInstance();
    logInfo(PROF, "APM end");
#endif // ENABLE_DEBUG_MODE

    // start CS module
    logInfo(PROF, "CS start");
    moduleManger->StartHandler("CS", std::make_shared<ServiceCS>());
    logInfo(PROF, "CS end");

    // start STH module
    logInfo(PROF, "STH start");
    moduleManger->StartHandler("STH", std::make_shared<ServiceSTH>());
    logInfo(PROF, "STH end");

    logInfo(PROF, "BPM start");
    moduleManger->StartHandler("BPM", std::make_shared<BPMService>());
    logInfo(PROF, "BPM end");

    // watchdog setting
    logInfo(PROF, "watchdog start");
    wdInit();
    char* wusec = getenv("WATCHDOG_USEC");
    int watchdogCheckIntervalUSec = 0;
    if (wusec){
        logInfoF(PROF,"system define env:WATCHDOG_USEC=%s",wusec);
        int watchdogTimeout = atol(wusec);
        //calculate the half cycle as the right interval to trigger the watchdog.
        watchdogCheckIntervalUSec = (watchdogTimeout / 4);
    }else{
        logInfo(PROF,"there isn't env WATCHDOG_USEC,we use default 5 seconds");
        watchdogCheckIntervalUSec = 5000000;
    }
    logInfo(PROF,"watchdogCheckIntervalUSec:",watchdogCheckIntervalUSec);
    logInfo(PROF, "watchdog end");

    /*
     * old sdnotify place
    */
    logInfo(PROF, "start camera library...");
    cameraSignalInit();
    logInfo(PROF, "start camera library OK");

    logInfo(PROF, "Enter while loop ");
    while (!isTerminateSignalReceived)
    {
        // send watch-dog task to every task thread
        wdSendWatchdogTask();
        usleep(watchdogCheckIntervalUSec);
        // check return status
        wdCheckAllStates();
    }
    logInfo(PROF, "while loop is over.");

    // start quiting...
    logInfo(PROF, "ama-app-manager exiting...");

    logInfo(PROF, "stop camera library...");
    cameraSignalDeinit();
    logInfo(PROF, "stop camera library OK");

	//SMdeInit();

    logInfo(PROF, "release modules...");
    TaskHandlerManager::RegisterGlobalObjet(nullptr);
    moduleManger = nullptr;
    logInfo(PROF, "release modules OK");

    logInfo(PROF, "ama-app-manager exit OK");
    logInfo("Bye, Wrold!");
    return 0;
}

/**
 * main()
 *
 * @param argc the number of arguments
 * @param argv the arguments from the commandline
 * @returns exit code of the application
 */
int main(int argc, char **argv)
{
    int ret = 0;

    try{
        //we do this to catch all exceptions and have a graceful ending just in case
        ret = mainProgram(argc,argv);
    }catch (std::exception& exc){
        logError("The ama-app-manager ended by throwing the exception", (&exc)->what());
        std::cerr<<"The ama-app-manager ended by throwing an exception "<< (&exc)->what() <<std::endl;
        ret = (EXIT_FAILURE);
    }catch (...){
        logError("The ama-app-manager ended by throwing an unknow exception");
        std::cerr<<"The ama-app-manager ended by throwing an unknow exception "<<std::endl;
        ret = (EXIT_FAILURE);
    }

    return ret;
}
