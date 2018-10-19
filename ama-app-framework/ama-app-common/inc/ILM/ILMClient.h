/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ILMClient.h
/// @brief contains the definition of class ILMClient
///
/// Created by zs on 2016/6/15.
/// this file contains the definition of class ILMClient
///

#ifndef ILMCLIENT_H
#define ILMCLIENT_H

#include <functional>

/// @class ILMClient
/// @brief lifecycle and communication
class ILMClient
{
public:
    static void Initialize();
    static void Destroy();
    static bool IsInitialized();

    /// Execute all enqueued commands since last commit.
    static void Commit();

    /// set callback for ILM-client-fatal-error
    typedef std::function<void(unsigned int)> FatalErrorCB;
    static void SetFatalErrorCB(const FatalErrorCB& f);

    friend class InputPeripheral;

private:
    static bool isInputPeripheralChangedCBRegistered;
};

#endif // ILMCLIENT_H
