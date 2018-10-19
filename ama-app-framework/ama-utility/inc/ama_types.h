/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
//#pragma once
#ifndef __AMA_TYPES_H__
#define __AMA_TYPES_H__

#include <string>
#include <map>

#include "ama_enum.h"
#include "ama_powerTypes.h"

typedef uint8_t ama_seatID_t;
typedef uint8_t ama_appID_t;

// AMPID
#define AMPID_CUSTOM(seatID, appType, appID) (static_cast<AMPID>(((seatID)<<(3*4)) | ((appType)<<(2*4)) | (appID)))
#define AMPID_APP(seatID, appID) 			AMPID_CUSTOM((seatID), E_APPTYPE_APP, appID)
#define AMPID_SERVICE(seatID, appID)		AMPID_CUSTOM((seatID), E_APPTYPE_SERVICE, appID)
#define AMPID_AMGR(seatID, appID)	    	AMPID_CUSTOM((seatID), E_APPTYPE_AMGR, appID)
#define AMPID_ANMS(seatID, appID)	    	AMPID_CUSTOM((seatID), E_APPTYPE_ANONYMOUS, appID)
#define GET_SEAT(ampid)                     (static_cast<E_SEAT>((ampid) >> 3*4))
#define GET_APPID(ampid)                    (static_cast<E_APPID>((ampid) & 0x000000FF))
#define IS_APP(ampid)                       (static_cast<bool>(((ampid) & 0x0F00)==0x0000))
#define IS_SERVICE(ampid)                   (static_cast<bool>(((ampid) & 0x0F00)==0x0100))
#define IS_AMGR(ampid)                      (static_cast<bool>(((ampid) & 0x0F00)==0x0200))
#define IS_ANONYMOUS(ampid)					(static_cast<bool>(((ampid) & 0x0F00)==0x0F00))
#define CHANGE_SEAT(ampid, seatID)          (static_cast<AMPID>(((ampid) & 0x0FFF) | ((seatID)<<(3*4))))

// layer ID
#define LAYERID(seatID, layer)				(((seatID)<<(7*4)) | ((layer)<<(2*4)))
#define GET_LAYERID(surfaceID)              (static_cast<unsigned int>((surfaceID) & 0xF000FF00))
#define GET_LAYER(layerID)					(static_cast<E_LAYER>(((layerID) & 0x0000FF00)>>(2*4)))

// surface ID
#define SURFACEID(ampid, layerID)			(static_cast<unsigned int>(((ampid)<<(4*4)) | (layerID)))
#define GET_AMPID(surfaceID)                (static_cast<AMPID>(((surfaceID) & 0xFFFF0000) >> 4*4))

// app's status
#define SET_AVAILABLE(appState, availability) (appState = static_cast<AppStatus>((((appState)&0xFFFFFFFE)|((availability)&0x1))))
#define SET_VISIBLE(appState, visibility) (appState = static_cast<AppStatus>((((appState)&0xFFFFFFFD)|((visibility)<<1))))
#define IS_AVAILABLE(appState) ((appState)&0x0001)
#define IS_VISIBLE(appState) ((appState)&0x0002)

extern std::map<std::string, E_DEBUG_CMD> DebugCmdMap;

extern const std::map<E_SEAT, std::string> ScreenName;

extern const std::map<E_SEAT, std::string> ScreenIDStr;

extern const std::map<std::string, E_SEAT> ScreenID2Seat;

#define BROADCASTFLAG                   (0xffff)

#endif /* __AMA_TYPES_H__ */
