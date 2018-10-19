/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ama_types.cpp
/// @brief contains
///
/// Created by zs on 2018/01/04.
///

#include "ama_types.h"

std::map<std::string, E_DEBUG_CMD> DebugCmdMap = {
    {"showConnections",         E_SHOW_CONNECTIONS},
    {"showAMSources",           E_SHOW_AMSOURCES},
    {"showAMSinks",             E_SHOW_AMSINKS},
    {"showConRecord",           E_SHOW_CONRECORD},
    {"showRoutingMap",          E_SHOW_ROUTINGMAP},
    {"showFocusSta",            E_SHOW_FOCUSSTA},
    {"showAudioSetting",        E_SHOW_AUDIOSETTING},
    {"setIVIUseSpeaker",        E_SET_SEAT_IVI_USE_SPEAKER},
    {"setLRSEUseSpeaker",        E_SET_SEAT_LRSE_USE_SPEAKER},
    {"setRRSEUseSpeaker",        E_SET_SEAT_RRSE_USE_SPEAKER},
    {"setMaskSurfaceVisible",    E_SET_MASK_SURFACE_VISIBLE},
    {"setMaskSurfaceInvisible",  E_SET_MASK_SURFACE_INVISIBLE},
    {"restartHMIC",             E_HMIC_RESTART},
};

const std::map<E_SEAT, std::string> ScreenName = {
    {E_SEAT_TEST, "E_SEAT_TEST"},
    {E_SEAT_IVI, "E_SEAT_IVI"},
    {E_SEAT_RSE1, "E_SEAT_RSE1"},
    {E_SEAT_RSE2, "E_SEAT_RSE2"},
    //{E_SEAT_SHARE, "E_SEAT_SHARE"},

    {E_SEAT_MAX, "E_SEAT_MAX"},
    {E_SEAT_RSE, "E_SEAT_RSE"},

    {E_SEAT_INVALID, "E_SEAT_INVALID"},

    {E_SEAT_NONE, "E_SEAT_NONE"},
};

const std::map<E_SEAT, std::string> ScreenIDStr = {
    {E_SEAT_TEST,   "scr0"},
    {E_SEAT_IVI,    "scr1"},
    {E_SEAT_RSE1,   "scr2"},
    {E_SEAT_RSE2,   "scr3"},
    //{E_SEAT_SHARE,  "scr4"},

    {E_SEAT_NONE,   "service"},
};

const std::map<std::string, E_SEAT> ScreenID2Seat = {
    {"scr0",    E_SEAT_TEST},
    {"scr1",    E_SEAT_IVI},
    {"scr2",    E_SEAT_RSE1},
    {"scr3",    E_SEAT_RSE2},
    //{"scr4",    E_SEAT_SHARE},

    {"service", E_SEAT_NONE},
};
