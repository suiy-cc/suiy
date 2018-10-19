/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchStateProxyTimeout.cpp
/// @brief contains 
/// 
/// Created by dev on 2017/07/19.
///

/// this file contains the implementation of class TouchStateProxyTimeout
///

#include "GR/TouchStateProxyTimeout.h"

TouchStateProxyTimeout::TouchStateProxyTimeout(std::shared_ptr<TouchState> touchState, const std::chrono::milliseconds &timeout)
: TouchStateProxyBase(touchState)
, timeout(timeout)
{
}

std::shared_ptr<TouchState> TouchStateProxyTimeout::TouchDown(double x, double y)
{
    // reset state and reif timeout
    if(IsTimeout())
    {
        auto state = GetInitState(GetRawTouchState()->GetCallback());
        state->Copy(this);
        return state->TouchDown(x, y);
    }
    else
        return TouchStateProxyBase::TouchDown(x, y);
}

std::shared_ptr<TouchState> TouchStateProxyTimeout::TouchUp(double x, double y)
{
    if(IsTimeout())
    {
        auto state = GetInitState(GetRawTouchState()->GetCallback());
        state->Copy(this);
        return state->TouchUp(x, y);
    }
    else
        return TouchStateProxyBase::TouchUp(x, y);
}

std::shared_ptr<TouchState> TouchStateProxyTimeout::TouchMove(double x, double y)
{
    if(IsTimeout())
    {
        auto state = GetInitState(GetRawTouchState()->GetCallback());
        state->Copy(this);
        return state->TouchMove(x, y);
    }
    else
        return TouchStateProxyBase::TouchMove(x, y);
}

void TouchStateProxyTimeout::Copy(TouchState *from)
{
    // copy raw state
    TouchStateProxyBase::Copy(from);

    // copy current object
    auto timeoutProxy = dynamic_cast<TouchStateProxyTimeout*>(from);
    if(timeoutProxy)
        this->timeout = timeoutProxy->timeout;
    else
        throw std::exception();
}

bool TouchStateProxyTimeout::IsTimeout()
{
    // reset timer for initial state
    if(typeid(*(GetRawTouchState().get())) == typeid(*(GetRawTouchState()->GetInitState(nullptr).get())))
    {
        timeout.StartTiming();
        return false;
    }
    else
    {
        return timeout.IsTimeout();
    }
}

std::shared_ptr<TouchStateProxyBase> TouchStateProxyTimeout::GetNewInstance(const std::shared_ptr<TouchState>& state)const
{
    return std::make_shared<TouchStateProxyTimeout>(state, Timeout().GetTimeout());
}
