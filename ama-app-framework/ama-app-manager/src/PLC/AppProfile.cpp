/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppProfile.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/09/18.
///
/// this file contains the implementation of class AppProfile
///

#include <fstream>

#include "AppProfile.h"

AppProfile::AppProfile()
{
    ;
}

AppProfile::AppProfile(const std::string &itemID)
    : itemID(itemID)
{}

AppProfile::~AppProfile()
{

}

const std::string &AppProfile::GetItemID() const
{
    return itemID;
}

void AppProfile::SetItemID(const std::string &itemID)
{
    AppProfile::itemID = itemID;
}

void AppProfile::AppendLog(const std::string &log)
{
    this->log.push_back(log);
}

void AppProfile::Serialize(const std::string &dir)
{
    std::string ext = ".bootlog";
    std::string path = dir+itemID+ext;

    // avoid overwriting a existing file.
    std::ifstream iFile(path.c_str());
    int duplicateCount = 0;
    while(iFile.is_open())
    {
        ++duplicateCount;
        path = dir+itemID+"."+std::to_string(duplicateCount)+ext;
        iFile.close();
        iFile.open(path.c_str());
    }
    iFile.close();

    // write profile log into bootlog
    std::ofstream oFile(path.c_str());
    if(oFile.is_open())
    {
        for(auto &line: log)
            oFile << line << std::endl;

        oFile.close();
    }
}
