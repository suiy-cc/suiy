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

#include "launcher.h"
#include <log.h>

static WLContextStruct *sContext = NULL;
struct wlContextStruct wlCtx_WorkSpaceBackGround;
struct wlContextStruct wlCtx_BackGround;
struct wlContextStruct wlCtx_ButtonMusic;
struct wlContextStruct wlCtx_ButtonVideo;

static int screenWidth = 0;
static int screenHeight = 0;

static unsigned int seatIdx = 0;

static void configure_ilm_surface(t_ilm_uint surface, struct ilmSurfaceProperties* prop)
{

    ilmErrorTypes err = ILM_SUCCESS;

    // if(DEMO_IS_HOME(surface)) {

	    ilm_surfaceSetSourceRectangle(surface, 0, 0, prop->origSourceWidth, prop->origSourceHeight);
	    LOGI("SetSourceRectangle     : surface ID (0x%08X), Width (%u), Height (%u)\n", surface, prop->origSourceWidth, prop->origSourceHeight);

	    if(surface == BASIC_SURFACE_ID) {
		    LOGI("SetDestinationRectangle : surface ID (0x%08X), X (%d), Y (%d), Width (%u), Height (%u)\n", surface, 0, 0, prop->origSourceWidth, prop->origSourceHeight);
		    ilm_surfaceSetDestinationRectangle(surface, 0, 0, prop->origSourceWidth, prop->origSourceHeight);
	    }
		else if(surface == BASIC_SURFACE_ID + 1) {
			LOGI("SetDestinationRectangle : surface ID (0x%08X), X (%d), Y (%d), Width (%u), Height (%u)\n", surface, 50, 50, prop->origSourceWidth, prop->origSourceHeight);
			//printSurfaceProperties(surface, "");
			ilm_surfaceSetDestinationRectangle(surface, 50, 50, prop->origSourceWidth, prop->origSourceHeight);
		}
		else if(surface == BASIC_SURFACE_ID + 2) {
			LOGI("SetDestinationRectangle : surface ID (0x%08X), X (%d), Y (%d), Width (%u), Height (%u)\n", surface, 50 + prop->origSourceWidth + 50, 50, prop->origSourceWidth, prop->origSourceHeight);
			//printSurfaceProperties(surface, "");
			ilm_surfaceSetDestinationRectangle(surface, 50 + prop->origSourceWidth + 50, 50, prop->origSourceWidth, prop->origSourceHeight);
		}

	    ilm_surfaceSetVisibility(surface, ILM_TRUE);
	    LOGI("SetVisibility          : surface ID (0x%08X), ILM_TRUE\n", surface);

        err = ilm_layerAddSurface(layerHome[seatIdx], surface);
        LOGI("layerAddSurface        : surface ID (0x%08X) is added to layer ID (0x%08X)\n", surface, layerHome[seatIdx]);
	    if(err != ILM_SUCCESS) {
	        LOGE("layerAddSurface : %s\n", ILM_ERROR_STRING(err));
	    }

	    ilm_commitChanges();
    // }
}

static void surfaceNotification(t_ilm_surface surface,
                                        struct ilmSurfaceProperties* prop,
                                        t_ilm_notification_mask mask)
{
    // #define ILM_NOTIFICATION_STRING(x)                                         \
    //     ( (x) == ILM_NOTIFICATION_VISIBILITY        ? "visibility"             \
    //     : (x) == ILM_NOTIFICATION_OPACITY           ? "opacity"                \
    //     : (x) == ILM_NOTIFICATION_ORIENTATION       ? "orientation"            \
    //     : (x) == ILM_NOTIFICATION_SOURCE_RECT       ? "source rectangle"       \
    //     : (x) == ILM_NOTIFICATION_DEST_RECT         ? "destination rectangle"  \
    //     : (x) == ILM_NOTIFICATION_CONTENT_AVAILABLE ? "content available"      \
    //     : (x) == ILM_NOTIFICATION_CONTENT_REMOVED   ? "content removed"        \
    //     : (x) == ILM_NOTIFICATION_CONFIGURED        ? "configured"             \
    //     : "unknown notification code"                                          )
    //
	// LOGI("surfaceNotification(0x%08X, %s)\n", surface, ILM_NOTIFICATION_STRING(mask));
    //
	// if ((unsigned)mask & ILM_NOTIFICATION_CONFIGURED) {
	// 	configure_ilm_surface(surface, prop);
	// }
    //
	// LOGI("surfaceNotification(0x%08X, %s) OUT\n", surface, ILM_NOTIFICATION_STRING(mask));
}

static void callbackFunction(ilmObjectType object, t_ilm_uint id, t_ilm_bool created, void *user_data)
{
    (void)user_data;
    struct ilmSurfaceProperties sp;

    LOGI("callbackFunction(%s ID : 0x%08X : %s)\n", (object == ILM_SURFACE)?"Surface":"Layer", id, created?"Created":"Removed");

    // if (object == ILM_SURFACE) {
    //     if (created) {
    //         LOGI("surface                : 0x%08X created\n",id);
    //         ilm_getPropertiesOfSurface(id, &sp);
    //
    //         if ((sp.origSourceWidth != 0) && (sp.origSourceHeight !=0)) {
    //             // surface is already configured
    //             LOGI("surface ID (0x%08X) is already configured\n", id);
    //             configure_ilm_surface(id, &sp);
    //         }
    //         else {
    //             // wait for configured event
    //             ilm_surfaceAddNotification(id, &surfaceNotification);
    //             // ilm_commitChanges();
    //         }
    //     }
    //     else if(!created) {
    //         LOGI("surface: 0x%08X destroyed\n",id);
    //     }
    // }
}

