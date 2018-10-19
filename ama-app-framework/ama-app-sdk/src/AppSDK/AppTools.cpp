/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppTools.cpp
/// @brief contains useful functions/classes
///
/// Created by zs on 2016/11/22.
/// this file contains the definition of useful functions/classes
///

#include <stdarg.h>
#include <map>

#include "AppPLC.h"
#include "AppTools.h"
#include "COMMON/StringOperation.h"
#include "COMMON/NotifyWaiter.h"
#include "ItemID.h"
#include "AppManager.h"

_ItemID ItemID::currentApp;

_ItemID::_ItemID(const std::string itemID)
{
    SetString(itemID);
}

std::string _ItemID::GetString()const
{
    std::vector<std::string> strings;

    strings.reserve(4);
    if(!GetDomain().empty())
        strings.push_back(GetDomain());
    if(!GetOrganization().empty())
        strings.push_back(GetOrganization());
    if(!GetAppID().empty())
        strings.push_back(GetAppID());
    if(!GetScreen().empty())
        strings.push_back(GetScreen());

    if(strings.size()==0)
        return "";
    if(strings.size()==1)
        return strings[0];
    if(strings.size()>=2)
    {
        return std::move(MKDS(".", strings));
    }
}

std::string _ItemID::GetDomain()const
{
    return domain;
}

std::string _ItemID::GetOrganization()const
{
    return organization;
}

std::string _ItemID::GetAppID()const
{
    return appID;
}

std::string _ItemID::GetScreen()const
{
    return screenID;
}

void _ItemID::SetString(const std::string& itemID)
{
    auto strings = Split(itemID, ".");
    if(strings.size()==4)
    {
        SetDomain(strings[0]);
        SetOrganization(strings[1]);
        SetAppID(strings[2]);
        SetScreenID(strings[3]);
    }
}

void _ItemID::SetDomain(const std::string& domain)
{
    this->domain = domain;
}

void _ItemID::SetOrganization(const std::string& organization)
{
    this->organization = organization;
}

void _ItemID::SetAppID(const std::string& appID)
{
    this->appID = appID;
}

void _ItemID::SetScreenID(const std::string& screenID)
{
    this->screenID = screenID;
}

_ItemID Get_ItemID(unsigned int app)
{
    std::string itemID = GetItemID(app);
    if(!IsValidItemID(itemID))
        itemID.clear();
    return _ItemID(itemID);
}

_ItemID Get_ItemID()
{
    return Get_ItemID(GetAMPID());
}

ItemID::ItemID()
: _ItemID("?.?.?.?")
{
    if(currentApp.GetString().empty())
        currentApp = Get_ItemID();
}

ItemID::ItemID(const std::string itemID)
: _ItemID(itemID)
{
    if(currentApp.GetString().empty())
        currentApp = Get_ItemID();

    if(_ItemID::GetString().empty())
        SetString("?.?.?.?");
}

std::string ItemID::GetString()const
{
    return GetDomain() + '.' + GetOrganization() + '.' + GetAppID() + '.' + GetScreen();
}

std::string ItemID::GetDomain()const
{
    if(_ItemID::GetDomain()=="?" || _ItemID::GetDomain().empty())
        return currentApp.GetDomain();
    else
        return _ItemID::GetDomain();
}

std::string ItemID::GetOrganization()const
{
    if(_ItemID::GetOrganization()=="?" || _ItemID::GetOrganization().empty())
        return currentApp.GetOrganization();
    else
        return _ItemID::GetOrganization();
}

std::string ItemID::GetAppID()const
{
    if(_ItemID::GetAppID()=="?" || _ItemID::GetAppID().empty())
        return currentApp.GetAppID();
    else
        return _ItemID::GetAppID();
}

std::string ItemID::GetScreen()const
{
    if(_ItemID::GetScreen()=="?" || _ItemID::GetScreen().empty())
        return currentApp.GetScreen();
    else
        return _ItemID::GetScreen();
}

ItemID ItemID::ShiftDomain(const std::string domain)const
{
    ItemID newItem = *this;
    newItem.SetDomain(domain);
    return newItem;
}

ItemID ItemID::ShiftOrganization(const std::string organization)const
{
    ItemID newItem = *this;
    newItem.SetOrganization(organization);
    return newItem;
}

ItemID ItemID::ShiftAppID(const std::string appID)const
{
    ItemID newItem = *this;
    newItem.SetAppID(appID);
    return newItem;
}

ItemID ItemID::ShiftScreenID(const std::string screenID)const
{
    ItemID newItem = *this;
    newItem.SetScreenID(screenID);
    return newItem;
}

static std::map<int, std::string> int2ScreenID = {
    {0x0, "scr0"},
    {0x1, "scr1"},
    {0x2, "scr2"},
    {0x3, "scr3"},
    {0xF, "service"},
};

ItemID ItemID::ShiftScreenID(unsigned int id)const
{
    ItemID newItem = *this;
    newItem.SetScreenID(int2ScreenID.at(id));
    return newItem;
}

void ItemID::Intent(const std::string action, const std::string data)
{
    Intent(action, data, false);
}

void ItemID::Intent(const std::string action, const std::string data, bool isQuiet)
{
    ::Intent(GetString(), action, data, isQuiet);
}

std::vector<std::string> Split(const std::string str, const std::string delimiter)
{
    return split(str, delimiter);
}

std::string MakeDelimitedString(const std::string& delimiter, const std::vector<std::string>& strings)
{
    std::string string;
    for(size_t i = 0; i < strings.size(); ++i)
    {
        string += strings[i];

        if(i != (strings.size()-1))
            string += delimiter;
    }

    return std::move(string);
}

std::string MakeDelimitedString(const char* delimiter, const std::vector<std::string>& strings)
{
    std::string delimiterStr;

    if(delimiter)
        delimiterStr = delimiter;

    return MakeDelimitedString(delimiterStr, strings);
}

std::string MakeDelimitedString(const char delimiter, const std::vector<std::string>& strings)
{
    std::string delimiterStr;

    delimiterStr += delimiter;

    return MakeDelimitedString(delimiterStr, strings);
}

void MakeStringVector(std::vector<std::string>& stringVec, const char* arg)
{
    if(arg)
        stringVec.emplace_back(arg);
}

void MakeStringVector(std::vector<std::string>& stringVec, const std::string& arg)
{
    stringVec.push_back(arg);
}

namespace NotifyWaiter {
void RegisterID(unsigned long long id)
{
    ::RegisterWaiterID(id);
}

bool Wait(unsigned long long id, unsigned int timeout_ms/* = 300*/)
{
    ::Wait(id, timeout_ms);
}

void Notify(unsigned long long id)
{
    ::Notify(id);
}

void UnregisterID(unsigned long long id)
{
    ::UnregisterWaiterID(id);
}

}

void AppendBootLog(const std::string& log)
{
    AppManager::GetInstance()->AppendBootLog(log);
}
