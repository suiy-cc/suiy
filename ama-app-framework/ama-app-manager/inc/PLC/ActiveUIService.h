/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveUIService.h
/// @brief contains class ActiveUIService
///
/// Created by zs on 2016/10/13.
/// this file contains the define of class ActiveUIService
///

#ifndef ACTIVEUISERVICE_H
#define ACTIVEUISERVICE_H

#include "UIProcess.h"
#include "ActiveService.h"

class ActiveUIService
: virtual public UIProcess
, virtual public ActiveService
{
public:
    /// @name constructors & destructors
    /// @{
    ActiveUIService():Process(INVALID_AMPID, std::make_shared<InitialState>()),ActiveService(INVALID_AMPID),UIProcess(INVALID_AMPID){};
    ActiveUIService(AMPID app):Process(app, std::make_shared<InitialState>()),ActiveService(app),UIProcess(app){};
    virtual ~ActiveUIService(){};
    /// @}

    bool IsAlwaysVisible();
    virtual bool IsShowStartupScreen()const override;
};


#endif // ACTIVEUISERVICE_H
