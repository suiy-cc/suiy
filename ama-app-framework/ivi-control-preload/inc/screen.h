/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
//
// Created by zs on 17-1-9.
//

#ifndef SCREEN_H
#define SCREEN_H

#include "ivi-layout-export.h"
#include "ama_enum.h"

struct screen_output{
    int x;
    int y;
    int width;
    int height;
};

struct screen{
    const struct ivi_controller_interface *interface;
    struct ivi_layout_screen* _screen;

    void (*get_layer_ids)(struct screen* self, unsigned int **layer_id_array, unsigned int* layer_count);
    void (*get_output)(struct screen* self, struct screen_output*);
    void (*set_render_order)(struct screen* self, struct ivi_layout_layer**, int n_layer);
};

struct screen* get_virtual_screen(const enum E_SEAT v_screen);
struct screen* get_screen_vm_or_cluster();
struct screen* get_screen_ivi();
struct screen* get_screen_rse1();
struct screen* get_screen_rse2();

// screen enumerator
struct screen* get_screen(int screen_number);
unsigned get_screen_count();

// fixme screen
struct ivi_layout_screen* get_fixme_screen(const struct ivi_controller_interface *interface);

// intializer
bool init_screen_api(const struct ivi_controller_interface *interface);
void destroy_screen_api();

#endif // SCREEN_H
