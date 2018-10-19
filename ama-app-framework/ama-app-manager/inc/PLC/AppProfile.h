/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppProfile.h
/// @brief contains 
/// 
/// Created by zs on 2017/09/18.
///
/// this file contains the definition of class AppProfile
/// 

#ifndef APPPROFILE_H
#define APPPROFILE_H

#include <vector>
#include <string>

class AppProfile
{
public:
    AppProfile();
    AppProfile(const std::string &itemID);
    virtual ~AppProfile();

    const std::string &GetItemID() const;
    void SetItemID(const std::string &itemID);

    void AppendLog(const std::string& log);
    void Serialize(const std::string& dir);

private:
    std::vector<std::string> log;
    std::string itemID;
};


#endif // APPPROFILE_H
