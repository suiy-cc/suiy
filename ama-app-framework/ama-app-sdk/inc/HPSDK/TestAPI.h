/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TestAPI.h
/// @brief contains APIs for all kinds of test
/// 
/// Created by zs on 2017/09/14.
///

#ifndef TESTAPI_H
#define TESTAPI_H

#include <ama_enum.h>

#include <string>

// to mock AMGR that current process is some APP
void Mock(AMPID app);
void Mock(const std::string& itemID);

#endif // TESTAPI_H
