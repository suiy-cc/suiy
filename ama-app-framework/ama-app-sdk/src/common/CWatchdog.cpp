/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
// #include <cassert>
// #include <cstdlib>
// #include <stdexcept>
//
// #include <systemd/sd-daemon.h>
//
// #include "CWatchdog.h"
// #include "CDltWrapper.h"
//
// CWatchdog::CWatchdog(CTimerHandler* CTimerHandler) :
//         TimerCallback(this, &CWatchdog::timerCallback), //
//         mpTimerHandler(CTimerHandler), //
//         mHandle(0) //
// {
//     assert(mpTimerHandler);
//
//
//     //first retrieve the timeout interval
//
//     int watchdogTimeout = 0;
//
//     char* wusec=getenv("WATCHDOG_USEC");
//     if (wusec)
//         watchdogTimeout=atol(wusec);
//
//     if (watchdogTimeout > 0)
//     {
//         timespec timeout;
//
//         //calculate the half cycle as the right interval to trigger the watchdog.
//         timeout.tv_sec =   (watchdogTimeout / 2) / 1000000;
//         timeout.tv_nsec = ((watchdogTimeout / 2) % 1000000) * 1000;
//         logInfo("CWatchdog::CWatchdog setting watchdog timeout:", watchdogTimeout, "us. Notification set to:",
//                 timeout.tv_sec, "sec and", timeout.tv_nsec, "ns");
//
//         //add the timer here
//         if (mpTimerHandler->addTimer(timeout, &TimerCallback, mHandle, NULL))
//         {
//             logError("CWatchdog::CWatchdog failed to add timer");
//             throw std::runtime_error("CWatchdog::CWatchdog failed to add timer");
//         }
//     }
//
//     else
//     {
//         logInfo("CWatchdog::CWatchdog watchdog timeout was ", watchdogTimeout, " museconds, no watchdog active");
//     }
// }
//
// CWatchdog::~CWatchdog()
// {
//     //remove the timer again.
//     mpTimerHandler->removeTimer(mHandle);
// }
//
// void CWatchdog::timerCallback(timerHandle_t handle, void* userData)
// {
//     (void) userData;
//     int error(sd_notify(0, "WATCHDOG=1"));
//     if (error < 0)
//     {
//         logError("CWatchdog::timerCallback could not reset watchdog, error ", error);
//         throw std::runtime_error("CWatchdog::timerCallback could not reset watchdog");
//     }
//
//     mpTimerHandler->restartTimer(handle);
// }
//
// void CWatchdog::startWatchdog()
// {
//     int error(sd_notify(0, "READY=1"));
//     if (error < 0)
//     {
//         logError("CWatchdog::startWatchdog could not start watchdog, error ", error);
//         throw std::runtime_error("CWatchdog::startWatchdog could not start watchdog");
//     }
//     logInfo("READY=1 was sent to systemd");
// }
