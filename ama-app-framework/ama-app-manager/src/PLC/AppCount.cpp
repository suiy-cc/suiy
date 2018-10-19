/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppCount.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/08/22.
///

/// this file contains the implementation of class AppCount
///

#include <fstream>

#include "AppCount.h"
#include "log.h"
#include "AMGRLogContext.h"

static std::string appCountPath = "/media/ivi-data/ama.d/appCount";

AppCount::AppCount()
: lastAppCount(0)
, currentAppCount(0)
, redundantFileIO(appCountPath)
{
    logInfo(PLC, LOG_HEAD, "this = ", reinterpret_cast<unsigned long long>(this));
    redundantFileIO.SetSerializeCB([this](const std::string& path){Serialize(path);});
    redundantFileIO.SetDeserializeCB([this](const std::string& path)->bool{return Deserialize(path);});

    redundantFileIO.Deserialize();
}

AppCount::~AppCount()
{

}

void AppCount::SetCurrentAppCount(unsigned int count)
{
    logInfo(PLC, LOG_HEAD, "this = ", reinterpret_cast<unsigned long long>(this));
    logInfo(PLC, LOG_HEAD, "count = ", count);

    currentAppCount = count;
    redundantFileIO.Serialize();
}

bool AppCount::IsThereMissingApps() const
{
    return currentAppCount < lastAppCount;
}

void AppCount::Serialize(const std::string &path)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTIONP__+"(%s)", path.c_str()));

    logInfo(PLC, LOG_HEAD, "this = ", reinterpret_cast<unsigned long long>(this));
    try
    {
        std::ofstream oFile(path.c_str());
        if(oFile.is_open())
        {
            oFile << currentAppCount;
            oFile.close();
            logInfo(PLC, LOG_HEAD, "app count = ", currentAppCount);
        }
        else
        {
            logWarn(PLC, LOG_HEAD, "failed in opening file: \"", path, "\"");
        }
    }
    catch(std::exception& e)
    {
        logError(PLC, LOG_HEAD, "exception: ", (&e)->what());
    }
}

bool AppCount::Deserialize(const std::string &path)
{
    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTIONP__+"(%s)", path.c_str()));

    try
    {
        std::ifstream iFile(path.c_str());
        if(iFile.is_open())
        {
            iFile >> lastAppCount;
            logInfo(PLC, LOG_HEAD, "lasttime app count = ", lastAppCount);
            iFile.close();
            return true;
        }
        else
        {
            logWarn(PLC, LOG_HEAD, "failed in opening file: \"", path, "\"");
            return false;
        }
    }
    catch(std::exception& e)
    {
        logError(PLC, LOG_HEAD, "exception: ", (&e)->what());
    }

    return false;
}
