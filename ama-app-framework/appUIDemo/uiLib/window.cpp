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

#include "hmi.h"
#include "common.h"

static void frame_listener_func(void *data, struct wl_callback *callback, uint32_t time)
{
	// LOGF("\n");

	if (callback)
		wl_callback_destroy(callback);
}

static const struct wl_callback_listener frame_listener = {
	frame_listener_func
};

static void drawImage(struct wlContextStruct *p_wlCtx)
{
	// LOGF("\n");

	struct wl_callback *callback;

	int width = 0;
	int height = 0;
	int stride = 0;
	void *data = NULL;

	width = cairo_image_surface_get_width(p_wlCtx->ctx_image);
	height = cairo_image_surface_get_height(p_wlCtx->ctx_image);
	stride = cairo_image_surface_get_stride(p_wlCtx->ctx_image);
	data = cairo_image_surface_get_data(p_wlCtx->ctx_image);

	LOGI("drawImage : %dx%d\n", width, height);

	memcpy(p_wlCtx->data, data, stride * height);

	wl_surface_attach(p_wlCtx->wlSurface, p_wlCtx->wlBuffer, 0, 0);
	wl_surface_damage(p_wlCtx->wlSurface, 0, 0, width, height);

	callback = wl_surface_frame(p_wlCtx->wlSurface);
	wl_callback_add_listener(callback, &frame_listener, NULL);

	wl_surface_commit(p_wlCtx->wlSurface);
}

static void
createShmBuffer(struct wlContextStruct *p_wlCtx)
{
	// LOGF("\n");

	struct wl_shm_pool *pool;

	int fd = -1;
	int size = 0;
	int width = 0;
	int height = 0;
	int stride = 0;

	width  = cairo_image_surface_get_width(p_wlCtx->ctx_image);
	height = cairo_image_surface_get_height(p_wlCtx->ctx_image);
	stride = cairo_image_surface_get_stride(p_wlCtx->ctx_image);

	size = stride * height;

	fd = os_create_anonymous_file(size);
	if (fd < 0) {
		LOGE("creating a buffer file for %d B failed: %m\n",
			size);
		return ;
	}

	p_wlCtx->data =
		mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (MAP_FAILED == p_wlCtx->data) {
		LOGE("mmap failed: %m\n");
		close(fd);
		return;
	}

	pool = wl_shm_create_pool(p_wlCtx->cmm->wlShm, fd, size);
	p_wlCtx->wlBuffer = wl_shm_pool_create_buffer(pool, 0,
						      width,
						      height,
						      stride,
						      WL_SHM_FORMAT_ARGB8888);

	if (NULL == p_wlCtx->wlBuffer) {
		LOGE("wl_shm_create_buffer failed: %m\n");
		close(fd);
		return;
	}

	wl_shm_pool_destroy(pool);
	close(fd);
}

static void create_ivisurface(struct wlContextStruct *p_wlCtx,
				  uint32_t id_surface,
				  cairo_surface_t *surface)
{
	// LOGF("\n");

	CHECK_IF_FAIL(p_wlCtx->cmm->wlCompositor != NULL);

	p_wlCtx->ctx_image = surface;
	p_wlCtx->id_surface = id_surface;

	wl_list_init(&p_wlCtx->link);
	wl_list_insert(&p_wlCtx->cmm->list_wlContextStruct, &p_wlCtx->link);

	p_wlCtx->wlSurface = wl_compositor_create_surface(p_wlCtx->cmm->wlCompositor);
	CHECK_IF_FAIL(p_wlCtx->wlSurface != NULL);

	createShmBuffer(p_wlCtx);

	int width = cairo_image_surface_get_width(p_wlCtx->ctx_image);
	int height = cairo_image_surface_get_height(p_wlCtx->ctx_image);

	LOGI("ilm_surfaceCreate(%dx%d)\n", width, height);
	ilmErrorTypes err = ilm_surfaceCreate((t_ilm_nativehandle)p_wlCtx->wlSurface, width, height,
            ILM_PIXELFORMAT_RGBA_8888, &p_wlCtx->id_surface);

	if(err != ILM_SUCCESS) {
        LOGE("ilm_surfaceCreate : %s", ILM_ERROR_STRING(err));
    }

	drawImage(p_wlCtx);
}

static void create_ivisurfaceFromFile(struct wlContextStruct *p_wlCtx,
			  uint32_t id_surface,
			  const char *imageFile)
{
	LOGI("create_ivisurfaceFromFile(0x%08X, %s)\n", id_surface, imageFile);

	cairo_surface_t *surface = load_cairo_surface(imageFile);

	if (NULL == surface) {
		fprintf(stderr, "Failed to load_cairo_surface %s\n", imageFile);
		return;
	}

	create_ivisurface(p_wlCtx, id_surface, surface);
}

void create_background(struct wlContextStruct *p_wlCtx, const uint32_t id_surface, const char *imageFile, surfaceNotificationFunc func)
{
	create_ivisurfaceFromFile(p_wlCtx, id_surface, imageFile);

	ilm_surfaceAddNotification(id_surface, func);
}

void create_button(struct wlContextStruct *p_wlCtx, const uint32_t id_surface, const char *imageFile, uint32_t number, surfaceNotificationFunc func)
{
	create_ivisurfaceFromFile(p_wlCtx, id_surface, imageFile);

	ilm_surfaceAddNotification(id_surface, func);
}
