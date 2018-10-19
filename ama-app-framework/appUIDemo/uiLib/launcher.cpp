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

#include <pthread.h>

#include <sys/prctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "launcher.h"


static WLContextStruct m_wlContextStruct;

static pthread_t s_tid = 0;
static bool quit = false;

t_ilm_uint layerHome[3] = {DEMO_LAYER_ID_HOME_F, DEMO_LAYER_ID_HOME_L, DEMO_LAYER_ID_HOME_R};
t_ilm_uint layerApplication[3] = {DEMO_LAYER_ID_HMI_F, DEMO_LAYER_ID_HMI_L, DEMO_LAYER_ID_HMI_R};
t_ilm_uint layerBar[3] = {DEMO_LAYER_ID_NOTI_F, DEMO_LAYER_ID_NOTI_L, DEMO_LAYER_ID_NOTI_R};

LOG_DECLARE();

extern struct wl_pointer_listener pointer_listener;
extern struct wl_touch_listener touch_listener;

static void
shm_format(void *data, struct wl_shm *pWlShm, uint32_t format)
{
	WLContextStruct *pCtx = (WLContextStruct*)data;

	pCtx->formats |= (1 << format);
}

static struct wl_shm_listener shm_listenter = {
	shm_format
};


int32_t getIdOfWlSurface(struct t_wlContextStruct *pCtx, struct wl_surface *wlSurface)
{
	struct wlContextStruct *pWlCtxSt = NULL;

	if (NULL == pCtx || NULL == wlSurface )
		return 0;

	wl_list_for_each(pWlCtxSt, &pCtx->list_wlContextStruct, link) {
		if (pWlCtxSt->wlSurface == wlSurface)
			return pWlCtxSt->id_surface;
	}

	return -1;
}

/**
 * Handler of capabilities
 */
static void
seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
	struct t_wlContextStruct *p_wlCtx = (struct t_wlContextStruct*)data;
	struct wl_seat *wlSeat = p_wlCtx->wlSeat;
	struct wl_pointer *wlPointer = p_wlCtx->wlPointer;
	struct wl_touch *wlTouch = p_wlCtx->wlTouch;

	//if (cursorFuncOn) {
		if ((caps & WL_SEAT_CAPABILITY_POINTER) && !wlPointer){
			wlPointer = wl_seat_get_pointer(wlSeat);
			wl_pointer_add_listener(wlPointer,
						&pointer_listener, data);
		}
		else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && wlPointer){
			wl_pointer_destroy(wlPointer);
			wlPointer = NULL;
		}
		p_wlCtx->wlPointer = wlPointer;
	//}

	if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !wlTouch){
		wlTouch = wl_seat_get_touch(wlSeat);
		wl_touch_add_listener(wlTouch, &touch_listener, data);
	} else
	if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && wlTouch){
		wl_touch_destroy(wlTouch);
		wlTouch = NULL;
	}
	p_wlCtx->wlTouch = wlTouch;
}

static struct wl_seat_listener seat_Listener = {
	seat_handle_capabilities,
};


void registry_handle_global(void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version)
{
//	LOGF("interface=%s\n", interface);

    WLContextStruct* p_wlCtx = (WLContextStruct*)data;
    int ans_strcmp = 0;
    (void)version;

    do
    {
        ans_strcmp = strcmp(interface, "wl_compositor");
        if (0 == ans_strcmp)
        {
            p_wlCtx->wlCompositor = (wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
            break;
        }

        if (!strcmp(interface, "wl_seat")) {
			p_wlCtx->wlSeat = (wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 1);
			wl_seat_add_listener(p_wlCtx->wlSeat, &seat_Listener, data);
		}

		if (!strcmp(interface, "wl_shm")) {
			p_wlCtx->wlShm = (wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
			wl_shm_add_listener(p_wlCtx->wlShm, &shm_listenter, p_wlCtx);
		}


    } while(0);
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    NULL
};

static void destroyWLContextCommon(WLContextStruct *p_wlCtx)
{
	if (p_wlCtx->pointer_surface)
		wl_surface_destroy(p_wlCtx->pointer_surface);

	if (p_wlCtx->wlCompositor)
		wl_compositor_destroy(p_wlCtx->wlCompositor);
}

static void destroyWLContextStruct(struct wlContextStruct *p_wlCtx)
{
	if (p_wlCtx->wlSurface)
		wl_surface_destroy(p_wlCtx->wlSurface);

	if (p_wlCtx->ctx_image) {
		cairo_surface_destroy(p_wlCtx->ctx_image);
		p_wlCtx->ctx_image = NULL;
	}
}

static void* launcherCore(void* arg)
{
	prctl (PR_SET_NAME, "launcherCore", 0, 0, 0, 0);

	memset(&m_wlContextStruct, 0, sizeof(m_wlContextStruct));

	wl_list_init(&m_wlContextStruct.list_wlContextStruct);

	ilm_init();

	m_wlContextStruct.wlDisplay = wl_display_connect(NULL);
	ilmErrorTypes err = ilmClient_init((t_ilm_nativedisplay)m_wlContextStruct.wlDisplay);
	if(err != ILM_SUCCESS) {
        LOGE("ilm_surfaceCreate : %s", ILM_ERROR_STRING(err));
    }

    m_wlContextStruct.wlRegistry = wl_display_get_registry(m_wlContextStruct.wlDisplay);
    wl_registry_add_listener(m_wlContextStruct.wlRegistry, &registry_listener, &m_wlContextStruct);
    wl_display_roundtrip(m_wlContextStruct.wlDisplay);
    wl_display_roundtrip(m_wlContextStruct.wlDisplay);

    create_cursors(&m_wlContextStruct);
	m_wlContextStruct.pointer_surface = wl_compositor_create_surface(m_wlContextStruct.wlCompositor);
	m_wlContextStruct.current_cursor = CURSOR_LEFT_PTR;

	*((bool*)arg) = true;

	int ret = 0;
	while (ret != -1 && !quit) {
		ret = wl_display_dispatch(m_wlContextStruct.wlDisplay);
	}

	struct wlContextStruct *pWlCtxSt = NULL;
	wl_list_for_each(pWlCtxSt, &m_wlContextStruct.list_wlContextStruct, link) {
		destroyWLContextStruct(pWlCtxSt);
	}

	destroyWLContextCommon(&m_wlContextStruct);

	destroy_cursors(&m_wlContextStruct);

	ilmClient_destroy();

	ilm_destroy();

	return(void *)0;

}

WLContextStruct* launcherInit(void)
{
	bool finished = false;
	quit = false;

	pthread_create(&(s_tid), NULL, launcherCore, &finished);

	while(!finished) {sleep(1);}

	return &m_wlContextStruct;
}

void launcherUninit(void)
{
	quit = true;
    wl_display_disconnect(m_wlContextStruct.wlDisplay);
}
