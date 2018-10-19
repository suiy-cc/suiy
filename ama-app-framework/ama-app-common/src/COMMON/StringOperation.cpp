/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file StringOperation.cpp
/// @brief contains some string operations
///
/// Created by zs on 2016/11/18.
/// this file contains the definition of some string operations
///

#include <set>

#include "ama_types.h"
#include "COMMON/StringOperation.h"

std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> ret;
    size_t last = 0;
    size_t index=str.find_first_of(delimiter,last);
    while (index!=std::string::npos)
    {
        ret.push_back(str.substr(last,index-last));
        last=index+1;
        index=str.find_first_of(delimiter,last);
    }
    if (index-last>0)
    {
        ret.push_back(str.substr(last,index-last));
    }

    return std::move(ret);
}

bool IsComment(const std::string& str)
{
    if(!str.empty() && str[0]=='#')
        return true;

    return false;
}

std::string ToString(const char* str)
{
    if(str==nullptr)
        return std::string();
    else
        return std::string(str);
}
