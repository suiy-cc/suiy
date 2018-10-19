/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPKG.cpp
/// @brief contains the implementation of class AppPKG
///
/// Created by zs on 2016/8/18.
/// this file contains the implementation of class AppPKG
///

#include "AppPKG.h"
#include "HomePLC.h"
AppPKG::AppPKG()
{}

AppPKG::~AppPKG()
{

}

bool AppPKG::operator==(AppPKG appPKG)const
{
    if(ampid!=appPKG.ampid)
        return false;
    if(appName!=appPKG.appName)
        return false;
    if(itemID!=appPKG.itemID)
        return false;
    if(bigIconPath!=appPKG.bigIconPath)
        return false;
    if(smallIconPath!=appPKG.smallIconPath)
        return false;

    return true;
}

void AppPKG::Start(int arg)
{
    StartApp(ampid, arg);
}
unsigned int AppPKG::GetAmpid() const
{
    return ampid;
}
void AppPKG::SetAmpid(unsigned int ampid)
{
    AppPKG::ampid = ampid;
}
const std::string &AppPKG::GetItemID() const
{
    return itemID;
}
void AppPKG::SetItemID(const std::string &itemID)
{
    AppPKG::itemID = itemID;
}
const std::string &AppPKG::GetBigIconPath() const
{
    return bigIconPath;
}
void AppPKG::SetBigIconPath(const std::string &bigIconPath)
{
    AppPKG::bigIconPath = bigIconPath;
}
const std::string &AppPKG::GetSmallIconPath() const
{
    return smallIconPath;
}
void AppPKG::SetSmallIconPath(const std::string &smallIconPath)
{
    AppPKG::smallIconPath = smallIconPath;
}
void AppPKG::SetAppName(const std::string &appName)
{
    AppPKG::appName = appName;
}
const std::string &AppPKG::GetAppName() const
{
    return appName;
}

AppPKGEx::AppPKGEx()
: ampid(0)
{

}

AppPKGEx::~AppPKGEx()
{

}

bool AppPKGEx::operator==(const AppPKGEx & appPKGEx) const
{
    if(ampid!=appPKGEx.ampid)
        return false;
    for(auto& item : keyValuePair)
        if(!appPKGEx.Get(item.first))
            return false;
        else if(appPKGEx.Get(item.first) && item.second!=*appPKGEx.Get(item.first))
            return false;

    return true;
}

void AppPKGEx::Start(int arg)
{
    StartApp(GetAmpid(), arg);
}

unsigned int AppPKGEx::GetAmpid() const
{
    return ampid;
}

void AppPKGEx::SetAmpid(unsigned int ampid)
{
    this->ampid = ampid;
}

const std::string AppPKGEx::GetItemID() const
{
    auto itemID = Get("ItemID");
    if(itemID)
        return std::move(*itemID);
    else
        return std::string();
}

void AppPKGEx::SetItemID(const std::string &itemID)
{
    Set("ItemID", itemID);
}

const std::string AppPKGEx::GetBigIconPath() const
{
    auto bigIconPath = Get("BigIconPath");
    if(bigIconPath)
        return std::move(*bigIconPath);
    else
        return std::string();
}

void AppPKGEx::SetBigIconPath(const std::string &bigIconPath)
{
    Set("BigIconPath", bigIconPath);
}

const std::string AppPKGEx::GetSmallIconPath() const
{
    auto smallIconPath = Get("SmallIconPath");
    if(smallIconPath)
        return std::move(*smallIconPath);
    else
        return std::string();
}
void AppPKGEx::SetSmallIconPath(const std::string &smallIconPath)
{
    Set("SmallIconPath", smallIconPath);
}

void AppPKGEx::SetAppName(const std::string &appName)
{
    Set("AppName", appName);
}

const std::string AppPKGEx::GetAppName() const
{
    auto appName = Get("AppName");
    if(appName)
        return std::move(*appName);
    else
        return std::string();
}

void AppPKGEx::Set(const std::string &key, const std::string &value)
{
    keyValuePair[key] = value;
}

std::shared_ptr<std::string> AppPKGEx::Get(const std::string &key) const
{
    if(keyValuePair.find(key)==keyValuePair.end())
        return nullptr;
    else
        return std::make_shared<std::string>(keyValuePair.find(key)->second);
}
