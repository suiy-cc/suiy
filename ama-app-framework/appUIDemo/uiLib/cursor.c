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

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

static buttonPressFunc sHandlerDown;
static buttonReleaseFunc sHandlerUp;

static void *
fail_on_null(void *p, size_t size, const char *file, int32_t line)
{
	if (size && !p) {
		fprintf(stderr, "%s(%d) %zd: out of memory\n",
			file, line, size);
		exit(EXIT_FAILURE);
	}

	return p;
}

static void *
mem_alloc(size_t size, const char *file, int32_t line)
{
	return fail_on_null(ama_calloc(1, size), size, file, line);
}

static void
set_pointer_image(struct wlContextCommon *pCtx, uint32_t index)
{
	struct wl_cursor *cursor = NULL;
	struct wl_cursor_image *image = NULL;
	struct wl_buffer *buffer = NULL;

	if (!pCtx->wlPointer || !pCtx->cursors)
		return;

	if (CURSOR_BLANK == pCtx->current_cursor) {
		wl_pointer_set_cursor(pCtx->wlPointer, pCtx->enter_serial,
							  NULL, 0, 0);
		return;
	}

	cursor = pCtx->cursors[pCtx->current_cursor];
	if (!cursor)
		return;

	if (cursor->image_count <= index) {
		fprintf(stderr, "cursor index out of range\n");
		return;
	}

	image = cursor->images[index];
	buffer = wl_cursor_image_get_buffer(image);

	if (!buffer)
		return;

	wl_pointer_set_cursor(pCtx->wlPointer, pCtx->enter_serial,
			      pCtx->pointer_surface,
			      image->hotspot_x, image->hotspot_y);

	wl_surface_attach(pCtx->pointer_surface, buffer, 0, 0);

	wl_surface_damage(pCtx->pointer_surface, 0, 0,
					  image->width, image->height);

	wl_surface_commit(pCtx->pointer_surface);
}
static void
PointerHandleEnter(void *data, struct wl_pointer *wlPointer, uint32_t serial,
		   struct wl_surface *wlSurface, wl_fixed_t sx, wl_fixed_t sy)
{
	LOGF("(serial=%d, x=%d, y=%d)\n", serial, sx, sy);

	struct wlContextCommon *pCtx = (struct wlContextCommon *)data;

	pCtx->enter_serial = serial;
	pCtx->enterSurface = wlSurface;
	set_pointer_image((struct wlContextCommon*)data, 0);
#ifdef _DEBUG
	printf("ENTER PointerHandleEnter: x(%d), y(%d)\n", sx, sy);
#endif
}

static void
PointerHandleLeave(void *data, struct wl_pointer *wlPointer, uint32_t serial,
		   struct wl_surface *wlSurface)
{
	LOGF("\n");

	struct wlContextCommon *pCtx = (struct wlContextCommon *)data;

	pCtx->enterSurface = NULL;

#ifdef _DEBUG
	printf("ENTER PointerHandleLeave: serial(%d)\n", serial);
#endif
}

static void
PointerHandleMotion(void *data, struct wl_pointer *wlPointer, uint32_t time,
		    wl_fixed_t sx, wl_fixed_t sy)
{
//	LOGF("\n");

#ifdef _DEBUG
	printf("ENTER PointerHandleMotion: x(%d), y(%d)\n", sx, sy);
#endif
}

/**
 * Even handler of Pointer event. IVI system is usually manipulated by touch
 * screen. However, some systems also have pointer device.
 * Release is the same behavior as touch off
 * Pressed is the same behavior as touch on
 */
static void
PointerHandleButton(void *data, struct wl_pointer *wlPointer, uint32_t serial,
		    uint32_t time, uint32_t button, uint32_t state)
{
	if(sHandlerDown == NULL) {
		return;
	}

	if(sHandlerUp == NULL) {
		return;
	}

	if (BTN_RIGHT == button)
		return;

	struct wlContextCommon *pCtx = (struct wlContextCommon *)data;

	const uint32_t id_surface = getIdOfWlSurface(pCtx, pCtx->enterSurface);
	LOGF("PointerHandleButton(surfaceID=%d)\n", id_surface);


	switch (state) {
		case WL_POINTER_BUTTON_STATE_RELEASED:
			sHandlerDown(id_surface);
			break;

		case WL_POINTER_BUTTON_STATE_PRESSED:
			sHandlerUp(id_surface);
			break;
	}
}

