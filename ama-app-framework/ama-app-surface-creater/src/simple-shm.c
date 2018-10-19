/*
 * The MIT License
 *
 * Copyright © 2016-2017 Neusoft, Inc.
 * Copyright © 2011 Benjamin Franzke
 * Copyright © 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#include <wayland-client.h>
#include "helpers.h"
#include "os-compatibility.h"

#include <sys/types.h>
#include "ivi-application-client-protocol.h"

#include "SurfaceCreater.h"
#include "HandleManagement.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

struct seat {
    struct display *display;
    struct wl_seat *seat;
    struct wl_touch *wl_touch;
    struct seat* next_seat;
};

struct display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	uint32_t formats;
	struct ivi_application *ivi_application;
    //int width, height;
    //void *data;
    unsigned int surface_id;
    TouchCallback touch;
    struct seat* seat_head;
	// weston touch bug workaround begin
    struct wl_seat *seat;
    struct wl_touch *wl_touch;
	// weston touch bug workaround end
};

struct buffer {
	struct wl_buffer *buffer;
	void *shm_data;
	int busy;
};

struct window {
	struct display *display;
	int width, height;
	struct wl_surface *surface;
	struct ivi_surface *ivi_surface;
	struct buffer buffers[2];
	struct buffer *prev_buffer;
	struct wl_callback *callback;
    RedrawCallback draw;
};

static int running = 1;

static void
buffer_release(void *data, struct wl_buffer *buffer)
{
	struct buffer *mybuf = data;

	mybuf->busy = 0;
}

static const struct wl_buffer_listener buffer_listener = {
	buffer_release
};

static int
create_shm_buffer(struct display *display, struct buffer *buffer,
		  int width, int height, uint32_t format)
{
	struct wl_shm_pool *pool;
	int fd, size, stride;
	void *data;

	stride = width * 4;
	size = stride * height;

	fd = os_create_anonymous_file(size);
	if (fd < 0) {
		fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
			size);
		return -1;
	}

	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %m\n");
		close(fd);
		return -1;
	}

	pool = wl_shm_create_pool(display->shm, fd, size);
	buffer->buffer = wl_shm_pool_create_buffer(pool, 0,
						   width, height,
						   stride, format);
	wl_buffer_add_listener(buffer->buffer, &buffer_listener, buffer);
	wl_shm_pool_destroy(pool);
	close(fd);

	buffer->shm_data = data;

	return 0;
}

static void
handle_ivi_surface_configure(void *data, struct ivi_surface *ivi_surface,
			     int32_t width, int32_t height)
{
	/* Simple-shm is resizable */
}

static const struct ivi_surface_listener ivi_surface_listener = {
	handle_ivi_surface_configure,
};

static struct window *
create_window(struct display *display, int width, int height, unsigned int surfaceID)
{
	struct window *window;

	window = ama_calloc(1, sizeof *window);
	if (!window)
		return NULL;

    printf("create_window(): 1\n");
	window->callback = NULL;
	window->display = display;
	window->width = width;
	window->height = height;
	window->surface = wl_compositor_create_surface(display->compositor);

    printf("create_window(): 2\n");
    if (display->ivi_application ) {
        printf("create_window(): 4\n");
        uint32_t id_ivisurf = surfaceID/*IVI_SURFACE_ID + (uint32_t)getpid()*/;
        window->ivi_surface =
            ivi_application_surface_create(display->ivi_application,
                id_ivisurf, window->surface);
        if (window->ivi_surface == NULL) {
            fprintf(stderr, "Failed to create ivi_client_surface\n");
            abort();
        }

        ivi_surface_add_listener(window->ivi_surface,
            &ivi_surface_listener, window);
        printf("create_window(): 5\n");
    } else {
		assert(0);
	}
    printf("create_window(): 3\n");
	return window;
}

static void
destroy_window(struct window *window)
{
	if (window->callback)
		wl_callback_destroy(window->callback);

	if (window->buffers[0].buffer)
		wl_buffer_destroy(window->buffers[0].buffer);
	if (window->buffers[1].buffer)
		wl_buffer_destroy(window->buffers[1].buffer);

	wl_surface_destroy(window->surface);
	ama_free(window);
}

