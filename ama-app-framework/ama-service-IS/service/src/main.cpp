/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppSDK.h"
#include "log.h"

#include "DomainInvoker.h"
#include "TaskDispatcher.h"
#include "ISService.h"
#include "InformingSoundStubImpl.h"

#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <stdio.h>
#include <chrono>

LOG_DECLARE_CONTEXT(IS_SER);
LOG_DECLARE_CONTEXT(IS);
LOG_DECLARE_CONTEXT(CAPI);

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
    logDebugF(IS_SER,"file:[%s],line:[%d],size:[%d]",file,line,size);
}

int main(int argc, char **argv)
{
    //set priority for this thread
    nice(-10);

    //send watchdog ready signal
    sd_notify(0, "READY=1");

    AppSDKInit("ISS","InformingSound service");

    DomainInvoker domainInvoker1([&](){RegisterContext(IS_SER,"ISS-");}, [&](){UnregisterContext(IS_SER);});
    DomainInvoker domainInvoker2([&](){RegisterContext(IS,"IS");}, [&](){UnregisterContext(IS);});
    DomainInvoker domainInvoker3([&](){RegisterContext(CAPI,"CAPI");}, [&](){UnregisterContext(CAPI);});

    logInfo(IS_SER,"main()-->IN");

    // start IS module
    logDebug(IS_SER, "IS start");
    auto is = std::make_shared<ISService>();
    is->Start();
    // connect task dispatcher to IS
    TaskDispatcher::GetInstance()->RegisterTaskHandler(is);
    logDebug(IS_SER, "IS end");

    commonAPIServerInit();

    //init watchdog
    int watchdogFeedCycle = getWatchdogTimeCycle();

    while (true) {
        sleep(watchdogFeedCycle);
        // feed dog
        sd_notify(0, "WATCHDOG=1");

        if(access("/tmp/show.ISService.memory", F_OK) == 0) {
            unlink("/tmp/show.ISService.memory");
            debug_malloc_summary_details(debug_malloc_detail, NULL);
            size_t _debug_memory_s = debug_malloc_summary();
            logDebugF(IS_SER, "memory summary: %d", _debug_memory_s);
        }
    }

    // disconnect IS from task dispatcher
    TaskDispatcher::GetInstance()->UnregisterTaskHandler(is);

    // shut down IS module
    is->End();
    is = nullptr;

    return 0;
}
