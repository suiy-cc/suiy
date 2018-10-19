/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DoubleTapState.h
/// @brief contains DoubleTapState
/// 
/// Created by zs on 2017/02/28.
///
/// this file contains the definition of class DoubleTapState
/// 

#ifndef DOUBLETOUCHSTATE_H
#define DOUBLETOUCHSTATE_H

#include <GR/TouchState.h>
#include <COMMON/Timeout.h>

class DoubleTapState
: public TouchState
{
public:
    std::shared_ptr<TouchState> GetInitState(std::function<void(void)> callback) const override;
};

class DoubleTapStateInit
: public DoubleTapState
{
public:
    virtual std::shared_ptr<TouchState> TouchDown(double x, double y)override;
    virtual std::shared_ptr<TouchState> TouchUp(double x, double y)override;
};

class DoubleTapStateDown
: public DoubleTapState
{
public:
    virtual std::shared_ptr<TouchState> TouchDown(double x, double y)override;
    virtual std::shared_ptr<TouchState> TouchUp(double x, double y)override;
};

class DoubleTapStateSingleTouch
: public DoubleTapState
{
public:
    virtual std::shared_ptr<TouchState> TouchDown(double x, double y)override;
    virtual std::shared_ptr<TouchState> TouchUp(double x, double y)override;
};

class DoubleTapStateSingleTouchDown
: public DoubleTapState
{
public:
    virtual std::shared_ptr<TouchState> TouchDown(double x, double y)override;
    virtual std::shared_ptr<TouchState> TouchUp(double x, double y)override;
};

#endif // DOUBLETOUCHSTATE_H
