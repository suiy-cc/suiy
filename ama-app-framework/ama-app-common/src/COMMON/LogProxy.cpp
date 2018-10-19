/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LogProxy.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/08/21.
///

/// this file contains the implementation of class LogProxy
///

#include "COMMON/LogProxy.h"

LogProxy::LogProxy()
{}
LogProxy::~LogProxy()
{

}
void LogProxy::SetVerboseLogCB(const LogFuncType &verboseLogCB)
{
    LogProxy::verboseLogCB = verboseLogCB;
}
void LogProxy::SetInfoLogCB(const LogFuncType &infoLogCB)
{
    LogProxy::infoLogCB = infoLogCB;
}
void LogProxy::SetWarningLogCB(const LogFuncType &warningLogCB)
{
    LogProxy::warningLogCB = warningLogCB;
}
void LogProxy::SetErrorLogCB(const LogFuncType &errorLogCB)
{
    LogProxy::errorLogCB = errorLogCB;
}
void LogProxy::Verbose(const std::string &log)
{
    if(verboseLogCB)
        verboseLogCB(log);
}
void LogProxy::Info(const std::string &log)
{
    if(infoLogCB)
        infoLogCB(log);
}
void LogProxy::Warning(const std::string &log)
{
    if(warningLogCB)
        warningLogCB(log);
}
void LogProxy::Error(const std::string &log)
{
    if(errorLogCB)
        errorLogCB(log);
}
