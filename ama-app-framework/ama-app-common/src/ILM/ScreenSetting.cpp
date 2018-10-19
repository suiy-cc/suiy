/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ScreenSetting.cpp
/// @brief contains
///
/// Created by zs on 2017/03/13.
///

#include "ILM/ScreenSetting.h"

std::map<E_SEAT, Section> ScreenSetting = {
#ifndef CREATE_LAYERS_BY_AMGR
    {E_SEAT_TEST, Section{0, 0, 1280, 720}},
#ifdef CODE_FOR_HS5
    {E_SEAT_IVI, Section{1920, 0, 1920, 720}},
#else // CODE_FOR_HS5
    {E_SEAT_IVI, Section{1920, 0, 1280, 720}},
#endif // CODE_FOR_HS5
#ifdef WITH_IAS_BACKEND
    {E_SEAT_RSE1, Section{3200, 0, 1280, 720}},
    {E_SEAT_RSE2, Section{4480, 0, 1280, 720}},
#else
    {E_SEAT_RSE1, Section{640, 720, 1280, 720}},
    {E_SEAT_RSE2, Section{1920, 720, 1280, 720}},
#endif
    //{E_SEAT_SHARE, Section{3200, 0, 1920, 720}},
#else // CREATE_LAYERS_BY_AMGR
    {E_SEAT_IVI, Section{0, 0, 1280, 720}},
    {E_SEAT_RSE1, Section{1280, 0, 1280, 720}},
    {E_SEAT_RSE2, Section{2560, 0, 1280, 720}},
#endif // CREATE_LAYERS_BY_AMGR
};

Section ScreenSettingRSE = Section{
    ScreenSetting[E_SEAT_RSE1].left,
    ScreenSetting[E_SEAT_RSE1].top,
    ScreenSetting[E_SEAT_RSE1].width + ScreenSetting[E_SEAT_RSE2].width,
    ScreenSetting[E_SEAT_RSE1].height,
};
