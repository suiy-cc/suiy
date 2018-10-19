/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppSDK.h"
#include "log.h"
#include "TaskHandlerManager.h"
#include "ProService.h"

#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <chrono>

#include "AppManagerProxy.h"
#include "MCUProxy.h"
#include "StateManagerProxy.h"
#include "AMGRSurfaces.h"
#include "ClusterProxy.h"
#include "ProCtrl.h"

LOG_DECLARE_CONTEXT(COS);
LOG_DECLARE_CONTEXT(AMRP);
LOG_DECLARE_CONTEXT(MCUP);
LOG_DECLARE_CONTEXT(CLTP);
LOG_DECLARE_CONTEXT(SMP);
LOG_DECLARE_CONTEXT(PRO);


#define WATCHDOGD_DEFAULT_FEED_CYCLE (8 * 1000000)

int getWatchdogTimeCycle()
{
    char* wusec = getenv("WATCHDOG_USEC");
    int watchdogTimeout = 0;
    if (wusec)
        watchdogTimeout = atol(wusec);
    else
        watchdogTimeout = WATCHDOGD_DEFAULT_FEED_CYCLE;
    //calculate the half cycle as the right interval to trigger the watchdog.
    timespec timeout;
    timeout.tv_sec =   (watchdogTimeout / 4) / 1000000;
    timeout.tv_nsec = ((watchdogTimeout / 4) % 1000000) * 1000;
    std::chrono::seconds delaySecs(timeout.tv_sec);

    return timeout.tv_sec;
}

static void debug_malloc_detail(
                const char* file, int line, size_t size, void* user_data)
{
    // make sure, no new/delete was called in this function/sub-function
    logDebugF(COS,"file:[%s],line:[%d],size:[%d]",file,line,size);
}

int main(int argc, char **argv)
{
    //make sure execute appSDKInit() first in AppManagerProxy::AppManagerProxy()
    AppManagerProxy::GetInstance();
    RegisterContext(COS,"COS", "common service's default log context");
    RegisterContext(PRO,"PRO", "project module log context");

    logDebug(COS,"main()-->IN");
    // Task-Handler manager
    logInfo(COS, "Task-Handler manager start");
    auto moduleManger = std::make_shared<TaskHandlerManager>();
    TaskHandlerManager::RegisterGlobalObjet(moduleManger);
    logInfo(COS, "Task-Handler manager start over");

    // start project module
    logInfo(COS, "project module start");
    moduleManger->StartHandler("AC", std::make_shared<ProjectService>());
    sendInitTaskToProjectModule();
    logInfo(COS, "project module start over");

    AppManagerProxy::GetInstance()->startListeningEvents();
    ClusterProxy::GetInstance()->tryToConnectWithCluster();
    MCUProxy::GetInstance()->startListeningEvents();
    StateManagerProxy::GetInstance()->init();

#ifdef CODE_FOR_GAIA
    // add navi surface to specified layer
    // ivi navi ---> cluster navi
    AddExtraSurface(0x400C0000, 0x50001000);
    AddExtraSurface(0x400C0100, 0x2000);
#endif // CODE_FOR_GAIA

    //init watchdog
    int watchdogFeedCycle = getWatchdogTimeCycle();
    //send watchdog ready signal
    sd_notify(0, "READY=1");
    while (true) {
        sleep(watchdogFeedCycle);
        // feed dog
        sd_notify(0, "WATCHDOG=1");

        if(access("/tmp/show.memory", F_OK) == 0) {
            unlink("/tmp/show.memory");
            debug_malloc_summary_details(debug_malloc_detail, NULL);
            size_t _debug_memory_s = debug_malloc_summary();
            logDebugF(COS, "memory summary: %d", _debug_memory_s);
        }
    }

#ifdef CODE_FOR_GAIA
    // unset the surface-layer mapping
    AddExtraSurface(0x400C0100, 0xFFFFFFFF);
    AddExtraSurface(0x400C0000, 0xFFFFFFFF);
#endif // CODE_FOR_GAIA

    logInfo(COS, "release modules...");
    TaskHandlerManager::RegisterGlobalObjet(nullptr);
    moduleManger = nullptr;
    logInfo(COS, "release modules OK");

    UnregisterContext(COS);
    return 0;
}


/*********************************function List********************************
1)start welink app when welink is selected in syssetting app and welink ios device attached
2)start carlife app in startup
3)notify powerState to Cluster
4)show IG popup
5)show screen abnormal popup
6)set system unmute when power changeto on
7)set system unmute when media focus change from loss to gain
8)send active media type to MCU
9)start factory app receive linetools
**********************************function List********************************/
