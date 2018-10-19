/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AnimationPlayer.h
/// @brief contains class AnimationPlayer
/// 
/// Created by zs on 2017/04/12.
///
/// this file contains the definition of class AnimationPlayer
/// 

#ifndef ANIMATIONPLAYER_H
#define ANIMATIONPLAYER_H

#include "ama_types.h"

class AnimationPlayer
{
public:
    AnimationPlayer(AMPID proc);
    virtual ~AnimationPlayer();

    void SetAMPID(AMPID proc);

    void Show();
    void Hide();

    bool IsReady();
private:
    AMPID proc;
    bool isVisible;
};


#endif // ANIMATIONPLAYER_H
