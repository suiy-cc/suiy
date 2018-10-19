/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AlwaysVisibleServices.cpp
/// @brief contains 
/// 
/// Created by zs on 2017/05/11.
///

#include "AlwaysVisibleServices.h"
#include "AMGRLogContext.h"
#include "log.h"

#include <fstream>
#include <set>

static std::set<std::string> alwaysVisibleServicesSet;

void LoadAlwaysVisibleServices()
{
    DomainVerboseLog chatter(PLC, "");

    static std::string path = "/etc/ama.d/AlwaysVisibleServices.txt";
    std::ifstream configFile(path.c_str());

    alwaysVisibleServicesSet.clear();

    if(configFile.is_open())
    {
        while(configFile.peek()!=EOF)
        {
            std::string itemID;
            configFile >> itemID;
            alwaysVisibleServicesSet.insert(itemID);
        }

        configFile.close();
    }
}

bool IsAlwaysVisibleService(const std::string& itemID)
{
    return alwaysVisibleServicesSet.find(itemID)!=alwaysVisibleServicesSet.end();
}