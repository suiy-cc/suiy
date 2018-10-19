/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef LOG_H
#define LOG_H

#include "CDltWrapper.h"

#include <sstream>
#include <cstdarg>
#include <cstring>
#include <algorithm>

#include "ama_mem.h"

// DEPRECATED!!!
void LogInitialize(const char *appID, const bool isEnableLocalPrint);

void LogInitialize(const char *appID, const char *description, const bool isEnableLocalPrint);

void LogInitialize(const char *appID, const char *description, const bool isEnableLocalPrint, const bool isUseFile);

void RegisterContext(LogContext &handle, const char *contextid); // DEPRECATED!!!

void RegisterContext(LogContext &handle, const char *contextid, const char *description);

void UnregisterContext(LogContext &handle);

static std::string ToHEX(unsigned long long ULL, bool isShowBase = true)
{
    std::string HEX;
    std::stringstream ULL2HEX;
    if(isShowBase)
        ULL2HEX.flags(std::ios::hex | std::ios::showbase);
    else
        ULL2HEX.flags(std::ios::hex);
    ULL2HEX << ULL;
    ULL2HEX >> HEX;

    return std::move(HEX);
}

static std::string _CutParenthesesNTail(std::string&& prettyFuncon)
{
    auto pos = prettyFuncon.find('(');
    if(pos!=std::string::npos)
        prettyFuncon.erase(prettyFuncon.begin()+pos, prettyFuncon.end());

    return std::move(prettyFuncon);
}

#define __STR_FUNCTION__ _CutParenthesesNTail(std::string(__PRETTY_FUNCTION__))

// means function name + parentheses (P = parentheses)
#define __STR_FUNCTIONP__ __STR_FUNCTION__+"()"

// means function name + parentheses + colon (C = colon)
#define __STR_FUNCTIONPC__ __STR_FUNCTION__+"(): "

// means the head of one piece of log.
#define LOG_HEAD __STR_FUNCTIONPC__

#define FUNCTION_SCOPE_LOG_C(context) DomainVerboseLog chatter(context, __STR_FUNCTIONP__)
#define FUNCTION_SCOPE_LOG DomainVerboseLog chatter(__STR_FUNCTIONP__)

template<typename T>
static void append(CDltWrapper* inst, T arg)
{
    if(inst)
        inst->append(arg);
}

template<typename T, typename ...ARGS>
static void append(CDltWrapper* inst, T argi, ARGS... args)
{
    if(inst)
        inst->append(argi);

    append(inst, args...);
}

/**
 * logs a given value with a given context (register first!) and given loglevel
 * @param context
 * @param loglevel
 * @param value
 */
template<typename ...ARGS>
static void log(LogContext* const context, LogLevelType loglevel, ARGS... args)
{
    CDltWrapper* inst(getWrapper());

    if(inst->init(loglevel, context))
    {
        append(inst, args...);
        inst->send();
    }
}

/**
 * logs a given value with a given context (register first!) and given loglevel
 * @param context
 * @param loglevel
 * @param value
 */
template<typename ...ARGS>
static void logUI(LogLevelType loglevel, ARGS... args)
{
    CDltWrapper* inst(getWrapper());

    if(inst->initUI(loglevel))
    {
        append(inst, args...);
        inst->send();
    }
}

/**************************|=========================|*****************************
***************************|      logError Start     |*****************************
***************************|=========================|****************************/
template<typename ...ARGS>
static void logError(ARGS... args)
{
    log(NULL,LOG_ERRO,args...);
}

template<typename ...ARGS>
static void logError(LogContext context,ARGS... args)
{
    log(&context,LOG_ERRO,args...);
}

template<typename ...ARGS>
static void logErrorUI(ARGS... args)
{
    logUI(LOG_ERRO,args...);
}

/**************************|=========================|*****************************
***************************|      logError End       |*****************************
***************************|=========================|****************************/



