/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TapState.h
/// @brief contains class TapState
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the definition of class TapState
/// 

#ifndef TAPSTATE_H
#define TAPSTATE_H


#include <GR/TouchState.h>

class TapState
: public TouchState
{
public:
    std::shared_ptr<TouchState> GetInitState(std::function<void(void)> callback) const override;
};

class TapStateInit
    :public TapState
{
public:
    std::shared_ptr<TouchState> TouchDown(double x, double y) override;
    std::shared_ptr<TouchState> TouchUp(double x, double y) override;
};

class TapStateDown
    :public TapState
{
public:
    std::shared_ptr<TouchState> TouchDown(double x, double y) override;
    std::shared_ptr<TouchState> TouchUp(double x, double y) override;
public:
};


#endif //AMA_APP_FRAMEWORK_TAPSTATE_H
