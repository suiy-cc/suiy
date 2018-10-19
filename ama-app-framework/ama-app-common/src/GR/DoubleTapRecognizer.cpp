/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DoubleTouchRecognizer.cpp
/// @brief contains class DoubleTouchRecognizer
/// 
/// Created by zs on 2017/02/28.
///
/// this file contains the implementation of class DoubleTouchRecognizer
///

#include "GR/DoubleTapRecognizer.h"
#include "GR/DoubleTapState.h"
#include "GR/TouchStateProxyTimeout.h"

DoubleTapRecognizer::DoubleTapRecognizer()
: TouchRecognizer(std::make_shared<TouchStateProxyTimeout>(std::make_shared<DoubleTapStateInit>(), std::chrono::milliseconds(1000)))
{
}

DoubleTapRecognizer::~DoubleTapRecognizer()
{
}

std::string DoubleTapRecognizer::GetStateString() const
{
    ///////////test code///////////
    bool isTimeout = true;
    /////////////////////////////////
    std::shared_ptr<TouchStateProxyBase> proxy = std::dynamic_pointer_cast<TouchStateProxyBase>(GetState());
    std::shared_ptr<TouchState> rawState = nullptr;
    while(proxy)
    {
        ///////test code//////
        if(proxy)
        {
            auto timerProxy = std::dynamic_pointer_cast<TouchStateProxyTimeout>(proxy);
            if(timerProxy)
            {
                isTimeout = timerProxy->IsTimeout();
            }
        }
        /////////////////////////////
        rawState = proxy->GetRawTouchState();
        proxy = std::dynamic_pointer_cast<TouchStateProxyBase>(rawState);
    }
    if(rawState)
        return typeid(*(rawState.get())).name() + std::string(isTimeout?"[timeout]":"[intime]");
    else
        return TouchRecognizer::GetStateString() + std::string(isTimeout?"[timeout]":"[intime]");
}
