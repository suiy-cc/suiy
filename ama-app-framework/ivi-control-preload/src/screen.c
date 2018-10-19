/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
//
// Created by zs on 17-1-10.
//

#include <memory.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "ivi-controller-interface.h"
#include "ivi-layout-export.h"
#include "ama_enum.h"
#include "LayerID.h"
#include "screen.h"

enum E_RAW_SCREEN{
    E_RAW_SCREEN_VM = 0,
    E_RAW_SCREEN_IVI = 1,
    E_RAW_SCREEN_RSE = 2,
    E_RAW_SCREEN_CLUSTER = 3,
};

static const struct screen_output raw_screen_output_setting[] = {
    {0, 0, 1280, 720}, // VM
#ifdef CODE_FOR_HS5
    {1920, 0, 1920, 720}, // IVI
#else // CODE_FOR_HS5
    {1920, 0, 1280, 720}, // IVI
#endif // CODE_FOR_HS5
#ifdef WITH_IAS_BACKEND
    {3200, 0, 2560, 720}, // RSE
#else
    {640, 720, 2560, 720}, // RSE
#endif
    {0, 0, 1920, 720}, // cluster
};

static const struct screen_output virtual_screen_output_setting[] = {
    {0, 0, 1280, 720}, // VM
#ifdef CODE_FOR_HS5
    {1920, 0, 1920, 720}, // IVI
#else // CODE_FOR_HS5
    {1920, 0, 1280, 720}, // IVI
#endif // CODE_FOR_HS5
#ifdef WITH_IAS_BACKEND
    {3200, 0, 1280, 720}, // RSE1
    {4480, 0, 1280, 720}, // RSE2
#else
    {640, 720, 1280, 720}, // RSE1
    {1920, 720, 1280, 720}, // RSE2
#endif
    {0, 0, 1920, 720}, // cluster
};

static bool equal(const struct screen_output* s1, const struct screen_output* s2)
{
    if(!s1 || !s2)
        return false;
    return s1->x==s2->x
        && s1->y==s2->y
        && s1->width==s2->width
        && s1->height==s2->height;
}

static void get_layer_ids_of_cluster(unsigned int **layer_id_array, unsigned int *layer_count)
{
    void *handle = dlopen("/usr/lib/cluster-control-preload.so", RTLD_LAZY|RTLD_LOCAL);
    if(handle)
    {
        typedef void (*cluster_layer_type)(unsigned int **, unsigned int *);
        cluster_layer_type get_cluster_layers = (cluster_layer_type)dlsym(handle, "get_cluster_layers");
        if(get_cluster_layers)
        {
            get_cluster_layers(layer_id_array, layer_count);
            dlclose(handle);
            return;
        }
        weston_log( "expected function \"void get_cluster_layers(unsigned int **, unsigned int *)\" is missing!\n" );
        dlclose(handle);
    }
    weston_log( "cluster-control-preload.so is missing!\n" );

    static const unsigned int cluster_layer_id_array[] = {
        0x0500,
        0x1000,
        0x2000,
    };
    unsigned int cluster_layer_count = sizeof(cluster_layer_id_array)/sizeof(unsigned int);

    if(layer_count)
        *layer_count = cluster_layer_count;
    if(layer_id_array)
    {
        (*layer_id_array) = malloc(sizeof(unsigned int)*cluster_layer_count);

        for(int i = 0; i < cluster_layer_count; ++i)
            (*layer_id_array)[i] = cluster_layer_id_array[i];
    }
}

// VM layers
void get_layer_ids_0(struct screen* self, unsigned int **layer_id_array, unsigned int* layer_count)
{
    // get ivi layer-IDs
    unsigned int *std_layer_id_array = NULL;
    unsigned int std_layer_count = 0;
    get_layer_ids_of_ivi(0, &std_layer_id_array, &std_layer_count);

    // get cluster layer-IDs
    unsigned int* cluster_layer_id_array = NULL;
    unsigned int cluster_layer_count = 0;
    get_layer_ids_of_cluster(&cluster_layer_id_array, &cluster_layer_count);

    // merge cluster & ivi
    unsigned int** total_layer_id_array = layer_id_array;
    unsigned int total_layer_count = cluster_layer_count + std_layer_count;

    if(layer_count)
        *layer_count = total_layer_count;
    if(total_layer_id_array)
    {
        (*total_layer_id_array) = malloc(sizeof(unsigned int)*total_layer_count);

        for(int i = 0; i < cluster_layer_count; ++i)
            (*total_layer_id_array)[i] = cluster_layer_id_array[i];

        for(int i = cluster_layer_count; i < total_layer_count && std_layer_id_array; ++i)
            (*total_layer_id_array)[i] = std_layer_id_array[i-cluster_layer_count];
    }

    // release cluster & ivi layer-IDs' array
    free(cluster_layer_id_array);
    free(std_layer_id_array);
}

