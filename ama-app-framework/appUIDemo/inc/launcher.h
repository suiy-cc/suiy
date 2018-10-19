/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef __LAUNCHER_H__
#define __LAUNCHER_H__

#include "common.h"
#include "hmi.h"

extern void PointerHandleOfButton(void *data, struct wl_pointer *wlPointer, uint32_t serial,
		    uint32_t time, uint32_t button, uint32_t state);

extern void createMainwindow(unsigned int seatIdx, int width, int height);
extern void releaseMainwindow(void);

extern WLContextStruct* launcherInit(void);
extern void launcherUninit(void);

#endif /* __LAUNCHER_H__ */
