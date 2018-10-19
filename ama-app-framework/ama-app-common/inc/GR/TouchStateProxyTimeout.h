/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchStateProxyTimeout.h
/// @brief contains 
/// 
/// Created by dev on 2017/07/19.
///
/// this file contains the definition of class TouchStateProxyTimeout
/// 

#ifndef TOUCHSTATEPROXYTIMEOUT_H
#define TOUCHSTATEPROXYTIMEOUT_H

#include "GR/TouchStateProxyBase.h"
#include "COMMON/Timeout.h"

class TouchStateProxyTimeout
: public TouchStateProxyBase
{
public:
    TouchStateProxyTimeout(std::shared_ptr<TouchState> touchState, const std::chrono::milliseconds &timeout);

    std::shared_ptr<TouchState> TouchDown(double x, double y) override;
    std::shared_ptr<TouchState> TouchUp(double x, double y) override;
    std::shared_ptr<TouchState> TouchMove(double x, double y) override;
    void Copy(TouchState *from) override;

protected:
    virtual std::shared_ptr<TouchStateProxyBase> GetNewInstance(const std::shared_ptr<TouchState>& state) const override;

public:
    bool IsTimeout();

private:
    Timeout timeout;
};


#endif // TOUCHSTATEPROXYTIMEOUT_H
