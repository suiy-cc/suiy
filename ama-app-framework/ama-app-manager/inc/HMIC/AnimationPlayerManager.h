/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AnimationPlayerManager.h
/// @brief contains class AnimationPlayerManager
/// 
/// Created by zs on 2017/04/13.
///
/// this file contains the definition of class AnimationPlayerManager
/// 

#ifndef ANIMATIONPLAYERMANAGER_H
#define ANIMATIONPLAYERMANAGER_H

#include <functional>

#include "ama_types.h"
#include "ZOrderWithAPPPriority.h"

typedef std::map<E_SEAT, ZOrderWithAPPPriority> ZOrderMap;
typedef std::function<void(AMPID, AMPID)>       APSwitcher;

class AnimationPlayerManager
{
public:
    AnimationPlayerManager();
    virtual ~AnimationPlayerManager();

    void ShowAP(AMPID proc);
    void HideAP(AMPID proc);

    void SetSwitcher(APSwitcher apSwitcher);

protected:
    void SwitchAP(AMPID from, AMPID to);

private:
    ZOrderMap  zorder;
    APSwitcher switcher;
};


#endif // ANIMATIONPLAYERMANAGER_H
