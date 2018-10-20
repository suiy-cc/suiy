/// @file AppPKG.h
/// @brief contains the defination of class AppPKG
///
/// Created by zs on 2016/8/18.
/// this file contains the defination of class AppPKG
///

#ifndef APPPKG_H
#define APPPKG_H

#include <string>

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


#endif // APPPKG_H
