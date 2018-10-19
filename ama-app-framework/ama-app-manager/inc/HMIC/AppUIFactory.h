/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUIFactory.h
/// @brief AppUIFactory.h
///
/// Created by zs on 2016/8/23
/// this file contains the declaration of AppUIHMI classes' factory function
///

#ifndef APPUIFACTORY_H
#define APPUIFACTORY_H

#include "AppUI.h"

shared_ptr<AppUI> GetUIObject(AMPID app);

#endif // APPUIFACTORY_H
