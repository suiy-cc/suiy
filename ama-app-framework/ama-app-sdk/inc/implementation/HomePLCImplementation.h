/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HomePLCImplementation.h
/// @brief contains class HomePLCImplementation
///
/// Created by zs on 2016/8/17
/// this file contains the definition of class HomePLCImplementation
///

#ifndef HOMEPLCIMPLEMENTATION_H
#define HOMEPLCIMPLEMENTATION_H

#include "AppPLCImplementation.h"
#include "AppPKGInfo.h"

class HomePLC;

class HomePLCImplementation
: public AppPLCImplementation
{
public:
    HomePLCImplementation(HomePLC *);
    virtual ~HomePLCImplementation();

private:
    friend class AppManager;
    void OnInstalledPKGsChanged();
};


#endif //AMA_APP_FRAMEWORK_HOMEPLCIMPLEMENTATION_H
