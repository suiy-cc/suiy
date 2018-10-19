/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchStateProxyBase.cpp
/// @brief contains 
/// 
/// Created by dev on 2017/07/19.
///

/// this file contains the implementation of class TouchStateProxyBase
///

#include <memory>

#include "GR/TouchStateProxyBase.h"
TouchStateProxyBase::TouchStateProxyBase(std::shared_ptr<TouchState> touchState)
: rawTouchState(touchState)
{}

TouchStateProxyBase::~TouchStateProxyBase()
{

}

std::shared_ptr<TouchState> TouchStateProxyBase::TouchDown(double x, double y)
{
    rawTouchState = rawTouchState->TouchDown(x, y);
    return shared_from_this();
}

std::shared_ptr<TouchState> TouchStateProxyBase::TouchUp(double x, double y)
{
    rawTouchState = rawTouchState->TouchUp(x, y);
    return shared_from_this();
}

std::shared_ptr<TouchState> TouchStateProxyBase::TouchMove(double x, double y)
{
    rawTouchState = rawTouchState->TouchMove(x, y);
    return shared_from_this();
}

void TouchStateProxyBase::GetLastPosition(double &ax, double &ay) const
{
    rawTouchState->GetLastPosition(ax, ay);
}

void TouchStateProxyBase::RegisterCallback(std::function<void(void)> notify)
{
    rawTouchState->RegisterCallback(notify);
}

std::function<void()> TouchStateProxyBase::GetCallback() const
{
    return rawTouchState->GetCallback();
}

void TouchStateProxyBase::Copy(TouchState *from)
{
    auto baseFrom = dynamic_cast<TouchStateProxyBase*>(from);
    if(baseFrom)
    {
        // onion-peeling
        this->rawTouchState->Copy(baseFrom->GetRawTouchState().get());
    }
    else
        rawTouchState->Copy(from);
}

std::shared_ptr<TouchState> TouchStateProxyBase::GetInitState(std::function<void(void)> callback) const
{
    auto rawInitState = rawTouchState->GetInitState(callback);
    auto initState = GetNewInstance(rawInitState);
    return initState;
}

std::shared_ptr<TouchState> TouchStateProxyBase::GetRawTouchState() const
{
    return rawTouchState;
}

void TouchStateProxyBase::SetRawTouchState(const std::shared_ptr<TouchState>& state)
{
    rawTouchState = state;
}
