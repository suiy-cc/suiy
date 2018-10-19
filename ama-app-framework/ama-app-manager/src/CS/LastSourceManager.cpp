/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LastSourceManager.cpp
/// @brief contains class
/// 
/// Created by zs on 2017/12/26.
///

/// this file contains the implementation of class LastSourceManager
///

#include <fstream>

#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

#include "LastSourceManager.h"
#include "AppList.h"
#include "COMMON/function.h"
#include "Configuration.h"

const std::string LastSourceManager::defLastSourcePath = "/etc/ama.d/defLastSource";
const std::string LastSourceManager::lastSourcePath = "/media/ivi-data/ama.d/lastSource";

LastSourceManager::LastSourceManager()
{
    // delete old configuration
    if(isAppMangerFirstRuningInThisMachine())
    {
        logInfo(CS, LOG_HEAD, "remove old configuration.");
        auto error = remove(lastSourcePath.c_str());
        if(0!=error)
            logWarn(CS, LOG_HEAD, "remove old ", lastSourcePath, " failed!!! ");
        else
            logInfo(CS, LOG_HEAD, "old configuration is removed.");
    }

    if(IsLastSourceEnabled())
        Deserialize();
    currentSourceSet = lastSourceSet;
}

LastSourceManager::~LastSourceManager()
{
}

void LastSourceManager::SetCurrentSource(const std::vector<IntentInfo> &sourceSet)
{
    FUNCTION_SCOPE_LOG_C(CS);

    currentSourceSet = sourceSet;
}

const std::vector<IntentInfo>& LastSourceManager::GetCurrentSource() const
{
    return currentSourceSet;
}

const std::vector<IntentInfo>& LastSourceManager::GetLastSource() const
{
    return lastSourceSet;
}

void LastSourceManager::Serialize()
{
    FUNCTION_SCOPE_LOG_C(CS);

    bool isLastSourceChanged = lastSourceSet!=this->currentSourceSet;

    // return if last source is not changed
    if(!isLastSourceChanged)
        return;

    // save last source set
    Serialize(lastSourcePath, currentSourceSet);

    // remove last source set from fast boot list
    for(const auto& source: this->lastSourceSet)
    {
        auto app = AppList::GetInstance()->GetAMPID(source.itemID);
        AppList::GetInstance()->fastBootList.RemoveFastBootApp(app);
    }

    // add current source set to fast boot list
    for(const auto& source: currentSourceSet)
    {
        auto ampid = AppList::GetInstance()->GetAMPID(source.itemID);
        if(ampid==INVALID_AMPID)
        {
            logWarn(CS, LOG_HEAD, "unknown item ID: ", source.itemID);
            continue;
        }
        App tmp = AppList::GetInstance()->GetApp(ampid);
        AppList::GetInstance()->fastBootList.AddFastBootApp(std::move(tmp));
    }

    // save fast boot list
    AppList::GetInstance()->fastBootList.Serialize();
}

void LastSourceManager::Deserialize()
{
    FUNCTION_SCOPE_LOG_C(CS);

    if(!Deserialize(lastSourcePath, lastSourceSet))
    {
        logWarn(CS, LOG_HEAD, "failed in deserializing from ", lastSourcePath);
        if(!Deserialize(defLastSourcePath, lastSourceSet))
        {
            logError(CS, LOG_HEAD, "failed in deserializing from ", defLastSourcePath);
            return;
        }
    }
}

bool LastSourceManager::Serialize(const std::string &path, const vector<IntentInfo>& _lastSourceSet)
{
    DomainVerboseLog chatter(CS, formatText(__STR_FUNCTION__+"(%s)", path.c_str()));

    try
    {
        std::ofstream oFile(path.c_str());
        cereal::JSONOutputArchive serial(oFile);
        serial(cereal::make_nvp("lastSource", _lastSourceSet));
    }
    catch(std::exception& e)
    {
        logError(CS, LOG_HEAD, "exception is caught: ", (&e)->what());
        return false;
    }
    catch(...)
    {
        logError(CS, LOG_HEAD, "unexpected exception is caught!");
        return false;
    }

    return true;
}

bool LastSourceManager::Deserialize(const std::string &path, std::vector<IntentInfo>& _lastSourceSet)
{
    DomainVerboseLog chatter(CS, formatText(__STR_FUNCTION__+"(%s)", path.c_str()));

    try
    {
        std::ifstream iFile(path.c_str());
        cereal::JSONInputArchive deserial(iFile);
        deserial(cereal::make_nvp("lastSource", _lastSourceSet));
    }
    catch(std::exception& e)
    {
        logError(CS, LOG_HEAD, "exception is caught: ", (&e)->what());
        return false;
    }
    catch(...)
    {
        logError(CS, LOG_HEAD, "unexpected exception is caught!");
        return false;
    }

    return true;
}
