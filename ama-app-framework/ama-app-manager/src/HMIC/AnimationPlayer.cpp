/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AnimationPlayer.cpp
/// @brief contains class AnimationPlayer
///
/// Created by zs on 2017/04/12.
///
/// this file contains the implementation of class AnimationPlayer
///

#include "AnimationPlayer.h"
#include "HMICTask.h"
#include "ProcessInfo.h"
#include "Configuration.h"

AnimationPlayer::AnimationPlayer(AMPID proc)
    : proc(proc)
    , isVisible(false)
{}

AnimationPlayer::~AnimationPlayer()
{
    DomainVerboseLog chatter(HMIC, "AnimationPlayer::~AnimationPlayer()");

    if(isVisible)
        Hide();
}

void AnimationPlayer::SetAMPID(AMPID proc)
{
    this->proc = proc;
}

void AnimationPlayer::Show()
{
    DomainVerboseLog chatter(HMIC, formatText("AnimationPlayer::Show(%#x)", proc));

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_ANIMATION_STARTUP_PLAY);
    hmicTask->SetAMPID(proc);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    isVisible = true;
}

void AnimationPlayer::Hide()
{
    DomainVerboseLog chatter(HMIC, formatText("AnimationPlayer::Hide(%#x)", proc));

    auto hmicTask = make_shared<HMICTask>();
    hmicTask->SetType(HMIC_NOTIFY_ANIMATION_STARTUP_STOP);
    hmicTask->SetAMPID(proc);
    TaskDispatcher::GetInstance()->Dispatch(hmicTask);

    isVisible = false;
}

bool AnimationPlayer::IsReady()
{
    if(IsSysUIMemOptmz())
        return true;
    else
    {
        // get animation-player's state
        E_SEAT seat            = GET_SEAT(proc);
        AMPID  animationPlayer = AP_AMPID(seat);
        auto   procInfo        = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto animationPlayerObj = procInfo->FindProc(animationPlayer);
        procInfo->Unlock();

        if(animationPlayerObj){
            return animationPlayerObj && animationPlayerObj->IsOperating();
        }else{
            return false;
        }

    }
}
