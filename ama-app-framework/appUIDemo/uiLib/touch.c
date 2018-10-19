/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/mman.h>

#include "common.h"
#include "hmi.h"

static buttonPressFunc sHandlerDown;
static buttonReleaseFunc sHandlerUp;

/**
 * Even handler of touch event
 */
static void
TouchHandleDown(void *data, struct wl_touch *wlTouch, uint32_t serial,
		uint32_t time, struct wl_surface *surface, int32_t id,
		wl_fixed_t x_w, wl_fixed_t y_w)
{
	if(sHandlerDown != NULL) {
		WLContextStruct *pCtx = (WLContextStruct*)data;
		uint32_t id_surface = 0;

		if (0 == id)
			pCtx->enterSurface = surface;

		id_surface = getIdOfWlSurface(pCtx, pCtx->enterSurface);

		sHandlerDown(id_surface);
	}
}

static void
TouchHandleUp(void *data, struct wl_touch *wlTouch, uint32_t serial,
	      uint32_t time, int32_t id)
{
	if(sHandlerUp != NULL) {
		WLContextStruct *pCtx = (WLContextStruct*)data;

		const uint32_t id_surface = getIdOfWlSurface(pCtx, pCtx->enterSurface);

		/**
		 * triggering event according to touch-up happening on which surface.
		 */
		if (id == 0){
			sHandlerUp(id_surface);
		}
	}
}

static void
TouchHandleMotion(void *data, struct wl_touch *wlTouch, uint32_t time,
		  int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
}

static void
TouchHandleFrame(void *data, struct wl_touch *wlTouch)
{
}

static void
TouchHandleCancel(void *data, struct wl_touch *wlTouch)
{
}

struct wl_touch_listener touch_listener = {
	TouchHandleDown,
	TouchHandleUp,
	TouchHandleMotion,
	TouchHandleFrame,
	TouchHandleCancel,
};

void registerTouch(buttonPressFunc down, buttonReleaseFunc up)
{
	LOGF("\n");

	sHandlerDown = down;
	sHandlerUp = up;
}
