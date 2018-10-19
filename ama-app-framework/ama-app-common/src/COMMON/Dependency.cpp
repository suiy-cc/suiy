/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Dependency.cpp
/// @brief contains the implementation of class Dependency
///
/// Created by zs on 2016/12/5.
/// this file contains the implementation of class Dependency
///

#include <map>
#include "log.h"
#include <COMMON/Dependency.h>

Dependency::Dependency()
: unitName()
, isActive(true)
, rebootCB(nullptr)
{

}

Dependency::Dependency(const std::string unitName)
    : unitName(unitName)
    , isActive(true)
    , rebootCB(nullptr)
{

}

std::string Dependency::GetName() const
{
    return unitName;
}

void Dependency::SetName(const std::string unitName)
{
    this->unitName = unitName;
}

bool Dependency::IsActive() const
{
    return isActive;
}

void Dependency::SetActive(bool isActive)
{
    // from inactive to active means the unit is successfully reboot.
    if(!this->isActive && isActive && rebootCB)
        rebootCB(GetName());

    this->isActive = isActive;
}

void Dependency::SetRebootCB(std::function<void(const std::string &)> f)
{
    rebootCB = f;
}

static std::map<std::string, Dependency> dependencyMap;

bool RegisterDependency(const Dependency& dependency)
{
    if(dependencyMap.find(dependency.GetName())==dependencyMap.end())
    {
        dependencyMap[dependency.GetName()] = dependency;
        return true;
    }
    else
        return false;
}

bool UnregisterDependency(const std::string unitName)
{
    auto iter = dependencyMap.find(unitName);
    if(iter!=dependencyMap.end())
    {
        dependencyMap.erase(iter);
        return true;
    }
    else
        return false;
}

Dependency * GetDependency(const std::string unitName)
{
    auto iter = dependencyMap.find(unitName);
    if(iter!=dependencyMap.end())
    {
        return &dependencyMap[unitName];
    }
    else
        return nullptr;
}

void UnitStateChange(const std::string unitName, bool isActive)
{
    auto dependency = GetDependency(unitName);
    if(dependency)
    {
        dependency->SetActive(isActive);
    }
}

void UnitStateChange(const char* unitName, bool isActive)
{
    UnitStateChange(std::string(unitName), isActive);
}