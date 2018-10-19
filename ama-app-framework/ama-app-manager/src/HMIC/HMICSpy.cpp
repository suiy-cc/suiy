/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HMICSpy.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/05/15.
///

/// this file contains the implementation of class HMICSpy
///

#include "HMICSpy.h"

namespace HMICSpy {
bool isScreenStatusReady = false;
std::vector<E_SEAT> screens;
}