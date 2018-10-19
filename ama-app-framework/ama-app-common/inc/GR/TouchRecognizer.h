/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TouchRecognizer.h
/// @brief contains 
/// 
/// Created by zs on 2017/03/01.
///
/// this file contains the definition of class TouchRecognizer
/// 

#ifndef TOUCHRECOGNIZER_H
#define TOUCHRECOGNIZER_H

#include <functional>
#include <map>
#include <memory>

class TouchState;

class TouchRecognizer
{
public:
    TouchRecognizer();
    TouchRecognizer(const std::shared_ptr<TouchState> state);
    virtual ~TouchRecognizer();

    virtual void TouchDown(double x, double y);
    virtual void TouchUp(double x, double y);
    virtual void TouchMove(double x, double y);
    virtual void GetLastPosition(double& x, double& y);

    virtual void RegisterHandler(std::function<void(void)> onDoubleTouch);
    virtual void UnregisterHandler(std::function<void(void)> onDoubleTouch);
    virtual std::string GetStateString()const;

    std::shared_ptr<TouchState> GetState() const;
    void SetState(const std::shared_ptr<TouchState> state);

protected:
    virtual void Triggered();

private:
    std::shared_ptr<TouchState> state;
    std::map<unsigned long long, std::function<void(void)>> handlers;
};


#endif //AMA_APP_FRAMEWORK_TOUCHRECOGNIZER_H
