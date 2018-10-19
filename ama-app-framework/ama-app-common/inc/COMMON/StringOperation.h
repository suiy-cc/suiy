/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file StringOperation.h
/// @brief contains some string operations
///
/// Created by zs on 2016/11/18.
/// this file contains the declaration of some string operations
///

#ifndef STRINGOPERATION_H
#define STRINGOPERATION_H

#include <vector>
#include <string>

std::vector<std::string> split(const std::string& str, const std::string& delimeter);
bool IsComment(const std::string& str);
std::string ToString(const char* str);

#endif // STRINGOPERATION_H
