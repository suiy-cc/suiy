/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ActiveAppHome.h
/// @brief contains class ActiveAppHome
///
/// Created by zs on 2016/10/13.
/// this file contains the definition of class ActiveAppHome
///

#ifndef ACTIVEAPPHOME_H
#define ACTIVEAPPHOME_H

#include "ActiveApp.h"

class ActiveAppHome
: public ActiveApp
{
public:
    /// @name constructors & destructors
    /// @{
    ActiveAppHome():Process(INVALID_AMPID, std::make_shared<InitialState>()){};
    ActiveAppHome(AMPID app):Process(app, std::make_shared<InitialState>()), ActiveApp(app){};
    virtual ~ActiveAppHome(){};
    /// @}

    /// @name notifies
    /// @{
    virtual void ShowNotify() override;
    virtual void InstalledPKGsChangedNotify();
    virtual void HomeReadyNotify();
    /// @}

    /// @name getters & setters
    /// @{
    virtual bool IsShowStartupScreen()const override;
    /// @}
};


#endif // ACTIVEAPPHOME_H
