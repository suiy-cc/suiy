/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LogProxy.h
/// @brief contains 
/// 
/// Created by zs on 2018/08/21.
///

/// this file contains the definition of class LogProxy
/// 

#ifndef LOGPROXY_H
#define LOGPROXY_H

#include <string>
#include <functional>

typedef std::function<void(std::string)> LogFuncType;

class LogProxy
{
public:
    LogProxy();
    virtual ~LogProxy();

    void SetVerboseLogCB(const LogFuncType &verboseLogCB);
    void SetInfoLogCB(const LogFuncType &infoLogCB);
    void SetWarningLogCB(const LogFuncType &warningLogCB);
    void SetErrorLogCB(const LogFuncType &errorLogCB);

    void Verbose(const std::string& log);
    void Info(const std::string& log);
    void Warning(const std::string& log);
    void Error(const std::string& log);

private:
    LogFuncType verboseLogCB;
    LogFuncType infoLogCB;
    LogFuncType warningLogCB;
    LogFuncType errorLogCB;
};


#endif // LOGPROXY_H
