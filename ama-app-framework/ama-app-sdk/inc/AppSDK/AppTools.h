/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppTools.h
/// @brief contains useful functions/classes
///
/// Created by zs on 2016/11/22.
/// this file contains the declaration of useful functions/classes
///

#ifndef APPTOOLS_H
#define APPTOOLS_H

#include <string>
#include <vector>

class _ItemID
{
public:
    _ItemID(){};
    _ItemID(const std::string itemID);
    virtual ~_ItemID(){};

    std::string GetString()const;
    std::string GetDomain()const;
    std::string GetOrganization()const;
    std::string GetAppID()const;
    std::string GetScreen()const;

    void SetString(const std::string& itemID);
    void SetDomain(const std::string& domain);
    void SetOrganization(const std::string& organization);
    void SetAppID(const std::string& appID);
    void SetScreenID(const std::string& screenID);

private:
    std::string domain;
    std::string organization;
    std::string appID;
    std::string screenID;
};

class ItemID
: public _ItemID
{
public:
    ItemID();
    ItemID(const std::string itemID);
    virtual ~ItemID(){};

    std::string GetString()const;
    std::string GetDomain()const;
    std::string GetOrganization()const;
    std::string GetAppID()const;
    std::string GetScreen()const;

    ItemID ShiftDomain(const std::string domain)const;
    ItemID ShiftOrganization(const std::string organization)const;
    ItemID ShiftAppID(const std::string appID)const;
    ItemID ShiftScreenID(const std::string screenID)const;
    ItemID ShiftScreenID(unsigned int id)const;

    void Intent(const std::string action = "", const std::string data ="");
    void Intent(const std::string action, const std::string data, bool isQuiet);

private:
    static _ItemID currentApp;
};

std::vector<std::string> Split(const std::string str, const std::string delimiter);
std::string MakeDelimitedString(const std::string& delimiter, const std::vector<std::string>& strings);
std::string MakeDelimitedString(const char* delimiter, const std::vector<std::string>& strings);
std::string MakeDelimitedString(const char delimiter, const std::vector<std::string>& strings);

void MakeStringVector(std::vector<std::string>& stringVec, const char* arg);
void MakeStringVector(std::vector<std::string>& stringVec, const std::string& arg);

template<class... ARGS>
static void MakeStringVector(std::vector<std::string>& stringVec, const std::string& argi, ARGS... argn);

template<class... ARGS>
static void MakeStringVector(std::vector<std::string>& stringVec, const char* argi, ARGS... argn)
{
    if(argi)
        stringVec.emplace_back(argi);

    MakeStringVector(stringVec, argn...);
}

template<class... ARGS>
static void MakeStringVector(std::vector<std::string>& stringVec, const std::string& argi, ARGS... argn)
{
    stringVec.push_back(argi);
    MakeStringVector(stringVec, argn...);
}

// MakeDelimitedString(delimiter, str1, str2 ...)
template<class... ARGS>
static std::string MakeDelimitedString(const std::string& delimiter, ARGS... args)
{
    std::vector<std::string> stringVec;
    MakeStringVector(stringVec, args...);
    return MakeDelimitedString(delimiter, stringVec);
}

// MakeDelimitedString(',', str1, str2 ...)
template<class... ARGS>
static std::string MakeDelimitedString(const char delimiter, ARGS... args)
{
    std::string delimiterStr;
    delimiterStr += delimiter;
    std::vector<std::string> stringVec;
    MakeStringVector(stringVec, args...);
    return MakeDelimitedString(delimiterStr, stringVec);
}

// MakeDelimitedString(",", "str1", "str2" ...)
template<class... ARGS>
static std::string MakeDelimitedString(const char* delimiter, ARGS... args)
{
    std::string delimiterStr;

    if(delimiter)
        delimiterStr = delimiter;

    std::vector<std::string> stringVec;
    MakeStringVector(stringVec, args...);
    return MakeDelimitedString(delimiterStr, stringVec);
}

#define MKDS MakeDelimitedString

/// @name NotifyWaiter
/// @{
namespace NotifyWaiter {
void RegisterID(unsigned long long id);
bool Wait(unsigned long long id, unsigned int timeout_ms = 300);
void Notify(unsigned long long id);
void UnregisterID(unsigned long long id);
}
/// @}

/// @name profiling tools
/// @{

// you can print log to /tmp/yourItemID.bootlog
// during the time your App is booting.
void AppendBootLog(const std::string& log);
/// @}

#endif // APPTOOLS_H
