/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveService.h
/// @brief contains class ActiveService
///
/// Created by zs on 2016/10/13.
/// this file contains the definition of class ActiveService
///

#ifndef ACTIVESERVICE_H
#define ACTIVESERVICE_H

#include "Process.h"

class ActiveService
: virtual public Process
{
public:
    /// @name constructors & destructors
    /// @{
    ActiveService(){};
    ActiveService(AMPID app):Process(app){};
    virtual ~ActiveService(){};
    /// @}

    void Stop();
};


#endif // ACTIVESERVICE_H
