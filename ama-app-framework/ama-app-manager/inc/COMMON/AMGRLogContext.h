/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AMGRLOGCONTEXT_H
#define AMGRLOGCONTEXT_H

#include "log.h"

extern LOG_DECLARE_CONTEXT(CAPI);   // for common API
extern LOG_DECLARE_CONTEXT(FPM);
extern LOG_DECLARE_CONTEXT(PMC);
extern LOG_DECLARE_CONTEXT(LCDM);
extern LOG_DECLARE_CONTEXT(PLC);    // for program lifecycle controller
extern LOG_DECLARE_CONTEXT(HMIC);   // for HMI controller
extern LOG_DECLARE_CONTEXT(SM);     // for state manager
extern LOG_DECLARE_CONTEXT(CS);     // for common service
extern LOG_DECLARE_CONTEXT(AC);     // for audio controller
extern LOG_DECLARE_CONTEXT(RPCW);
extern LOG_DECLARE_CONTEXT(WD);     // for watchdog
extern LOG_DECLARE_CONTEXT(PROF);   // for profile
extern LOG_DECLARE_CONTEXT(DBG);    // for appClient debug command
extern LOG_DECLARE_CONTEXT(RVC);    // for camera client (RVC)
extern LOG_DECLARE_CONTEXT(STH);    // for synchronized task handler
extern LOG_DECLARE_CONTEXT(BPM);    // for background process manager task handler
extern LOG_DECLARE_CONTEXT(ADBG);   // for audio debug
extern LOG_DECLARE_CONTEXT(DB);     // for AMGR's database operation

#endif // AMGRLOGCONTEXT_H
