/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file CMDOption.h
/// @brief contains class CMDOption
///
/// Created by zs on 2016/10/27
/// this file contains the definition of class CMDOption
///

#ifndef CMDOPTION_H
#define CMDOPTION_H

#include <string>

class CMDOption
{
public:
    CMDOption();
    CMDOption(const std::string &name);
    virtual ~CMDOption();

    const std::string &GetName() const;
    void SetName(const std::string &name);
    const std::string &GetValue() const;

    std::string GetPrefix()const;
    bool IsMatch(const std::string &optionText) const;
    bool FromString(const std::string &optionText);

private:
    std::string name;
    std::string value;
};


#endif // CMDOPTION_H
