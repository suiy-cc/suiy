/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Dependency.h
/// @brief contains class Dependency
///
/// Created by zs on 2016/12/5.
/// this file contains the definition of class Dependency
///

#ifndef DEPENDENCY_H
#define DEPENDENCY_H

#include <string>
#include <functional>

class Dependency
{
public:
    Dependency();
    Dependency(const std::string unitName);
    virtual ~Dependency(){};

    std::string GetName()const;
    void SetName(const std::string unitName);
    bool IsActive()const;
    void SetActive(bool isActive = true);

    void SetRebootCB(std::function<void(const std::string &)> f);

private:
    std::string unitName;
    bool isActive;
    std::function<void(const std::string&)> rebootCB;
};

bool RegisterDependency(const Dependency& dependency);
bool UnregisterDependency(const std::string unitName);
Dependency * GetDependency(const std::string unitName);
void UnitStateChange(const std::string unitName, bool isActive);
void UnitStateChange(const char* unitName, bool isActive);

#endif // DEPENDENCY_H