void get_layer_ids_1(struct screen* self, unsigned int **layer_id_array, unsigned int* layer_count)
{
    get_layer_ids_of_ivi(1, layer_id_array, layer_count);
}

void get_layer_ids_2(struct screen* self, unsigned int **layer_id_array, unsigned int* layer_count)
{
    get_layer_ids_of_ivi(2, layer_id_array, layer_count);
}

void get_layer_ids_3(struct screen* self, unsigned int **layer_id_array, unsigned int* layer_count)
{
    get_layer_ids_of_ivi(3, layer_id_array, layer_count);
}

// cluster layers
void get_layer_ids_4(struct screen* self, unsigned int **layer_id_array, unsigned int* layer_count)
{
    get_layer_ids_of_cluster(layer_id_array, layer_count);
}

static void get_raw_output(const struct ivi_controller_interface *interface, struct ivi_layout_screen* _screen, struct screen_output* s_output)
{
    if(s_output)
    {
        struct weston_output *w_output = interface->screen_get_output(_screen);

        s_output->x = w_output->x;
        s_output->y = w_output->y;
        s_output->width = w_output->width;
        s_output->height = w_output->height;
    }
}

// get VM output
void get_output_0(struct screen* self, struct screen_output* s_output)
{
    get_raw_output(self->interface, self->_screen, s_output);
}

void get_output_1(struct screen* self, struct screen_output* s_output)
{
    get_raw_output(self->interface, self->_screen, s_output);
}

void get_output_2(struct screen* self, struct screen_output* s_output)
{
    if(s_output)
        *s_output = virtual_screen_output_setting[E_SEAT_RSE1];
}

void get_output_3(struct screen* self, struct screen_output* s_output)
{
    if(s_output)
        *s_output = virtual_screen_output_setting[E_SEAT_RSE2];
}

// get cluster output
void get_output_4(struct screen* self, struct screen_output* s_output)
{
    get_raw_output(self->interface, self->_screen, s_output);
}

// set VM-screen's render order
void set_render_order_0(struct screen* self, struct ivi_layout_layer** layers, int n_layer)
{
    self->interface->screen_set_render_order(self->_screen, layers, n_layer);
}

void set_render_order_1(struct screen* self, struct ivi_layout_layer** layers, int n_layer)
{
    self->interface->screen_set_render_order(self->_screen, layers, n_layer);
}

static struct ivi_layout_layer** screen2_layers = NULL;
static int n_screen2_layers = 0;
static struct ivi_layout_layer** screen3_layers = NULL;
static int n_screen3_layers = 0;

void set_render_order_2(struct screen* self, struct ivi_layout_layer** layers, int n_layer)
{
    if(layers==NULL || n_layer==0 || self==NULL)
        return;

    free(screen2_layers);

    screen2_layers = malloc(n_layer*sizeof(struct ivi_layout_layer*));
    n_screen2_layers = n_layer;
    for(int i = 0; i < n_layer && layers; ++i)
        screen2_layers[i] = layers[i];

    int total_n_layers = n_screen2_layers + n_screen3_layers;
    struct ivi_layout_layer** total_layers = malloc(total_n_layers*sizeof(struct ivi_layout_layer*));
    for(int i = 0; i < n_screen2_layers && n_screen2_layers; ++i)
        total_layers[i] = screen2_layers[i];
    for(int i = 0; i < n_screen3_layers && n_screen3_layers; ++i)
        total_layers[i+n_screen2_layers] = screen3_layers[i];

    self->interface->screen_set_render_order(self->_screen, total_layers, total_n_layers);

    free(total_layers);
}

void set_render_order_3(struct screen* self, struct ivi_layout_layer** layers, int n_layer)
{
    if(layers==NULL || n_layer==0 || self==NULL)
        return;

    free(screen3_layers);

    screen3_layers = malloc(n_layer*sizeof(struct ivi_layout_layer*));
    n_screen3_layers = n_layer;
    for(int i = 0; i < n_layer && layers; ++i)
        screen3_layers[i] = layers[i];

    int total_n_layers = n_screen2_layers + n_screen3_layers;
    struct ivi_layout_layer** total_layers = malloc(total_n_layers*sizeof(struct ivi_layout_layer*));
    for(int i = 0; i < n_screen2_layers && n_screen2_layers; ++i)
        total_layers[i] = screen2_layers[i];
    for(int i = 0; i < n_screen3_layers && n_screen3_layers; ++i)
        total_layers[i+n_screen2_layers] = screen3_layers[i];

    self->interface->screen_set_render_order(self->_screen, total_layers, total_n_layers);

    free(total_layers);
}

