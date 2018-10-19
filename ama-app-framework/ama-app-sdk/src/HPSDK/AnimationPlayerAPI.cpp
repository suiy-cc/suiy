/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AnimationPlayerAPI.cpp
/// @brief contains APIs of animation-player
///
/// Created by zs on 2016/9/8.
/// this file contains the definition of animation-player's APIs
///

#include "AnimationPlayerAPI.h"
#include "AppManager.h"

static PlayAnimationNotifyCB PlayAnimation = nullptr;

void ListenToPlayAnimationNotify(PlayAnimationNotifyCB f)
{
    AppManager::GetInstance()->SetPlayAnimationNotifyCB(f);
}

void ListenToStopAnimationNotify(StopAnimationNotifyCB f)
{
    AppManager::GetInstance()->SetStopAnimationNotifyCB(f);
}

void StopAnimationReq(unsigned int app)
{
    AppManager::GetInstance()->StopAnimation(app);
}

void StopSuccess(unsigned int app)
{
    AppManager::GetInstance()->StopSuccess(app, 0);
}