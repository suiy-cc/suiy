/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LayerID.cpp
/// @brief contains
///
/// Created by zs on 2017/12/27.
///

/// this file contains the implementation of class LayerID
///

#include <stdlib.h>

#include "LayerID.h"
#include "ama_enum.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

#define LAYERID(seatID, layer)				(((seatID)<<(7*4)) | ((layer)<<(2*4)))

void get_layer_ids_of_ivi(unsigned int screen_id, unsigned int **layer_id_array, unsigned int *layer_count)
{
    static const enum E_LAYER layer_array[] = {
        E_LAYER_RELEASE_PLAY,
        E_LAYER_RELEASE_HMI,
        E_LAYER_BACKGROUND,
        E_LAYER_VIDEOPLAY,
        E_LAYER_NORMALAPP,
        E_LAYER_IME0,
        E_LAYER_ANIMATION,
        E_LAYER_TOUCH_FORBIDDEN_NORMALAPP,
        E_LAYER_UNUSED1,
        E_LAYER_UNUSED5,
        E_LAYER_UNUSED6,
        E_LAYER_TITLEBAR,
        E_LAYER_IME2,
        E_LAYER_UNUSED7,
        E_LAYER_NOTIFICATION,
        E_LAYER_IME3,
        E_LAYER_APP_MASK_AIO,
        E_LAYER_CAMERABKG,
        E_LAYER_BOOT_IMAGE_BKG,
        E_LAYER_BOOT_IMAGE,
        E_LAYER_UNUSED12,
        E_LAYER_UNUSED13,
        E_LAYER_BOOT_ANIMATION,
        E_LAYER_UNUSED15,
        E_LAYER_UNUSED16,
        E_LAYER_UNUSED17,
        E_LAYER_CAMERAVIDEO,
        E_LAYER_UNUSED18,
        E_LAYER_UNUSED19,
        E_LAYER_UNUSED1A,
        E_LAYER_UNUSED20,
        E_LAYER_UNUSED21,
        E_LAYER_UNUSED22,
        E_LAYER_CAMERAGUIDLINE,
        E_LAYER_UNUSED23,
        E_LAYER_UNUSED24,
        E_LAYER_UNUSED25,
        E_LAYER_UNUSED26,
        E_LAYER_CAMERAHMI,
        E_LAYER_UNUSED27,
        E_LAYER_UNUSED28,
        E_LAYER_E_CALL,
        E_LAYER_IME4,
        E_LAYER_APP_MASK_HYPERVISOR,
        E_LAYER_TOUCH_FORBIDDEN_CHILD_SAFETY_LOCK,
        E_LAYER_UNUSED32,
        E_LAYER_TOUCH_FORBIDDEN_EVERYTHING,
        E_LAYER_GR,
    };

    *layer_count = sizeof(layer_array)/sizeof(enum E_LAYER);
    *layer_id_array = ama_malloc((*layer_count)*sizeof(unsigned int*));

    for(int i = 0; i < *layer_count; ++i)
        (*layer_id_array)[i] = LAYERID(screen_id, layer_array[i]);
}
