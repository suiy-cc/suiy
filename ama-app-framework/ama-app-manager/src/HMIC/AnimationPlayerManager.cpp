/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AnimationPlayerManager.cpp
/// @brief contains class AnimationPlayerManager
/// 
/// Created by zs on 2017/04/13.
///
/// this file contains the implementation of class AnimationPlayerManager
///

#include "AnimationPlayer.h"
#include "AnimationPlayerManager.h"

AnimationPlayerManager::AnimationPlayerManager()
: switcher(nullptr)
{

}

AnimationPlayerManager::~AnimationPlayerManager()
{
    ;
}

void AnimationPlayerManager::ShowAP(AMPID proc)
{
    E_SEAT seat = GET_SEAT(proc);
    auto oldTop = zorder[seat].GetTopApp();
    zorder[seat].SetTopApp(proc);
    auto newTop = zorder[seat].GetTopApp();

    // if current process is not top app,
    // do nothing
    if(newTop==oldTop)
        return;

    // WARNING: don't save the switch when
    // the AP works at the same layer after
    // the switch. if the AP do give an
    // animation, new app would like to
    // show it from start. (current AP just
    // is a static picture)

    // switch AP from old process to new process
    SwitchAP(oldTop, newTop);
}

void AnimationPlayerManager::HideAP(AMPID proc)
{
    E_SEAT seat = GET_SEAT(proc);
    auto oldTop = zorder[seat].GetTopApp();
    zorder[seat].RemoveApp(proc);
    auto newTop = zorder[seat].GetTopApp();

    // if current process is not top app,
    // do nothing
    if(proc!=oldTop)
        return;

    // switch AP from old process to new process
    SwitchAP(oldTop, newTop);
}

void AnimationPlayerManager::SetSwitcher(APSwitcher apSwitcher)
{
    switcher = apSwitcher;
}

void AnimationPlayerManager::SwitchAP(AMPID from, AMPID to)
{
    if(switcher)
        switcher(from, to);
}
