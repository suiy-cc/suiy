/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchState.cpp
/// @brief contains class TouchState
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the implementation of class TouchState
///

#include "GR/TouchState.h"

TouchState::TouchState()
: x(0)
, y(0)
, notify(nullptr)
{
    ;
}

TouchState::~TouchState()
{
    ;
}

std::shared_ptr<TouchState> TouchState::TouchDown(double x, double y)
{
    this->x = x;
    this->y = y;
    return shared_from_this();
}

std::shared_ptr<TouchState> TouchState::TouchUp(double x, double y)
{
    this->x = x;
    this->y = y;
    return shared_from_this();
}

std::shared_ptr<TouchState> TouchState::TouchMove(double x, double y)
{
    this->x = x;
    this->y = y;
    return shared_from_this();
}

void TouchState::GetLastPosition(double& ax, double& ay)const
{
    ax = x;
    ay = y;
}

void TouchState::Copy(TouchState *from)
{
    if(from)
    {
        x = from->x;
        y = from->y;
        notify = from->notify;
    }
}
void TouchState::SendNotify()
{
    if(notify)
        notify();
}

