/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ShutdownManager.h
/// @brief contains class ShutdownManager
///
/// Created by zs on 2016/10/26.
/// this file contains the definition of class ShutdownManager
///

#ifndef SHUTDOWNNOTIFIER_H
#define SHUTDOWNNOTIFIER_H

#include <memory>
#include <unordered_set>

#include "ama_types.h"

class Process;

enum E_SHUTDOWN_TYPE
{
    E_SHUTDOWN_TYPE_NORMAL = 0x1, /**< normal shutdown */
    E_SHUTDOWN_TYPE_FAST = 0x2 /**< fast shutdown */
};

class ShutdownManager
{
public:
    virtual ~ShutdownManager();

    void HandleIVIShutdown(E_SHUTDOWN_TYPE type);
    void ShutdownComplete(AMPID proc);

    bool IsShutdown();
    bool IsShuttingDown();

    static ShutdownManager* GetInstance();

private:
    // for singleton pattern
    ShutdownManager();

private:
    bool isShutdown;
    bool isShuttingDown;
    std::unordered_set<AMPID> processesToBeShutdown;
    static std::string dpendentUnitName;
};

#endif // SHUTDOWNNOTIFIER_H