/**************************|=========================|*****************************
***************************|      logWarn Start      |*****************************
***************************|=========================|****************************/
template<typename ...ARGS>
static void logWarn(ARGS... args)
{
    log(NULL,LOG_WARN,args...);
}

template<typename ...ARGS>
static void logWarn(LogContext context,ARGS... args)
{
    log(&context,LOG_WARN,args...);
}

template<typename ...ARGS>
static void logWarnUI(ARGS... args)
{
    logUI(LOG_WARN,args...);
}

/**************************|=========================|*****************************
***************************|      logWarn End        |*****************************
***************************|=========================|****************************/

/**************************|=========================|*****************************
***************************|      logInfo Start      |*****************************
***************************|=========================|****************************/
template<typename ...ARGS>
static void logInfo(ARGS ...args)
{
    log(NULL,LOG_INFM,args...);
}

template<typename ...ARGS>
static void logInfo(LogContext context,ARGS... args)
{
    log(&context,LOG_INFM,args...);
}

template<typename ...ARGS>
static void logInfoUI(ARGS... args)
{
    logUI(LOG_INFM,args...);
}

/**************************|=========================|*****************************
***************************|      logInfo End        |*****************************
***************************|=========================|****************************/

/**************************|=========================|*****************************
***************************|      logDebug Start     |*****************************
***************************|=========================|****************************/
template<typename ...ARGS>
static void logDebug(ARGS... args)
{
    log(NULL,LOG_DEBG,args...);
}

template<typename ...ARGS>
static void logDebug(LogContext context,ARGS... args)
{
    log(&context,LOG_DEBG,args...);
}

template<typename ...ARGS>
static void logDebugUI(ARGS... args)
{
    logUI(LOG_DEBG,args...);
}

/**************************|=========================|*****************************
***************************|      logDebug End       |*****************************
***************************|=========================|****************************/

/**************************|=========================|*****************************
***************************|      logVerbose Start   |*****************************
***************************|=========================|****************************/
template<typename ...ARGS>
static void logVerbose(ARGS... args)
{
    log(NULL,LOG_VBOS,args...);
}

template<typename ...ARGS>
static void logVerbose(LogContext context,ARGS... args)
{
    log(&context,LOG_VBOS,args...);
}

template<typename ...ARGS>
static void logVerboseUI(ARGS... args)
{
    logUI(LOG_VBOS,args...);
}

/**************************|=========================|*****************************
***************************|      logVerbose End     |*****************************
***************************|=========================|****************************/

/**************************|=========================|*****************************
***************************|      logF Begin         |*****************************
***************************|=========================|****************************/

static std::string _formatText(const char *format, va_list argList)
{
    const char* errorReport = "_formatText(): failed invoking vsnprintf(), errno = %d";

    // allocate memory
    unsigned long minCapacity = strlen(errorReport)+2; // string + \0 + errno
    unsigned long capacity = strlen(format)*4+2;
    capacity = std::max(minCapacity, capacity);
    char *tmpString = new char[capacity];

    // make string
    va_list argListBackup;
    va_copy(argListBackup, argList);
    int size = vsnprintf(tmpString, capacity, format, argList);
    if(capacity <= size && size >= 0)
    {
        capacity = size+1;
        delete[] tmpString;
        tmpString = new char[capacity];
        size = vsnprintf(tmpString, capacity, format, argListBackup);
    }
    va_end(argListBackup);

    // error handling
    if(size < 0)
        snprintf(tmpString, capacity, errorReport, errno);

    // free memory
    std::string returnString = tmpString;
    delete[] tmpString;

    return std::move(returnString);
}

static std::string formatText(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    std::string returnString = _formatText(format, ap);
    va_end(ap);

    return std::move(returnString);
}

static std::string formatText(const std::string format, ...)
{
    va_list ap;
    va_start(ap, format);
    std::string returnString = _formatText(format.c_str(), ap);
    va_end(ap);

    return std::move(returnString);
}

