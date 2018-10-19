/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ProcessFactory.h
/// @brief contains class Process's factory method
///
/// Created by zs on 2016/10/13.
/// this file contains the declaration of class Process's factory method
///

#ifndef PROCESSFACTORY_H
#define PROCESSFACTORY_H

#include "ActiveApp.h"
#include "ActiveUIService.h"

shared_ptr<ActiveApp> GetAppObject(AMPID app);
shared_ptr<ActiveUIService> GetUIServiceObject(AMPID app);
shared_ptr<ActiveService> GetCMDServiceObject(AMPID proc);

#endif // PROCESSFACTORY_H
