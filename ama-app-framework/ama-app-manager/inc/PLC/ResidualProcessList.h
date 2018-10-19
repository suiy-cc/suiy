/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ResidualProcessList.h
/// @brief contains class ResidualProcessList
///
/// Created by zs on 2016/10/11.
/// this file contains the definition of class ResidualProcessList
///

#ifndef ACTIVEAPPLIST_H
#define ACTIVEAPPLIST_H

#include <string>
#include <vector>

class ResidualProcessList
{
    static const std::string filePath;
public:
    static void Save(const std::string& unitName);
    static void Delete(const std::string& unitName);
    static std::vector<std::string> ReadRecords();
};


#endif // ACTIVEAPPLIST_H