static void _logF(LogContext* const context, LogLevelType loglevel, const char *format, va_list argList)
{
    log(context, loglevel, _formatText(format, argList));
}

static void logF(LogContext* const context, LogLevelType loglevel, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(context, loglevel,format, ap);
    va_end(ap);
}

static void _logUIF(LogLevelType loglevel, const char *format, va_list argList)
{
    logUI(loglevel, _formatText(format, argList));
}

static void logUIF(LogLevelType loglevel, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logUIF(loglevel, format, ap);
    va_end(ap);
}

static void logErrorF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(nullptr, LOG_ERRO, format, ap);
    va_end(ap);
}

static void logErrorF(LogContext context, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(&context, LOG_ERRO, format, ap);
    va_end(ap);
}

static void logErrorUIF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logUIF(LOG_ERRO, format, ap);
    va_end(ap);
}

static void logWarnF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(nullptr, LOG_WARN, format, ap);
    va_end(ap);
}

static void logWarnF(LogContext context, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(&context, LOG_WARN, format, ap);
    va_end(ap);
}

static void logWarnUIF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logUIF(LOG_WARN, format, ap);
    va_end(ap);
}

static void logInfoF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(nullptr, LOG_INFM, format, ap);
    va_end(ap);
}

static void logInfoF(LogContext context, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(&context, LOG_INFM, format, ap);
    va_end(ap);
}

static void logInfoUIF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logUIF(LOG_INFM, format, ap);
    va_end(ap);
}

static void logDebugF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(nullptr, LOG_DEBG, format, ap);
    va_end(ap);
}

static void logDebugF(LogContext context, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(&context, LOG_DEBG, format, ap);
    va_end(ap);
}

static void logDebugUIF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logUIF(LOG_DEBG, format, ap);
    va_end(ap);
}

static void logVerboseF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(nullptr, LOG_VBOS, format, ap);
    va_end(ap);
}

static void logVerboseF(LogContext context, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logF(&context, LOG_VBOS, format, ap);
    va_end(ap);
}

static void logVerboseUIF(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    _logUIF(LOG_VBOS, format, ap);
    va_end(ap);
}

/**************************|=========================|*****************************
***************************|      logF End           |*****************************
***************************|=========================|****************************/
class DomainLog
{
    std::string domainName;
    LogContext* lc;
    LogLevelType logLevel;
public:
    DomainLog(const LogContext& context, LogLevelType logLevel, const std::string& name)
        : domainName(name)
        , logLevel(logLevel)
    {
        lc = new LogContext();
        *lc = context;
        log(lc, logLevel, domainName, "--->IN");
    };
    DomainLog(LogLevelType logLevel, const std::string& name)
        : domainName(name)
        , logLevel(logLevel)
        , lc(nullptr)
    {
        log(lc, logLevel, domainName, "--->IN");
    };
    virtual ~DomainLog()
    {
        log(lc, logLevel, domainName, "--->OUT");
        delete lc;
        lc = nullptr;
    }
};

class DomainLogUI{
    DomainLog domainLog;
public:
    DomainLogUI(LogLevelType logLevel, const std::string& name)
        : domainLog(*getWrapper()->getUILogContext(), logLevel, name)
    {};
    virtual ~DomainLogUI(){};
};

class DomainVerboseLog
{
    DomainLog domainLog;
public:
    DomainVerboseLog(const LogContext& context, const std::string& name)
        : domainLog(context, LOG_VBOS, name)
    {};
    DomainVerboseLog(const std::string& name)
        : domainLog(LOG_VBOS, name)
    {};
    virtual ~DomainVerboseLog(){};
};

class DomainVerboseLogUI{
    DomainLogUI domainLogUI;
public:
    DomainVerboseLogUI(const std::string& name)
    : domainLogUI(LOG_VBOS, name)
    {};
    virtual ~DomainVerboseLogUI(){};
};

#ifdef _DEBUG_MEMORY_
    #undef new
#endif

#endif // LOG_H