static void buttonPress(unsigned int surfaceID)
{
	LOGF("0x%08X\n", surfaceID);

	switch(surfaceID){
		case BASIC_SURFACE_ID + 1: {

			LOGI("LAUNCHING MUSIC\n");

		    const unsigned int ids[DEMO_SCREEN_MAX] = {
		        DEMO_SURFACE_ID_MUSIC_F, DEMO_SURFACE_ID_MUSIC_L, DEMO_SURFACE_ID_MUSIC_R
		    };

		    const unsigned int layers[DEMO_SCREEN_MAX] = {
		        DEMO_LAYER_ID_HMI_F, DEMO_LAYER_ID_HMI_L, DEMO_LAYER_ID_HMI_R
		    };

		    do_start_execv("/usr/share/ilm/testILM/testILM-music", ids[seatIdx], layers[seatIdx], screenWidth, screenHeight);
		}
		break;
		case BASIC_SURFACE_ID + 2: {

			LOGI("LAUNCHING VIDEO\n");

		    const unsigned int ids[DEMO_SCREEN_MAX] = {
		        DEMO_SURFACE_ID_VIDEO_F, DEMO_SURFACE_ID_VIDEO_L, DEMO_SURFACE_ID_VIDEO_R
		    };

		    const unsigned int layers[DEMO_SCREEN_MAX] = {
		        DEMO_LAYER_ID_HMI_F, DEMO_LAYER_ID_HMI_L, DEMO_LAYER_ID_HMI_R
		    };

		    do_start_execv("/usr/share/ilm/testILM/testILM-video", ids[seatIdx], layers[seatIdx], screenWidth, screenHeight);
		}
		break;
	}

}

static void buttonRelease(unsigned int surfaceID)
{
	LOGF("0x%08X\n", surfaceID);
}

void createMainwindow(unsigned int idx, int width, int height)
{
	LOGF("\n");

	screenWidth = width;
	screenHeight = height;

	seatIdx = idx;

	sContext = launcherInit();

	ilm_registerNotification(callbackFunction, NULL);

	memset(&wlCtx_WorkSpaceBackGround, 0, sizeof(wlCtx_WorkSpaceBackGround));
	memset(&wlCtx_BackGround, 0, sizeof(wlCtx_BackGround));
	memset(&wlCtx_ButtonMusic, 0, sizeof(wlCtx_ButtonMusic));
	memset(&wlCtx_ButtonVideo, 0, sizeof(wlCtx_ButtonVideo));

	// registerCursor(buttonPress, buttonRelease);
	// registerTouch(buttonPress, buttonRelease);

	wlCtx_BackGround.cmm  = sContext;
    switch(GET_APPID(GET_AMPID(gSurfaceID)))
    {
        case E_APPID_HOME:      create_background(&wlCtx_BackGround, gSurfaceID, HOME_BG_IMAGE, surfaceNotification);break;
        case E_APPID_MUSIC:     create_background(&wlCtx_BackGround, gSurfaceID, MEDIA_BG_IMAGE, surfaceNotification);break;
        case E_APPID_RADIO:     create_background(&wlCtx_BackGround, gSurfaceID, RADIO_BG_IMAGE, surfaceNotification);break;
        case E_APPID_SETTING:   create_background(&wlCtx_BackGround, gSurfaceID, SETTING_BG_IMAGE, surfaceNotification);break;
        case E_APPID_VR:        create_background(&wlCtx_BackGround, gSurfaceID, VR_BG_IMAGE, surfaceNotification);break;
        case E_APPID_BT:  		create_background(&wlCtx_BackGround, gSurfaceID, BT_BG_IMAGE, surfaceNotification);break;
        case E_APPID_POPUP:    	create_background(&wlCtx_BackGround, gSurfaceID, NOTIFY_BG_IMAGE, surfaceNotification);break;
        case E_APPID_TBOX: 		create_background(&wlCtx_BackGround, gSurfaceID, TBOX_BG_IMAGE, surfaceNotification);break;
        default: logError("unrecognized AMPID: ", GET_AMPID(gSurfaceID));break;
    }
	//create_background(&wlCtx_BackGround, gSurfaceID, LAUNCHER_IMAGE, surfaceNotification);
    // create_background(&wlCtx_BackGround, gSurfaceID, LAUNCHER_IMAGE, surfaceNotification);

	// wlCtx_WorkSpaceBackGround.cmm = sContext;
    //
	// wlCtx_ButtonMusic.cmm  = sContext;
	// create_button(&wlCtx_ButtonMusic, gSurfaceID + 1, MUSIC_IMAGE, -1, surfaceNotification);
	wlCtx_ButtonVideo.cmm  = sContext;
	create_button(&wlCtx_ButtonVideo, gSurfaceID + 3, TBOX_BG_IMAGE, -1, surfaceNotification);
    ilm_surfaceRemove(gSurfaceID + 3);
}

void releaseMainwindow(void)
{
	LOGF("\n");

	destroy_cursors(sContext);

	ilm_surfaceRemove(gSurfaceID);
	// ilm_surfaceRemove(gSurfaceID + 1);
	// ilm_surfaceRemove(gSurfaceID + 2);

	ilm_unregisterNotification();

	launcherUninit();
}
