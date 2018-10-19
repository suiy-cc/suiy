/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ID.h
/// @brief contains
///
/// Created by zs on 2017/05/23.
///

#ifndef ID_H
#define ID_H

#include <string>

#include "ama_enum.h"

extern std::string GET_APPLOGID_FROM_AMPID(AMPID ampid);

extern E_APPID E_APPID_HOME;
extern E_APPID E_APPID_TITLEBAR;
extern E_APPID E_APPID_POPUP;
extern E_APPID E_APPID_MUSIC;
extern E_APPID E_APPID_VIDEO;
extern E_APPID E_APPID_IMAGE;
extern E_APPID E_APPID_RADIO;
extern E_APPID E_APPID_IME;
extern E_APPID E_APPID_BKG;
extern E_APPID E_APPID_AP;
extern E_APPID E_APPID_SYNC_APP;
extern E_APPID E_APPID_TOUCH_FORBIDDER;
extern E_APPID E_APPID_GR;
extern E_APPID E_APPID_MASK_APPS;
extern E_APPID E_APPID_CHILD_LOCK;
E_APPID GetAppID(const std::string& appid);

E_LAYER GetLayer(E_APPID appid);

// home app's AMPID
#define HOME_AMPID(seat) AMPID_APP(seat,E_APPID_HOME)

// animation player's AMPID
#define AP_AMPID(seat) AMPID_SERVICE(seat,E_APPID_AP)

// AMGR's surface ID
#define SURFACEID_AMGR(seatID, layer, appid) SURFACEID(AMPID_AMGR(seatID, appid), LAYERID(seatID, layer))

// touch forbidder's surface ID
#define SURFACEID_TF(seatID, layer) SURFACEID_AMGR(seatID, layer, E_APPID_TOUCH_FORBIDDER)

// child lock's surface ID
#define SURFACEID_CL(seatID) SURFACEID(AMPID_SERVICE(seatID, E_APPID_CHILD_LOCK), LAYERID(seatID, GetLayer(E_APPID_CHILD_LOCK)))

// background's surface ID
#define SURFACEID_BKG(seatID) SURFACEID_AMGR(seatID, GetLayer(E_APPID_BKG), E_APPID_BKG)

// gesture recognizer's surface ID
#define SURFACEID_GR(seatID) SURFACEID_AMGR(seatID, GetLayer(E_APPID_GR), E_APPID_GR)

// the mask surface ID let all apps invisible
#ifdef CODE_FOR_AIO
#define E_LAYER_APP_MASK E_LAYER_APP_MASK_AIO
#else // CODE_FOR_AIO
#define E_LAYER_APP_MASK E_LAYER_APP_MASK_HYPERVISOR
#endif // CODE_FOR_AIO
#define SURFACEID_MASK(seatID) SURFACEID_AMGR(seatID, E_LAYER_APP_MASK, E_APPID_MASK_APPS)

// animation player's surface ID
#define SURFACEID_AP_AMGR(seatID) SURFACEID_AMGR(seatID, GetLayer(E_APPID_AP), E_APPID_AP)

extern unsigned int GetSurfaceID(AMPID app);

std::string GetAppIDStr(const std::string& itemID);

unsigned int GetScreenID(const std::string& itemID);

#endif // ID_H
