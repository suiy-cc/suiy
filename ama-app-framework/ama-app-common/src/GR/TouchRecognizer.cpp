/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchRecognizer.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/03/01.
///

/// this file contains the implementation of class TouchRecognizer
///

#include "GR/TouchRecognizer.h"
#include "GR/TouchState.h"

TouchRecognizer::TouchRecognizer()
: handlers()
, state()
{}

TouchRecognizer::TouchRecognizer(const std::shared_ptr<TouchState> state)
    : handlers()
    , state(state)
{
    if(this->state)
        this->state->RegisterCallback([=](){
            Triggered();
        });
}

TouchRecognizer::~TouchRecognizer()
{

}

void TouchRecognizer::TouchDown(double x, double y)
{
    if(state)
    {
        auto tmpState = state->TouchDown(x, y);
        if(tmpState)
            state = tmpState;
    }
}

void TouchRecognizer::TouchUp(double x, double y)
{
    if(state)
    {
        auto tmpState = state->TouchUp(x, y);
        if(tmpState)
            state = tmpState;
    }
}

void TouchRecognizer::TouchMove(double x, double y)
{
    if(state)
    {
        auto tmpState = state->TouchMove(x, y);
        if(tmpState)
            state = tmpState;
    }
}

void TouchRecognizer::GetLastPosition(double& x, double& y)
{
    if(state)
        state->GetLastPosition(x, y);
}

void TouchRecognizer::RegisterHandler(std::function<void(void)> onDoubleTouch)
{
    handlers.insert(std::make_pair(reinterpret_cast<unsigned long long>(onDoubleTouch.target<void(void)>()), onDoubleTouch));
}

void TouchRecognizer::UnregisterHandler(std::function<void(void)> onDoubleTouch)
{
    handlers.erase(reinterpret_cast<unsigned long long>(onDoubleTouch.target<void(void)>()));
}

std::string TouchRecognizer::GetStateString()const
{
    if(state)
        return typeid(*(state.get())).name();
    else
        return "no state object!";
}

void TouchRecognizer::Triggered()
{
    for(auto& handler: handlers)
        if(handler.second)
            handler.second();
}

std::shared_ptr<TouchState> TouchRecognizer::GetState() const
{
    return state;
}

void TouchRecognizer::SetState(const std::shared_ptr<TouchState> state)
{
    TouchRecognizer::state = state;
}