static struct buffer *
window_next_buffer(struct window *window)
{
	struct buffer *buffer;
	int ret = 0;

	if (!window->buffers[0].busy)
		buffer = &window->buffers[0];
	else if (!window->buffers[1].busy)
		buffer = &window->buffers[1];
	else
		return NULL;

	if (!buffer->buffer) {
		ret = create_shm_buffer(window->display, buffer,
					window->width, window->height,
					WL_SHM_FORMAT_ARGB8888);

		if (ret < 0)
			return NULL;

		/* paint the padding */
		memset(buffer->shm_data, 0xff,
		       window->width * window->height * 4);
	}

	return buffer;
}

static void
paint_pixels(void *image, int padding, int width, int height, uint32_t time, RedrawCallback callback)
{
    if(callback)
        callback(image, width, height);
    else
    {
        memset(image, 0, width*height*4);
        const int halfh  = padding + (height - padding*2)/2;
        const int halfw  = padding + (width - padding*2)/2;
        int       ir, or;
        uint32_t  *pixel = image;
        int       y;

        /* squared radii thresholds */
        or    = (halfw < halfh?halfw:halfh) - 8;
        ir    = or - 32;
        or *= or;
        ir *= ir;

        pixel += padding*width;
        for(y = padding; y < height - padding; y++)
        {
            int x;
            int y2 = (y - halfh)*(y - halfh);

            pixel += padding;
            for(x  = padding; x < width - padding; x++)
            {
                uint32_t v;

                /* squared distance from center */
                int r2 = (x - halfw)*(x - halfw) + y2;

                if(r2 < ir)
                    v = (r2/32 + time/64)*0x0080401;
                else if(r2 < or)
                    v = (y + time/32)*0x0080401;
                else
                    v = (x + time/16)*0x0080401;
                v &= 0x00ffffff;

                /* cross if compositor uses X from XRGB as alpha */
                if(abs(x - y) > 6 && abs(x + y - height) > 6)
                    v = 0x00000000;

                *pixel++ = v;
            }

            pixel += padding;
        }
    }
}

static const struct wl_callback_listener frame_listener;

static void
redraw(void *data, struct wl_callback *callback, uint32_t time)
{
	struct window *window = data;
	struct buffer *buffer;

	buffer = window_next_buffer(window);
	if (!buffer) {
		fprintf(stderr,
			!callback ? "Failed to create the first buffer.\n" :
			"Both buffers busy at redraw(). Server bug?\n");
		return;
	}

    paint_pixels(buffer->shm_data, 20, window->width, window->height, time, window->draw);

	wl_surface_attach(window->surface, buffer->buffer, 0, 0);
	wl_surface_damage(window->surface,
			  0, 0, window->width, window->height);

	if (callback)
		wl_callback_destroy(callback);
    if(callback==window->callback)
        window->callback = NULL;

//	window->callback = wl_surface_frame(window->surface);
//	wl_callback_add_listener(window->callback, &frame_listener, window);
	wl_surface_commit(window->surface);
	buffer->busy = 1;
}

static const struct wl_callback_listener frame_listener = {
	redraw
};

static void
shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
	struct display *d = data;

	d->formats |= (1 << format);
}

struct wl_shm_listener shm_listener = {
	shm_format
};

static void
touch_handle_down(void *data, struct wl_touch *wl_touch,
                  uint32_t serial, uint32_t time, struct wl_surface *surface,
                  int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct display *display = data;
    float x = wl_fixed_to_double(x_w);
    float y = wl_fixed_to_double(y_w);

    //touch_paint(touch, x, y, id);
    if(display->touch)
        display->touch(display->surface_id, E_TOUCH_DOWN, x, y, id, time);
}

static void
touch_handle_up(void *data, struct wl_touch *wl_touch,
                uint32_t serial, uint32_t time, int32_t id)
{
    struct display *display = data;

    if(display->touch)
        display->touch(display->surface_id, E_TOUCH_UP, -1.0f, -1.0f, id, time);
}

static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
                    uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
    struct display *display = data;
    float x = wl_fixed_to_double(x_w);
    float y = wl_fixed_to_double(y_w);

    //touch_paint(display, x, y, id);
    if(display->touch)
        display->touch(display->surface_id, E_TOUCH_MOVE, x, y, id, time);
}

static void
touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
}

static void
touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
}

static const struct wl_touch_listener touch_listener = {
    touch_handle_down,
    touch_handle_up,
    touch_handle_motion,
    touch_handle_frame,
    touch_handle_cancel,
};

