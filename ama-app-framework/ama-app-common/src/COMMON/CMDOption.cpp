/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file CMDOption.cpp
/// @brief contains the implementation of class CMDOption
///
/// Created by zs on 2016/10/27.
/// this file contains the implementation of class CMDOption
///

#include "COMMON/CMDOption.h"


CMDOption::CMDOption()
{

}

CMDOption::CMDOption(const std::string &name)
    : name(name)
{}

CMDOption::~CMDOption()
{

}

const std::string &CMDOption::GetName() const
{
    return name;
}

void CMDOption::SetName(const std::string &name)
{
    CMDOption::name = name;
}

const std::string &CMDOption::GetValue() const
{
    return value;
}

std::string CMDOption::GetPrefix()const
{
    return name;
}

bool CMDOption::IsMatch(const std::string &optionText) const
{
    return optionText.find(GetPrefix())==0;
}

bool CMDOption::FromString(const std::string &optionText)
{
    if(IsMatch(optionText))
    {
        value = optionText.substr(GetPrefix().size());
        return !value.empty();
    }
    else
        return false;
}
