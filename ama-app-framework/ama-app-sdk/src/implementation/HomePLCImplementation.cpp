/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HomePLCImplementation.cpp
/// @brief contains the implementation of class HomePLCImplementation
///
/// Created by zs on 2016/8/17.
/// this file contains the implementation of class HomePLCImplementation
///

#include "HomePLCImplementation.h"
#include "HomePLC.h"
HomePLCImplementation::HomePLCImplementation(HomePLC *home)
    : AppPLCImplementation(home)
{
    ;
}

HomePLCImplementation::~HomePLCImplementation()
{
    ;
}

void HomePLCImplementation::OnInstalledPKGsChanged()
{
    auto plcIF = GetPLCInterface();
    auto homeIF = dynamic_cast<HomePLC*>(plcIF);

    if(homeIF)
    {
        homeIF->OnInstalledPKGsChanged();
    }
}