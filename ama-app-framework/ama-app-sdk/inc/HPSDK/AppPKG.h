/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPKG.h
/// @brief contains the definition of class AppPKG
///
/// Created by zs on 2016/8/18.
/// this file contains the definition of class AppPKG
///

#ifndef APPPKG_H
#define APPPKG_H

#include <string>
#include <unordered_map>
#include <memory>

class AppPKG
{
public:
    AppPKG();
    virtual ~AppPKG();

public:
    bool operator==(AppPKG)const;

    void Start(int arg);

    /// @name getters & setters
    /// @{
    unsigned int GetAmpid() const;
    void SetAmpid(unsigned int ampid);
    const std::string &GetItemID() const;
    void SetItemID(const std::string &itemID);
    const std::string &GetBigIconPath() const;
    void SetBigIconPath(const std::string &bigIconPath);
    const std::string &GetSmallIconPath() const;
    void SetSmallIconPath(const std::string &smallIconPath);
    void SetAppName(const std::string &appName);
    const std::string &GetAppName() const;
    /// @}

private:
    unsigned int ampid;
    std::string appName;
    std::string itemID;
    std::string bigIconPath;
    std::string smallIconPath;
};

class AppPKGEx
{
public:
    AppPKGEx();
    virtual ~AppPKGEx();

public:
    bool operator==(const AppPKGEx& )const;

    void Start(int arg = 0);

    /// @name getters & setters
    /// @{
    unsigned int GetAmpid() const;
    void SetAmpid(unsigned int ampid);
    const std::string GetItemID() const;
    void SetItemID(const std::string &itemID);
    const std::string GetBigIconPath() const;
    void SetBigIconPath(const std::string &bigIconPath);
    const std::string GetSmallIconPath() const;
    void SetSmallIconPath(const std::string &smallIconPath);
    void SetAppName(const std::string &appName);
    const std::string GetAppName() const;
    void Set(const std::string& key, const std::string& value);
    // return null if specified value is not exist.
    std::shared_ptr<std::string> Get(const std::string& key)const;
    /// @}

private:
    unsigned int ampid;
    std::unordered_map<std::string, std::string> keyValuePair;
};

#endif // APPPKG_H
