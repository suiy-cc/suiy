/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file BootArguments.h
/// @brief contains function GetBootArguments
///
/// Created by zs on 2016/8/10.
/// this file contains the declaration of function GetBootArguments
///

#ifndef BOOTARGUMENTS_H
#define BOOTARGUMENTS_H

#include <string>
#include <vector>

const std::string system(const std::string& cmd);
std::vector<std::string> GetBootArguments();

#endif // BOOTARGUMENTS_H
