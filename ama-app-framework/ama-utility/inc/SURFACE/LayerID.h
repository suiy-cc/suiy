/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LayerID.h
/// @brief contains 
/// 
/// Created by zs on 2017/12/27.
///

/// this file contains the definition of class LayerID
/// 

#ifndef LAYERID_H
#define LAYERID_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void get_layer_ids_of_ivi(unsigned int screen_id, unsigned int **layer_id_array, unsigned int *layer_count);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // LAYERID_H
