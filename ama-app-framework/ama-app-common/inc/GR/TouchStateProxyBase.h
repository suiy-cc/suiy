/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchStateProxyBase.h
/// @brief contains 
/// 
/// Created by dev on 2017/07/19.
///
/// this file contains the definition of class TouchStateProxyBase
/// 

#ifndef TOUCHSTATEPROXYBASE_H
#define TOUCHSTATEPROXYBASE_H

#include <GR/TouchState.h>

class TouchStateProxyBase
: public TouchState
{
public:
    TouchStateProxyBase(std::shared_ptr<TouchState> touchState);
    virtual ~TouchStateProxyBase();

    std::shared_ptr<TouchState> TouchDown(double x, double y) override;
    std::shared_ptr<TouchState> TouchUp(double x, double y) override;
    std::shared_ptr<TouchState> TouchMove(double x, double y) override;
    void GetLastPosition(double &ax, double &ay) const override;
    void RegisterCallback(std::function<void(void)> notify) override;
    std::function<void()> GetCallback() const override;
    void Copy(TouchState *from) override;
    std::shared_ptr<TouchState> GetInitState(std::function<void(void)> callback) const override;

    virtual std::shared_ptr<TouchState> GetRawTouchState()const;
    virtual void SetRawTouchState(const std::shared_ptr<TouchState>& state);

protected:
    virtual std::shared_ptr<TouchStateProxyBase> GetNewInstance(const std::shared_ptr<TouchState>& state)const =0;

private:
    std::shared_ptr<TouchState> rawTouchState;
};


#endif // TOUCHSTATEPROXYBASE_H
