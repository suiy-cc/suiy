/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LastSourceManager.h
/// @brief contains class LastSourceManager
/// 
/// Created by zs on 2017/12/26.
///
/// this file contains the definition of class LastSourceManager
/// 

#ifndef LASTSOURCEMANAGER_H
#define LASTSOURCEMANAGER_H

#include <string>
#include <vector>

#include "ama_types.h"
#include "IntentInfo.h"

class LastSourceManager
{
public:
    LastSourceManager();
    ~LastSourceManager();

    void SetCurrentSource(const std::vector<IntentInfo> &sourceSet);
    const std::vector<IntentInfo>& GetCurrentSource()const;
    const std::vector<IntentInfo>& GetLastSource()const;

    void Serialize();
    void Deserialize();

protected:
    static bool Serialize(const std::string &path, const std::vector<IntentInfo> &lastSourceSet);
    static bool Deserialize(const std::string& path, std::vector<IntentInfo>& lastSourceSet);

private:
    std::vector<IntentInfo> lastSourceSet;
    std::vector<IntentInfo> currentSourceSet;

    static const std::string defLastSourcePath;
    static const std::string lastSourcePath;
};


#endif // LASTSOURCEMANAGER_H
