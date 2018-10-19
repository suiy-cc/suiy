/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file main.c
/// @brief contains
///
/// Created by zs on 2017/12/26.
///

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "LayerID.h"
#include "ama_enum.h"

struct ivi_layer_setup
{
    uint32_t layer_id;
    int32_t visibility;
    float opacity;
};

struct ivi_scene_description
{
    uint32_t version;
    uint32_t num_layers;
    struct ivi_layer_setup *layers;
};

struct ivi_scene_description *ivi_scene_provider_get_description(void)
{
    unsigned int cluster_layer_count = 0;
    unsigned int* cluster_layers = NULL;
    {
        void *handle = dlopen("/usr/lib/cluster-control-preload.so", RTLD_LAZY|RTLD_LOCAL);
        if(handle)
        {
            typedef void (*cluster_layer_type)(unsigned int **, unsigned int *);
            cluster_layer_type get_cluster_layers = (cluster_layer_type)dlsym(handle, "get_cluster_layers");
            if(get_cluster_layers)
            {
                get_cluster_layers(&cluster_layers, &cluster_layer_count);
            }
            else
                printf( "expected function \"void get_cluster_layers(unsigned int **, unsigned int *)\" is missing!\n" );
            dlclose(handle);
        }
        else
            printf( "cluster-contorl-preload.so is missing!\n" );
    }

    struct ivi_scene_description * scene_description = (struct ivi_scene_description *)malloc(sizeof(struct ivi_scene_description));
    scene_description->version = 2;
    scene_description->num_layers = 0;
    scene_description->layers = malloc(sizeof(struct ivi_layer_setup)*E_LAYER_MAX*3+cluster_layer_count); // IVI+RSE1+RSE2+cluster_share

    for(int i = E_SEAT_IVI; i < E_SEAT_MAX; ++i)
    {
        unsigned int layer_count = 0;
        unsigned int *layers = NULL;
        get_layer_ids_of_ivi(i, &layers, &layer_count);
        for(int j = 0; j < layer_count; ++j)
        {
            scene_description->layers[scene_description->num_layers+j].visibility = 1;
            scene_description->layers[scene_description->num_layers+j].layer_id = layers[j];
            scene_description->layers[scene_description->num_layers+j].opacity = 1.0f;
        }
        scene_description->num_layers += layer_count;
        free(layers);
    }

    if(cluster_layers && cluster_layer_count>0)
    {
        for(int i = 0; i < cluster_layer_count; ++i)
        {
            scene_description->layers[scene_description->num_layers+i].visibility = 1;
            scene_description->layers[scene_description->num_layers+i].layer_id = cluster_layers[i];
            scene_description->layers[scene_description->num_layers+i].opacity = 1.0f;
        }
        scene_description->num_layers += cluster_layer_count;
        free(cluster_layers);
    }

    return scene_description;
}

/* IVI_SCENE_DESC_VERSION 2 */
struct ivi_render_order
{
    uint32_t length;
    uint32_t *render_order;
};

struct ivi_render_order *ivi_scene_provider_get_screen_render_order(char const *output_model)
{
    struct ivi_render_order * render_order = (struct ivi_render_order *)malloc(sizeof(struct ivi_render_order));
    memset(render_order, 0, sizeof(struct ivi_render_order));

    static const char ivi[] = "DP-3";
    //static const char rse[] = "DP-1"; // the whole RSE
    static const char rse1[] = "left";
    static const char rse2[] = "right";
    static const char share[] = "one"; // ivi->cluster share
    if(strcmp(output_model, ivi)==0)
    {
        get_layer_ids_of_ivi(1, &render_order->render_order, &render_order->length);
    }

    if(strcmp(output_model, rse1)==0)
    {
        get_layer_ids_of_ivi(2, &render_order->render_order, &render_order->length);
    }

    if(strcmp(output_model, rse2)==0)
    {
        get_layer_ids_of_ivi(3, &render_order->render_order, &render_order->length);
    }

//    if(strcmp(output_model, rse)==0)
//    {
//        render_order->length = 0;
//        render_order->render_order = malloc(sizeof(unsigned int)*E_LAYER_MAX*2);
//
//        // get rse1
//        unsigned int layer_count = 0;
//        unsigned int *layers = NULL;
//        get_layer_ids_of_ivi(2, &layers, &layer_count);
//        for(int i = 0; i < layer_count; ++i)
//            render_order->render_order[i+render_order->length] = layers[i];
//        render_order->length += layer_count;
//        free(layers);
//
//        // get rse2
//        layer_count = 0;
//        layers = NULL;
//        get_layer_ids_of_ivi(3, &layers, &layer_count);
//        for(int i = 0; i < layer_count; ++i)
//            render_order->render_order[i+render_order->length] = layers[i];
//        render_order->length += layer_count;
//        free(layers);
//    }

    if(strcmp(output_model, share)==0)
    {
        void *handle = dlopen("/usr/lib/cluster-control-preload.so", RTLD_LAZY|RTLD_LOCAL);
        if(handle)
        {
            typedef void (*cluster_layer_type)(unsigned int **, unsigned int *);
            cluster_layer_type get_cluster_layers = (cluster_layer_type)dlsym(handle, "get_cluster_layers");
            if(get_cluster_layers)
            {
                get_cluster_layers(&render_order->render_order, &render_order->length);
                dlclose(handle);
                return;
            }
            printf( "expected function \"void get_cluster_layers(unsigned int **, unsigned int *)\" is missing!\n" );
            dlclose(handle);
        }
        printf( "cluster-contorl-preload.so is missing!\n" );
    }

//    FILE* output = fopen("/tmp/output_model.txt", "a");
//    fprintf(output, "%s\n", output_model);
//    fclose(output);

    return render_order;
}
