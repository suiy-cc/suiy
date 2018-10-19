/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file DataBase.h
/// @brief contains 
/// 
/// Created by zs on 2018/06/21.
///

#ifndef DATABASE_H
#define DATABASE_H

#include "AppDB.h"

extern AppDatabase AMGRDB;

extern bool IsRSEEnabled();
extern void EnableRSE(bool enable = true);

#define AMGRDB_KEY_IS_RSE_ENABLED "IsRSEEnabled"

#endif // DATABASE_H
