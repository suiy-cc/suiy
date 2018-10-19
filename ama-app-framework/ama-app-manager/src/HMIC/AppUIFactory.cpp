/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUIFactory.cpp
/// @brief AppUIFactory.cpp
///
/// Created by zs on 2016/8/23.
/// this file contains the definition of AppUIHMI classes' factory function
///

#include "AMGRLogContext.h"
#include "AppUIFactory.h"
#include "AppUIHMI.h"
#include "VideoLikeUI.h"
#include "TitlebarUI.h"
#include "SyncUI.h"
#include "IMEUI.h"
#include "ID.h"

shared_ptr<AppUI> GetUIObject(AMPID app)
{
    DomainVerboseLog chatter(HMIC, formatText(__STR_FUNCTION__+"(%#x)", app));

    E_APPID appID = GET_APPID(app);
    SurfaceID surfaceID = GetSurfaceID(app);
    Surface surface(surfaceID);

    shared_ptr<AppUI> UI = nullptr;
    if(appID==E_APPID_TITLEBAR)
        UI = std::make_shared<TitlebarUI>(surface);
    else if(appID==E_APPID_SYNC_APP)
        UI = std::make_shared<SyncUI>(surface);
    else if(appID==E_APPID_IME)
        UI = std::make_shared<IMEUI>(surface);
    else
        UI = std::make_shared<VideoLikeUI>(surface);

    return UI;
}