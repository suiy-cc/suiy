/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HMICSpy.h
/// @brief contains 
/// 
/// Created by zs on 2018/05/15.
///
/// this file contains the definition of class HMICSpy
/// 

#ifndef AMA_APP_FRAMEWORK_HMICSPY_H
#define AMA_APP_FRAMEWORK_HMICSPY_H


#include <vector>
#include <ama_enum.h>

namespace HMICSpy {
// WARNING: never modify these values!!!
extern bool isScreenStatusReady;
extern std::vector<E_SEAT> screens;
};

#endif //AMA_APP_FRAMEWORK_HMICSPY_H
