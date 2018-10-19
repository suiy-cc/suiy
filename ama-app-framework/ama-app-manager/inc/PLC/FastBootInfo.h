/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FastBootInfo.h
/// @brief contains 
/// 
/// Created by zs on 2018/04/14.
///

/// this file contains the definition of class FastBootInfo
/// 

#ifndef FASTBOOTINFO_H
#define FASTBOOTINFO_H

#include <map>
#include <vector>

#include "ama_types.h"
#include "App.h"

class FastBootInfo
{
public:
    /// @name app info cache system
    /// @{
    void AddFastBootApp(const App& app);
    void RemoveFastBootApp(AMPID app);
    bool IsFastBootApp(AMPID app);
    std::vector<App> GetFastBootApps();
    void Serialize();
    /// @}

protected:
    /// @name app info cache system
    /// @{
    static void SerializeAppInfo(const std::vector<App>& apps, const std::string& path);
    static bool DeserializeAppInfo(std::vector<App>& app, const std::string& path);
    static void SerializeFastBootApps(std::vector<App> homes);
    static std::vector<App> DeserializeFastBootApps();
    /// @}

private:
    std::map<AMPID, App> fastBootAppInfoCache;

};


#endif // FASTBOOTINFO_H