// weston touch bug workaround begin
// original code
//static void
//seat_handle_capabilities(void *data, struct wl_seat *wl_seat,
//                         enum wl_seat_capability caps)
//{
//    struct seat *seat = data;
//    struct display *display = seat->display;
//
//    if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !seat->wl_touch) {
//        seat->wl_touch = wl_seat_get_touch(wl_seat);
//        wl_touch_set_user_data(seat->wl_touch, display);
//        wl_touch_add_listener(seat->wl_touch, &touch_listener, display);
//    } else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && seat->wl_touch) {
//        wl_touch_destroy(seat->wl_touch);
//        seat->wl_touch = NULL;
//    }
//}

// new code
static void
seat_handle_capabilities(void *data, struct wl_seat *wl_seat,
						 enum wl_seat_capability caps)
{
	struct display *d = data;

	if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !d->wl_touch) {
		d->wl_touch = wl_seat_get_touch(wl_seat);
		wl_touch_set_user_data(d->wl_touch, d);
		wl_touch_add_listener(d->wl_touch, &touch_listener, d);
	} else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && d->wl_touch) {
		wl_touch_destroy(d->wl_touch);
		d->wl_touch = NULL;
	}
}

static void seat_handle_name(void *data,
							 struct wl_seat *wl_seat,
							 const char *name)
{

}
// weston touch bug workaround end

static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
    seat_handle_name // weston touch bug workaround
};

static void
add_seat(struct display *d, uint32_t name, uint32_t version)
{
    struct seat *seat;

    seat = ama_malloc(sizeof *seat);
    assert(seat);

    seat->display = d;
    seat->next_seat = NULL;
    if(d->seat_head)
    {
        struct seat *next_seat = d->seat_head;
        while (next_seat->next_seat)
            next_seat = next_seat->next_seat;
        next_seat->next_seat = seat;
    }
    else
        d->seat_head = seat;
    seat->wl_touch = NULL;
    seat->seat = wl_registry_bind(d->registry, name,
        &wl_seat_interface, 1);
    wl_seat_add_listener(seat->seat, &seat_listener, seat);
}

