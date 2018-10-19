/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file PLCImplementation.h
/// @brief contains class PLCImplementation
///
/// Created by zs on 2016/10/18.
/// this file contains the definition of class PLCImplementation
///

#ifndef PLCIMPLEMENTATION_H
#define PLCIMPLEMENTATION_H

#include <string>

class PLCImplementation
{
public:
    virtual void OnNotifyEarlyInitialize()=0;
    virtual void OnNotifyInitialize(int startArg, bool visible)=0;
    virtual void OnNotifyStop(const std::string &stopCMD)=0;
    virtual void OnNotifyResume()=0;
    virtual void OnNotifyPreShow(int startArg)=0;
    virtual void OnNotifyShow()=0;
    virtual void OnNotifyPreHide()=0;
    virtual void OnNotifyHide()=0;
    virtual bool Initialize(unsigned int ampid)=0;

    //cj add for screen sync
    virtual void OnNotifyActiveAppChange(unsigned int seatid)=0;
};

#endif // PLCIMPLEMENTATION_H
