/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DoubleTouchState.cpp
/// @brief contains DoubleTouchState
///
/// Created by zs on 2017/02/28.
///
/// this file contains the implementation of class DoubleTouchState
///

#include <chrono>

#include "GR/DoubleTapState.h"
#include "log.h"

static std::chrono::milliseconds timeout = std::chrono::milliseconds(2000);

std::shared_ptr<TouchState> DoubleTapState::GetInitState(std::function<void(void)> callback) const
{
    auto initState = std::make_shared<DoubleTapStateInit>();
    initState->RegisterCallback(callback);
    return initState;
}

std::shared_ptr<TouchState> DoubleTapStateInit::TouchDown(double x, double y)
{
    logDebugF("DoubleTouchStateInit::TouchDown()");

    TouchState::TouchDown(x, y);
    auto downState = std::make_shared<DoubleTapStateDown>();
    downState->Copy(this);
    return downState;
}

std::shared_ptr<TouchState> DoubleTapStateInit::TouchUp(double x, double y)
{
    logDebugF("DoubleTouchStateInit::TouchUp()");

    return GetInitState(GetCallback());
}

std::shared_ptr<TouchState> DoubleTapStateDown::TouchDown(double x, double y)
{
    logDebugF("DoubleTouchStateDown::TouchDown()");

    return GetInitState(GetCallback());
}

std::shared_ptr<TouchState> DoubleTapStateDown::TouchUp(double x, double y)
{
    logDebugF("DoubleTouchStateDown::TouchUp()");

    TouchState::TouchUp(x, y);
    auto singleTouchState = std::make_shared<DoubleTapStateSingleTouch>();
    singleTouchState->Copy(this);
    return singleTouchState;
}

std::shared_ptr<TouchState> DoubleTapStateSingleTouch::TouchDown(double x, double y)
{
    logDebugF("DoubleTouchStateSingleTouch::TouchDown()");

    TouchState::TouchDown(x, y);
    auto singleTouchDownState = std::make_shared<DoubleTapStateSingleTouchDown>();
    singleTouchDownState->Copy(this);
    return singleTouchDownState;
}

std::shared_ptr<TouchState> DoubleTapStateSingleTouch::TouchUp(double x, double y)
{
    logDebugF("DoubleTouchStateSingleTouch::TouchUp()");

    return GetInitState(GetCallback());
}

std::shared_ptr<TouchState> DoubleTapStateSingleTouchDown::TouchDown(double x, double y)
{
    logDebugF("DoubleTouchStateSingleTouchDown::TouchDown()");

    return GetInitState(GetCallback());
}

std::shared_ptr<TouchState> DoubleTapStateSingleTouchDown::TouchUp(double x, double y)
{
    logDebugF("DoubleTouchStateSingleTouchDown::TouchUp()");

    SendNotify();
    return GetInitState(GetCallback());
}