static void
registry_handle_global(void *data, struct wl_registry *registry,
		       uint32_t id, const char *interface, uint32_t version)
{
	struct display *d = data;

	if (strcmp(interface, "wl_compositor") == 0) {
		d->compositor =
			wl_registry_bind(registry,
					 id, &wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shm") == 0) {
		d->shm = wl_registry_bind(registry,
					  id, &wl_shm_interface, 1);
		wl_shm_add_listener(d->shm, &shm_listener, d);
	}
	else if (strcmp(interface, "ivi_application") == 0) {
		d->ivi_application =
			wl_registry_bind(registry, id,
					 &ivi_application_interface, 1);
	} else if (strcmp(interface, "wl_seat") == 0) {
		// weston touch bug workaround begin
		// original code
		//add_seat(d, id, version);

		// new code
		d->seat = (struct wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 1);
		wl_seat_add_listener(d->seat, &seat_listener, d);
		// weston touch bug workaround end
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
			      uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global,
	registry_handle_global_remove
};

static struct display *
create_display(void)
{
	struct display *display;

	display = ama_malloc(sizeof *display);
	if (display == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}
	memset(display, 0, sizeof(*display));
    display->seat_head = NULL;
	display->display = wl_display_connect(NULL);
	assert(display->display);

	display->formats = 0;
	display->registry = wl_display_get_registry(display->display);
	wl_registry_add_listener(display->registry,
				 &registry_listener, display);
	wl_display_roundtrip(display->display);
	if (display->shm == NULL) {
		fprintf(stderr, "No wl_shm global\n");
		exit(1);
	}

	wl_display_roundtrip(display->display);

	/*
	 * Why do we need two roundtrips here?
	 *
	 * wl_display_get_registry() sends a request to the server, to which
	 * the server replies by emitting the wl_registry.global events.
	 * The first wl_display_roundtrip() sends wl_display.sync. The server
	 * first processes the wl_display.get_registry which includes sending
	 * the global events, and then processes the sync. Therefore when the
	 * sync (roundtrip) returns, we are guaranteed to have received and
	 * processed all the global events.
	 *
	 * While we are inside the first wl_display_roundtrip(), incoming
	 * events are dispatched, which causes registry_handle_global() to
	 * be called for each global. One of these globals is wl_shm.
	 * registry_handle_global() sends wl_registry.bind request for the
	 * wl_shm global. However, wl_registry.bind request is sent after
	 * the first wl_display.sync, so the reply to the sync comes before
	 * the initial events of the wl_shm object.
	 *
	 * The initial events that get sent as a reply to binding to wl_shm
	 * include wl_shm.format. These tell us which pixel formats are
	 * supported, and we need them before we can create buffers. They
	 * don't change at runtime, so we receive them as part of init.
	 *
	 * When the reply to the first sync comes, the server may or may not
	 * have sent the initial wl_shm events. Therefore we need the second
	 * wl_display_roundtrip() call here.
	 *
	 * The server processes the wl_registry.bind for wl_shm first, and
	 * the second wl_display.sync next. During our second call to
	 * wl_display_roundtrip() the initial wl_shm events are received and
	 * processed. Finally, when the reply to the second wl_display.sync
	 * arrives, it guarantees we have processed all wl_shm initial events.
	 *
	 * This sequence contains two examples on how wl_display_roundtrip()
	 * can be used to guarantee, that all reply events to a request
	 * have been received and processed. This is a general Wayland
	 * technique.
	 */

	if (!(display->formats & (1 << WL_SHM_FORMAT_ARGB8888))) {
		fprintf(stderr, "WL_SHM_FORMAT_ARGB32 not available\n");
		exit(1);
	}

	return display;
}

static void free_seat(struct seat* seat_head)
{
    if(seat_head->next_seat)
        free_seat(seat_head->next_seat);
    ama_free(seat_head);
}

static void
destroy_display(struct display *display)
{
	if (display->shm)
		wl_shm_destroy(display->shm);

	if (display->compositor)
		wl_compositor_destroy(display->compositor);

	wl_registry_destroy(display->registry);
	wl_display_flush(display->display);
	wl_display_disconnect(display->display);

    if(display->seat_head)
        free_seat(display->seat_head);

	ama_free(display);
}

static void
signal_int(int signum)
{
	running = 0;
}

void CreateSurface(unsigned int surfaceID, unsigned int width, unsigned int height, RedrawCallback callback, TouchCallback tCallback)
{
	struct sigaction sigint;
	struct display *display;
	struct window *window;
	int ret = 0;

    CreateHandle(surfaceID);

    printf("before create_display\n");
	display = create_display();
    printf("after create_display\n");

    printf("before create_window\n");
    window = create_window(display, width, height, surfaceID);
    printf("window = %lu\n", (unsigned long)(window));
    if (!window)
        return;
    window->draw = callback;
    window->display->touch = tCallback;
    window->display->surface_id = surfaceID;
    AssociateObject2Handle(surfaceID, window);
    printf("after create_window\n");

	//sigint.sa_handler = signal_int;
	//sigemptyset(&sigint.sa_mask);
	//sigint.sa_flags = SA_RESETHAND;
	//sigaction(SIGINT, &sigint, NULL);

	/* Initialise damage to full surface, so the padding gets painted */
	wl_surface_damage(window->surface, 0, 0,
			  window->width, window->height);

	redraw(window, NULL, 0);

	while (IsHandleExists(surfaceID) && ret != -1)
        ret = wl_display_dispatch(display->display);

	fprintf(stderr, "simple-shm exiting\n");

	if (window->display->ivi_application) {
		ivi_surface_destroy(window->ivi_surface);
		ivi_application_destroy(window->display->ivi_application);
	}

	destroy_window(window);
	destroy_display(display);

	return;
}

void DestroySurface(unsigned int surfaceID)
{
    struct window* window = GetObjectFromHandle(surfaceID);

    DeleteHandle(surfaceID);
    if(window)
        wl_display_roundtrip(window->display->display);
}

void ForceRedrawSurface(unsigned int surfaceID)
{
    struct window* window = GetObjectFromHandle(surfaceID);
    if(window)
    {
        window->callback = wl_surface_frame(window->surface);
        wl_callback_add_listener(window->callback, &frame_listener, window);
        wl_surface_commit(window->surface);
        wl_display_roundtrip(window->display->display);
    }
}

void Transparent(unsigned int* pixels, unsigned int width, unsigned int height)
{
    memset(pixels, 0, width*height*4);
}

void Black(unsigned int* pixels, unsigned int width, unsigned int height)
{
    for(int i = 0; i < height; ++i)
        for(int j = 0; j < width; ++j)
            pixels[width*i+j] = 0xFF000000;
}
