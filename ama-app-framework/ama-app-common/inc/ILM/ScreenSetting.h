/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ScreenSetting.h
/// @brief contains screen information
/// 
/// Created by zs on 2017/03/13.
///

#ifndef SCREENSETTING_H
#define SCREENSETTING_H

#include <map>

#include <ama_types.h>
#include <ILM/HMIType.h>

extern std::map<E_SEAT, Section> ScreenSetting;
extern Section ScreenSettingRSE;

#endif // SCREENSETTING_H