// set cluster-screen's render order
void set_render_order_4(struct screen* self, struct ivi_layout_layer** layers, int n_layer)
{
    self->interface->screen_set_render_order(self->_screen, layers, n_layer);
}

static struct screen* virtual_screens[E_SEAT_MAX];
static unsigned int virtual_screens_count = 0;

static bool get_screens(const struct ivi_controller_interface *interface, unsigned int* n_screen, struct screen*** screen_array)
{
    int screen_count = 0;
    struct ivi_layout_screen** screens = NULL;
    int ret = interface->get_screens( &screen_count, &screens);

    if(ret!=IVI_SUCCEEDED)
        return false;

    int virtual_screen_count = 0;
    struct screen** virtual_screen_set = malloc(E_SEAT_MAX*sizeof(struct screen*));

    for(int i = 0; i < screen_count; ++i)
    {
        struct screen_output tmp_output = {0};
        get_raw_output(interface, screens[i], &tmp_output);

        weston_log( "%dth(wayland array's subscript) screen's output = %d, %d, %d, %d. wayland screen id = %d.\n", i, tmp_output.x, tmp_output.y, tmp_output.width, tmp_output.height, interface->get_id_of_screen(screens[i]));

        // create VM screen
        struct screen *screen0 = NULL;
        if(equal(&tmp_output, &raw_screen_output_setting[E_RAW_SCREEN_VM]))
        {
            weston_log("VM screen is found! i = %d(wayland number), output = %d, %d, %d, %d\n", i, tmp_output.x, tmp_output.y, tmp_output.width, tmp_output.height);
            screen0 = malloc(sizeof(struct screen));
            screen0->_screen          = screens[i];
            screen0->interface        = interface;
            screen0->get_layer_ids    = get_layer_ids_0;
            screen0->get_output       = get_output_0;
            screen0->set_render_order = set_render_order_0;

            virtual_screen_set[virtual_screen_count] = screen0;
            ++virtual_screen_count;
        }

        // create IVI screen
        struct screen *screen1 = NULL;
        if(equal(&tmp_output, &raw_screen_output_setting[E_RAW_SCREEN_IVI]))
        {
            weston_log("IVI screen is found! i = %d(wayland number), output = %d, %d, %d, %d\n", i, tmp_output.x, tmp_output.y, tmp_output.width, tmp_output.height);
            screen1 = malloc(sizeof(struct screen));
            screen1->_screen          = screens[i];
            screen1->interface        = interface;
            screen1->get_layer_ids    = get_layer_ids_1;
            screen1->get_output       = get_output_1;
            screen1->set_render_order = set_render_order_1;

            virtual_screen_set[virtual_screen_count] = screen1;
            ++virtual_screen_count;
        }

        // create RSE1 & RSE2 screen
        struct screen *screen2 = NULL;
        struct screen *screen3 = NULL;
        if(equal(&tmp_output, &raw_screen_output_setting[E_RAW_SCREEN_RSE]))
        {
            weston_log("RSE screen is found! i = %d(wayland number), output = %d, %d, %d, %d\n", i, tmp_output.x, tmp_output.y, tmp_output.width, tmp_output.height);
            // RSE1
            screen2 = malloc(sizeof(struct screen));
            screen2->_screen          = screens[i];
            screen2->interface        = interface;
            screen2->get_layer_ids    = get_layer_ids_2;
            screen2->get_output       = get_output_2;
            screen2->set_render_order = set_render_order_2;

            virtual_screen_set[virtual_screen_count] = screen2;
            ++virtual_screen_count;

            //RSE2
            screen3 = malloc(sizeof(struct screen));
            screen3->_screen          = screens[i];
            screen3->interface        = interface;
            screen3->get_layer_ids    = get_layer_ids_3;
            screen3->get_output       = get_output_3;
            screen3->set_render_order = set_render_order_3;

            virtual_screen_set[virtual_screen_count] = screen3;
            ++virtual_screen_count;
        }

        // create cluster screen
        struct screen *screen4 = NULL;
        if(equal(&tmp_output, &raw_screen_output_setting[E_RAW_SCREEN_CLUSTER]))
        {
            weston_log("cluster screen is found! i = %d(wayland number), output = %d, %d, %d, %d\n", i, tmp_output.x, tmp_output.y, tmp_output.width, tmp_output.height);
            screen4 = malloc(sizeof(struct screen));
            screen4->_screen          = screens[i];
            screen4->interface        = interface;
            screen4->get_layer_ids    = get_layer_ids_4;
            screen4->get_output       = get_output_4;
            screen4->set_render_order = set_render_order_4;

            virtual_screen_set[virtual_screen_count] = screen4;
            ++virtual_screen_count;
        }
    }

    *screen_array = virtual_screen_set;
    *n_screen = virtual_screen_count;

    free(screens);

    return true;
}

