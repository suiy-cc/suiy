/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
// #ifndef CWATCHDOG_H_
// #define CWATCHDOG_H_
//
// #include "CTimerHandler.h"
//
// class CWatchdog
// {
// public:
//     CWatchdog(CTimerHandler* timerHandler);
//     virtual ~CWatchdog();
//     void startWatchdog(); //!< starts the watchdog by sending ready to systemD
//     void timerCallback(timerHandle_t handle, void * userData); //!< the watchdog timer callback
//     TTimerCallBack<CWatchdog> TimerCallback;
//
// private:
//     CTimerHandler* mpTimerHandler; //!< pointer to the sockethandler
//     timerHandle_t mHandle; //!< handle of the timer
// };
//
// #endif//CWATCHDOG_H_
