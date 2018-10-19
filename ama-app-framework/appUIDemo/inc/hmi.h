/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef __HMI_H__
#define __HMI_H__

#include <linux/input.h>

#include <ilm_common.h>
#include <ilm_client.h>
#include <ilm_control.h>
#include <ilm_input.h>

#include <wayland-cursor.h>
#include <wayland-client.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include "cairo-util.h"
#include "os-compatibility.h"
#include "helpers.h"

#ifdef  __cplusplus
}
#endif


#define DEMO_SEAT_ID_IVI_F       ( 0x00000000U )
#define DEMO_SEAT_ID_RSE_L       ( 0x01000000U )
#define DEMO_SEAT_ID_RSE_R       ( 0x02000000U )

#define DEMO_LAYER_ID_HOME_F     ( 0x00002000 | DEMO_SEAT_ID_IVI_F )
#define DEMO_LAYER_ID_HOME_L     ( 0x00002000 | DEMO_SEAT_ID_RSE_L )
#define DEMO_LAYER_ID_HOME_R     ( 0x00002000 | DEMO_SEAT_ID_RSE_R )

#define DEMO_LAYER_ID_HMI_F      ( 0x00005000 | DEMO_SEAT_ID_IVI_F )
#define DEMO_LAYER_ID_HMI_L      ( 0x00005000 | DEMO_SEAT_ID_RSE_L )
#define DEMO_LAYER_ID_HMI_R      ( 0x00005000 | DEMO_SEAT_ID_RSE_R )

#define DEMO_LAYER_ID_NOTI_F     ( 0x00006000 | DEMO_SEAT_ID_IVI_F )
#define DEMO_LAYER_ID_NOTI_L     ( 0x00006000 | DEMO_SEAT_ID_RSE_L )
#define DEMO_LAYER_ID_NOTI_R     ( 0x00006000 | DEMO_SEAT_ID_RSE_R )

#define DEMO_SURFACE_ID_HOME_F   ( 0x00000300 | DEMO_SEAT_ID_IVI_F )
#define DEMO_SURFACE_ID_HOME_L   ( 0x00000300 | DEMO_SEAT_ID_RSE_L )
#define DEMO_SURFACE_ID_HOME_R   ( 0x00000300 | DEMO_SEAT_ID_RSE_R )

#define DEMO_SURFACE_ID_BAR_F    ( 0x00002000 | DEMO_SEAT_ID_IVI_F )
#define DEMO_SURFACE_ID_BAR_L    ( 0x00002000 | DEMO_SEAT_ID_RSE_L )
#define DEMO_SURFACE_ID_BAR_R    ( 0x00002000 | DEMO_SEAT_ID_RSE_R )

#define DEMO_SURFACE_ID_TP_F     ( 0x00010000 | DEMO_SEAT_ID_IVI_F )
#define DEMO_SURFACE_ID_TP_L     ( 0x00010000 | DEMO_SEAT_ID_RSE_L )
#define DEMO_SURFACE_ID_TP_R     ( 0x00010000 | DEMO_SEAT_ID_RSE_R )

#define DEMO_SURFACE_ID_MUSIC_F  ( 0x00010100 | DEMO_SEAT_ID_IVI_F )
#define DEMO_SURFACE_ID_MUSIC_L  ( 0x00010100 | DEMO_SEAT_ID_RSE_L )
#define DEMO_SURFACE_ID_MUSIC_R  ( 0x00010100 | DEMO_SEAT_ID_RSE_R )

#define DEMO_SURFACE_ID_VIDEO_F  ( 0x00010200 | DEMO_SEAT_ID_IVI_F )
#define DEMO_SURFACE_ID_VIDEO_L  ( 0x00010200 | DEMO_SEAT_ID_RSE_L )
#define DEMO_SURFACE_ID_VIDEO_R  ( 0x00010200 | DEMO_SEAT_ID_RSE_R )


#define DEMO_GET_SEAT_ID(id)     ( (id) & 0xFF000000U )

#define DEMO_IS_HOME(id)         ( ( (id) & 0x00000300U ) == ( (id) & 0x00FFFF00U ) )
#define DEMO_IS_NOTI(id)         ( ( (id) & 0x00002000U ) == ( (id) & 0x00FFFF00U ) )

enum {
    DEMO_SCREEN_IVI_F = 0,
    DEMO_SCREEN_RSE_L,
    DEMO_SCREEN_RSE_R,
    DEMO_SCREEN_MAX
};

#define MEM_ALLOC(s) mem_alloc((s),__FILE__,__LINE__)

enum cursor_type {
	CURSOR_BOTTOM_LEFT,
	CURSOR_BOTTOM_RIGHT,
	CURSOR_BOTTOM,
	CURSOR_DRAGGING,
	CURSOR_LEFT_PTR,
	CURSOR_LEFT,
	CURSOR_RIGHT,
	CURSOR_TOP_LEFT,
	CURSOR_TOP_RIGHT,
	CURSOR_TOP,
	CURSOR_IBEAM,
	CURSOR_HAND1,
	CURSOR_WATCH,

	CURSOR_BLANK
};

typedef struct t_wlContextStruct
{
    struct wl_display* wlDisplay;
    struct wl_registry* wlRegistry;
    struct wl_compositor* wlCompositor;
    struct wl_shm			*wlShm;
    int width;
    int height;

	struct wl_seat			*wlSeat;
	struct wl_pointer		*wlPointer;
	struct wl_touch			*wlTouch;

	struct wl_cursor_theme		*cursor_theme;
	struct wl_cursor		**cursors;
	struct wl_surface		*pointer_surface;
	enum   cursor_type		current_cursor;
	struct wl_list			list_wlContextStruct;
	uint32_t			enter_serial;
	struct wl_surface		*enterSurface;

    uint32_t mask;
    uint32_t			formats;

} WLContextStruct;

#define wlContextCommon t_wlContextStruct

struct hmi_homescreen_srf {
	uint32_t	id;
	char		*filePath;
	uint32_t	color;
};

struct wlContextStruct {
	struct t_wlContextStruct	*cmm;
	struct wl_surface	*wlSurface;
	struct wl_buffer	*wlBuffer;
	cairo_surface_t		*ctx_image;
	void			*data;
	uint32_t		id_surface;
	struct wl_list		link;
};

#ifdef  __cplusplus
extern "C" {
#endif

extern t_ilm_uint layerHome[3];
extern t_ilm_uint layerApplication[3];
extern t_ilm_uint layerBar[3];

extern unsigned int getSeatIndexBySurfaceId(unsigned int surface);
extern void do_start_execv(const char* bin, unsigned int surfaceID, unsigned int layerId, int width, int height);

typedef void(*buttonPressFunc)(unsigned int surfaceID);
typedef void(*buttonReleaseFunc)(unsigned int surfaceID);

extern void create_cursors(WLContextStruct *p_wlCtx);
extern void destroy_cursors(WLContextStruct *cmm);

extern void registerCursor(buttonPressFunc down, buttonReleaseFunc up);
extern void registerTouch(buttonPressFunc down, buttonReleaseFunc up);

extern int32_t getIdOfWlSurface(WLContextStruct *pCtx, struct wl_surface *wlSurface);

extern void create_background(struct wlContextStruct *p_wlCtx, const uint32_t id_surface, const char *imageFile, surfaceNotificationFunc func);
extern void create_button(struct wlContextStruct *p_wlCtx, const uint32_t id_surface, const char *imageFile, uint32_t number, surfaceNotificationFunc func);

extern void printSurfaceProperties(unsigned int surfaceid, const char* prefix);

#ifdef  __cplusplus
}
#endif

#endif
