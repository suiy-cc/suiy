/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPKGInfo.h
/// @brief contains struct AppPKGInfo
///
/// Created by zs on 2016/8/17
/// this file contains the definition of struct AppPKGInfo
///

#ifndef APPPKGINFO_H
#define APPPKGINFO_H

#include <string>
#include <ama_types.h>

struct AppPKGInfo
{
    AMPID ampid;
    std::string appName;
    std::string itemID;
    std::string bigIconPath;
    std::string smallIconPath;
    std::string appType;
};

#endif // APPPKGINFO_H
