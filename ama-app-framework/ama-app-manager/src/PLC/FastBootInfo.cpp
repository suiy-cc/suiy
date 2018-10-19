/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FastBootInfo.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/04/14.
///

/// this file contains the implementation of class FastBootInfo
///

#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"

#include "FastBootInfo.h"
#include "log.h"
#include "AMGRLogContext.h"
#include "COMMON/function.h"

void FastBootInfo::Serialize()
{
    DomainVerboseLog chatter(PLC, __STR_FUNCTIONP__);

    std::vector<App> apps;
    apps.reserve(fastBootAppInfoCache.size());
    for(auto& app: fastBootAppInfoCache)
    {
        apps.push_back(app.second);
    }

    SerializeFastBootApps(apps);
}

void FastBootInfo::AddFastBootApp(const App& app)
{
    DomainVerboseLog chatter(PLC, formatText("FastBootInfo::AddFastBootApp(%#x)", app.getAMPID()));

    fastBootAppInfoCache.insert(std::make_pair(app.getAMPID(), app));
}

void FastBootInfo::RemoveFastBootApp(AMPID app)
{
    DomainVerboseLog chatter(PLC, formatText("FastBootInfo::RemoveFastBootApp(%#x)", app));

    fastBootAppInfoCache.erase(app);
}

bool FastBootInfo::IsFastBootApp(AMPID app)
{
    DomainVerboseLog chatter(PLC, formatText("FastBootInfo::IsFastBootApp(%#x)", app));

    return fastBootAppInfoCache.find(app)!=fastBootAppInfoCache.end();
}

std::vector<App> FastBootInfo::GetFastBootApps()
{
    DomainVerboseLog chatter(PLC, "FastBootInfo::GetFastBootApps()");

    std::vector<App> rtv;
    if(fastBootAppInfoCache.empty())
    {
        rtv = DeserializeFastBootApps();
        for(auto& app: rtv)
            fastBootAppInfoCache.insert(std::make_pair(app.getAMPID(), app));
    }
    else
    {
        for(auto& app: fastBootAppInfoCache)
        {
            rtv.push_back(app.second);
        }
    }

    return std::move(rtv);
}

static std::string defaultFastBootAppList = "/etc/ama.d/FastBootList";
static std::string fastBootAppListFilePath = "/media/ivi-data/ama.d/fastBootApps";
static std::string fastBootAppListRedundance = "/media/ivi-data/ama.d/fastBootApps.redundance";

void FastBootInfo::SerializeAppInfo(const std::vector<App>& apps, const std::string& path)
{
    DomainVerboseLog chatter(PLC, formatText("FastBootInfo::SerializeAppInfo(%s)", path.c_str()));

    try
    {
        std::ofstream file(path.c_str());
        if(file.is_open())
        {
            {
                cereal::JSONOutputArchive serialize(file);
                serialize(cereal::make_nvp("fastBootApps", apps));
            }
            file.close();
        }
    }
    catch(std::exception& e)
    {
        logWarn(PLC, "FastBootInfo::SerializeAppInfo(): exception is caught: ", (&e)->what());
    }
    catch(...)
    {
        logWarn(PLC, "FastBootInfo::SerializeAppInfo(): unexpected exception is caught!");
    }
}

bool FastBootInfo::DeserializeAppInfo(std::vector<App>& apps, const std::string& path)
{
    DomainVerboseLog chatter(PLC, formatText("FastBootInfo::DeserializeAppInfo(%s)", path.c_str()));

    try
    {
        std::ifstream file(path.c_str());
        if(file.is_open())
        {
            {
                cereal::JSONInputArchive deserialize(file);
                deserialize(cereal::make_nvp("fastBootApps", apps));
            }

            file.close();
            return true;
        }
        else
        {
            logWarn(PLC, "FastBootInfo::DeserializeAppInfo(): failed in opening file: ", path);
            return false;
        }
    }
    catch(std::exception& e)
    {
        logWarn(PLC, "FastBootInfo::DeserializeAppInfo(): exception is caught: ", (&e)->what());
    }
    catch(...)
    {
        logWarn(PLC, "FastBootInfo::DeserializeAppInfo(): unexpected exception is caught!");
    }

    return false;
}

void FastBootInfo::SerializeFastBootApps(std::vector<App> apps)
{
    DomainVerboseLog chatter(PLC, "FastBootInfo::SerializeFastBootApps()");

    // save app info
    SerializeAppInfo(apps, fastBootAppListFilePath);

    // make a reduandance file
    SerializeAppInfo(apps, fastBootAppListRedundance);
}

std::vector<App> FastBootInfo::DeserializeFastBootApps()
{
    DomainVerboseLog chatter(PLC, "FastBootInfo::DeserializeFastBootApps()");

    // delete old configuration
    if(isAppMangerFirstRuningInThisMachine())
    {
        logInfo(PLC, LOG_HEAD, "remove old configuration.");
        auto error = remove(fastBootAppListRedundance.c_str());
        if(0!=error)
            logWarn(PLC, LOG_HEAD, "remove old ", fastBootAppListRedundance, " failed!!! ");

        error = remove(fastBootAppListFilePath.c_str());
        if(0!=error)
            logWarn(PLC, LOG_HEAD, "remove old ", fastBootAppListFilePath, " failed!!! ");
    }

    std::vector<App> rtv;
    if(!DeserializeAppInfo(rtv, fastBootAppListFilePath))
    {
        logWarn(PLC, "FastBootInfo::DeserializeFastBootApps(): failed in deserializing original file! try redundance file...");
        rtv.clear();
        if(!DeserializeAppInfo(rtv, fastBootAppListRedundance))
        {
            logWarn(PLC, "FastBootInfo::DeserializeFastBootApps(): failed in deserializing redundance file too, try default file...");
            if(!DeserializeAppInfo(rtv, defaultFastBootAppList))
                logError(PLC, "FastBootInfo::DeserializeFastBootApps(): failed in deserializing default file too, fast boot failed.");
        }
        else
            logWarn(PLC, "FastBootInfo::DeserializeFastBootApps(): redundance file is OK!");
    }

    return std::move(rtv);
}
