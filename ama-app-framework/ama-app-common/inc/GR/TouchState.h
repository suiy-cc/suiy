/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchState.h
/// @brief contains class TouchState
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the definition of class TouchState
/// 

#ifndef TOUCHSTATE_H
#define TOUCHSTATE_H

#include <memory>
#include <chrono>

class TouchState
: public std::enable_shared_from_this<TouchState>
{
public:
    TouchState();
    virtual ~TouchState();

    virtual std::shared_ptr<TouchState> TouchDown(double x, double y);
    virtual std::shared_ptr<TouchState> TouchUp(double x, double y);
    virtual std::shared_ptr<TouchState> TouchMove(double x, double y);
    virtual void GetLastPosition(double& ax, double& ay)const;
    virtual void RegisterCallback(std::function<void(void)> notify){this->notify = notify;};
    virtual std::function<void(void)> GetCallback()const{return notify;};
    virtual void Copy(TouchState *from);
    virtual std::shared_ptr<TouchState> GetInitState(std::function<void(void)> callback)const =0;

protected:
    virtual void SendNotify();

private:
    double x;
    double y;

    std::function<void(void)> notify;
};


#endif // TOUCHSTATE_H
