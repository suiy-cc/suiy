/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TapState.cpp
/// @brief contains class TapState
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the implementation of class TapState
///

#include "GR/TapState.h"

std::shared_ptr<TouchState> TapState::GetInitState(std::function<void(void)> callback) const
{
    auto initState = std::make_shared<TapStateInit>();
    initState->RegisterCallback(callback);
    return initState;
}

std::shared_ptr<TouchState> TapStateInit::TouchDown(double x, double y)
{
    TouchState::TouchDown(x, y);

    auto downState = std::make_shared<TapStateDown>();
    downState->Copy(this);

    return downState;
}

std::shared_ptr<TouchState> TapStateInit::TouchUp(double x, double y)
{
    return GetInitState(GetCallback());
}

std::shared_ptr<TouchState> TapStateDown::TouchDown(double x, double y)
{
    return GetInitState(GetCallback());
}

std::shared_ptr<TouchState> TapStateDown::TouchUp(double x, double y)
{
    SendNotify();
    
    return GetInitState(GetCallback());
}