static void
PointerHandleAxis(void *data, struct wl_pointer *wlPointer, uint32_t time,
		  uint32_t axis, wl_fixed_t value)
{
#ifdef _DEBUG
	printf("ENTER PointerHandleAxis: axis(%d), value(%d)\n", axis, value);
#endif
}

struct wl_pointer_listener pointer_listener = {
	PointerHandleEnter,
	PointerHandleLeave,
	PointerHandleMotion,
	PointerHandleButton,
	PointerHandleAxis
};

/*
 * The following correspondences between file names and cursors was copied
 * from: https://bugs.kde.org/attachment.cgi?id=67313
 */
static const char *bottom_left_corners[] = {
	"bottom_left_corner",
	"sw-resize",
	"size_bdiag"
};

static const char *bottom_right_corners[] = {
	"bottom_right_corner",
	"se-resize",
	"size_fdiag"
};

static const char *bottom_sides[] = {
	"bottom_side",
	"s-resize",
	"size_ver"
};

static const char *grabbings[] = {
	"grabbing",
	"closedhand",
	"208530c400c041818281048008011002"
};

static const char *left_ptrs[] = {
	"left_ptr",
	"default",
	"top_left_arrow",
	"left-arrow"
};

static const char *left_sides[] = {
	"left_side",
	"w-resize",
	"size_hor"
};

static const char *right_sides[] = {
	"right_side",
	"e-resize",
	"size_hor"
};

static const char *top_left_corners[] = {
	"top_left_corner",
	"nw-resize",
	"size_fdiag"
};

static const char *top_right_corners[] = {
	"top_right_corner",
	"ne-resize",
	"size_bdiag"
};

static const char *top_sides[] = {
	"top_side",
	"n-resize",
	"size_ver"
};

static const char *xterms[] = {
	"xterm",
	"ibeam",
	"text"
};

static const char *hand1s[] = {
	"hand1",
	"pointer",
	"pointing_hand",
	"e29285e634086352946a0e7090d73106"
};

static const char *watches[] = {
	"watch",
	"wait",
	"0426c94ea35c87780ff01dc239897213"
};

struct cursor_alternatives {
	const char **names;
	size_t count;
};

static const struct cursor_alternatives cursorsAlt[] = {
	{ bottom_left_corners, ARRAY_LENGTH(bottom_left_corners) },
	{ bottom_right_corners, ARRAY_LENGTH(bottom_right_corners) },
	{ bottom_sides, ARRAY_LENGTH(bottom_sides) },
	{ grabbings, ARRAY_LENGTH(grabbings) },
	{ left_ptrs, ARRAY_LENGTH(left_ptrs) },
	{ left_sides, ARRAY_LENGTH(left_sides) },
	{ right_sides, ARRAY_LENGTH(right_sides) },
	{ top_left_corners, ARRAY_LENGTH(top_left_corners) },
	{ top_right_corners, ARRAY_LENGTH(top_right_corners) },
	{ top_sides, ARRAY_LENGTH(top_sides) },
	{ xterms, ARRAY_LENGTH(xterms) },
	{ hand1s, ARRAY_LENGTH(hand1s) },
	{ watches, ARRAY_LENGTH(watches) },
};

void create_cursors(WLContextStruct *p_wlCtx)
{
	LOGF("\n");

	uint32_t i = 0;
	uint32_t j = 0;
	struct wl_cursor *cursor = NULL;

	p_wlCtx->cursor_theme = wl_cursor_theme_load("default", 32, p_wlCtx->wlShm);

	p_wlCtx->cursors = (struct wl_cursor**)MEM_ALLOC(ARRAY_LENGTH(cursorsAlt) * sizeof(cursorsAlt[0]));

	for (i = 0; i < ARRAY_LENGTH(cursorsAlt); i++) {
		cursor = NULL;

		for (j = 0; !cursor && j < cursorsAlt[i].count; ++j) {
			cursor = wl_cursor_theme_get_cursor(p_wlCtx->cursor_theme, cursorsAlt[i].names[j]);
		}

		if (!cursor) {
			fprintf(stderr, "could not load cursor '%s'\n", cursorsAlt[i].names[0]);
		}

		p_wlCtx->cursors[i] = cursor;
	}
}

void destroy_cursors(WLContextStruct *cmm)
{
	LOGF("\n");

	if (cmm->cursor_theme) {
		wl_cursor_theme_destroy(cmm->cursor_theme);
	}

	ama_free(cmm->cursors);
}

void registerCursor(buttonPressFunc down, buttonReleaseFunc up)
{
	LOGF("\n");

	sHandlerDown = down;
	sHandlerUp = up;
}