static struct screen* _get_virtual_screen(const enum E_SEAT v_screen)
{
    for(int i=0; i < virtual_screens_count; ++i)
    {
        struct screen_output tmp_output;
        virtual_screens[i]->get_output(virtual_screens[i], &tmp_output);

        if(equal(&tmp_output, &virtual_screen_output_setting[v_screen]))
            return virtual_screens[i];
    }

    return NULL;
}

struct screen* get_virtual_screen(const enum E_SEAT v_screen)
{
    // get vm/cluster screen
    if(E_SEAT_TEST==v_screen)
    {
        struct screen* cluster = _get_virtual_screen(4);
        if(cluster)
            return cluster;

        return _get_virtual_screen(v_screen);
    }

    // get ivi screen
    return _get_virtual_screen(v_screen);
}

struct screen* get_screen_vm_or_cluster()
{
    return get_virtual_screen(E_SEAT_TEST);
}

struct screen* get_screen_ivi()
{
    return get_virtual_screen(E_SEAT_IVI);
}

struct screen* get_screen_rse1()
{
    return get_virtual_screen(E_SEAT_RSE1);
}

struct screen* get_screen_rse2()
{
    return get_virtual_screen(E_SEAT_RSE2);
}

struct screen* get_screen(int screen_number)
{
    if(screen_number>=virtual_screens_count)
        return NULL;

    return virtual_screens[screen_number];
}

unsigned get_screen_count()
{
    return virtual_screens_count;
}

struct ivi_layout_screen* get_fixme_screen(const struct ivi_controller_interface *interface)
{
    int screen_count = 0;
    struct ivi_layout_screen** screens = NULL;
    int ret = interface->get_screens( &screen_count, &screens);
    if(ret!=IVI_SUCCEEDED)
        return NULL;

    if(screen_count>0)
    {
        struct ivi_layout_screen* fixme_layout_screen = interface->get_screen_from_id(screen_count-1);
        if(fixme_layout_screen)
        {
            for(int i = 0; i < get_screen_count(); ++i)
            {
                if(get_screen(i)->_screen==fixme_layout_screen)
                {
                    weston_log("fixme screen is %d (seat number)\n", i);
                    free(screens);
                    return fixme_layout_screen;
                }
            }
            weston_log("fixme screen is not a known screen!\n");
            free(screens);
            return fixme_layout_screen;
        }
        else
            weston_log("fixme screen is not found! id = %d\n", screen_count-1);
    }
    else
        weston_log("screen_count = %d", screen_count);

    free(screens);
    return NULL;
}

bool init_screen_api(const struct ivi_controller_interface *interface)
{
    weston_log( "init_screen_api() in\n" );
    memset(virtual_screens, '0', E_SEAT_MAX*sizeof(struct screen*));

    unsigned int n_screen = 0;
    struct screen** screen_array = NULL;
    bool rtv = get_screens(interface, &n_screen, &screen_array);

    unsigned int screen_count = n_screen>=E_SEAT_MAX?E_SEAT_MAX:n_screen;
    if(n_screen>E_SEAT_MAX)
        weston_log( "too many screens!\n" );

    for(int i = 0; i < screen_count; ++i)
        virtual_screens[i] = screen_array[i];

    virtual_screens_count = screen_count;

    free(screen_array);

    weston_log( "init_screen_api() out\n" );
    return rtv;
}

void destroy_screen_api()
{
    for(int i = 0; i < virtual_screens_count; ++i)
        free(virtual_screens[i]);

    n_screen2_layers = 0;
    if(screen2_layers)
        free(screen2_layers);

    n_screen3_layers = 0;
    if(screen3_layers)
        free(screen3_layers);

    virtual_screens_count = 0;
}
