/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AnimationPlayerAPI.h
/// @brief contains APIs of animation-player
///
/// Created by zs on 2016/9/8.
/// this file contains the declaration of animation-player's APIs
///

#ifndef ANIMATIONPLAYERAPI_H
#define ANIMATIONPLAYERAPI_H

typedef void (*PlayAnimationNotifyCB)(unsigned int);
typedef void (*StopAnimationNotifyCB)(unsigned int);

void ListenToPlayAnimationNotify(PlayAnimationNotifyCB f);
void ListenToStopAnimationNotify(StopAnimationNotifyCB f);

// used to tell app-manager that animation is over
void StopAnimationReq(unsigned int app);

// used to tell app-manager that stop animation is success
void StopSuccess(unsigned int app);

#endif // ANIMATIONPLAYERAPI_H
