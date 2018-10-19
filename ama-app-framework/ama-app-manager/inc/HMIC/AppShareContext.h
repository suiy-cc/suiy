/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppShareContext.h
/// @brief contains class AppShareContext
///
/// Created by zs on 2016/12/19
/// this file contains the definition of class AppShareContext
///

#ifndef APPSHARECONTEXT_H
#define APPSHARECONTEXT_H


#include "ama_types.h"

struct AppShareContext
{
    AMPID sourceApp;
    E_SEAT destinationScreen;
    bool isMove;
};


#endif // APPSHARECONTEXT_H
