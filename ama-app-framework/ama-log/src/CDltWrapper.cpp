/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "CDltWrapper.h"

#include <dlt.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <mutex>

//Be sure to put it at the end of statement
#include "ama_mem.h"

CDltWrapper* CDltWrapper::mpDLTWrapper = NULL;
pthread_mutex_t CDltWrapper::mMutex = PTHREAD_MUTEX_INITIALIZER;

CDltWrapper *CDltWrapper::instance(const bool enableNoDLTDebug)
{
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static CDltWrapper wrapper(enableNoDLTDebug);

    return &wrapper;
}

static std::string CDltWrapperappID;
CDltWrapper::CDltWrapper(const bool isOutputLog2File) :
        mDltContext(), //
        mUIContext(),
        mDltContextData()
{
//    char* logPath = nullptr;
//    if(!isOutputLog2File)
//    {
//        std::string logPathStr;
//        std::ifstream logDirFile("/etc/ama.d/logDir.txt");
//        logDirFile >> logPathStr;
//        logDirFile.close();
//        logPathStr += CDltWrapperappID;
//        logPathStr += ".log";
//        logPath = new char[logPathStr.length()+1];
//        strcpy(logPath, logPathStr.c_str());
//    }

//    DltReturnValue drv = DLT_RETURN_OK;

//    if(logPath)
//        drv = dlt_init_file(logPath);

//    if(drv!=DLT_RETURN_OK && drv!=DLT_RETURN_TRUE)
//    {
//        char logFileOpenError[1024];
//        sprintf(logFileOpenError, "/tmp/dlt_wrapper_init_error_pid%d.log", getpid());
//        FILE* log = fopen(logFileOpenError, "at");
//        fprintf(log, "dlt init failure! log file's path = %s, pointer = %lu, errorno = %d\n", logPath, logPath, drv);
//        fclose(log);
//    }
}

void CDltWrapper::registerApp(const char *appid, const char *description)
{
    // new register will replace old ones
    dlt_unregister_app();

    // register specified appID
    dlt_register_app(appid, description);

    //register a default context
    registerContext(mDltContext, "def", "default Context registered by CDltWrapper class");

    //register a ui context
    registerContext(mUIContext, "ui", "a UI Context registered by CDltWrapper class");
}

void CDltWrapper::registerContext(LogContext& handle, const char *contextid, const char *description)
{
    pthread_mutex_lock(&mMutex);

    dlt_register_context(static_cast<DltContext*>(handle.GetDltContext()), contextid, description);
    handle.SetRegistered(true);

    pthread_mutex_unlock(&mMutex);
}

void CDltWrapper::unregisterContext(LogContext & handle)
{
    pthread_mutex_lock(&mMutex);

    handle.SetRegistered(false);
    dlt_unregister_context(static_cast<DltContext*>(handle.GetDltContext()));

    pthread_mutex_unlock(&mMutex);
}

bool CDltWrapper::init(LogLevelType loglevel, LogContext *context)
{
    (void) loglevel;
    pthread_mutex_lock(&mMutex);
    if (!context)
        context = &mDltContext;
    if(!context->IsRegistered())
        context = &mDltContext;
    bool isSuccess =  dlt_user_log_write_start(static_cast<DltContext*>(context->GetDltContext()), static_cast<DltContextData*>(mDltContextData.GetDltContextData()), static_cast<DltLogLevelType>(loglevel)) > 0;
    if(!isSuccess)
        pthread_mutex_unlock(&mMutex);
    return isSuccess;
}

bool CDltWrapper::initUI(LogLevelType loglevel)
{
    return init(loglevel, &mUIContext);
}

void CDltWrapper::send()
{
    dlt_user_log_write_finish(static_cast<DltContextData*>(mDltContextData.GetDltContextData()));

    pthread_mutex_unlock(&mMutex);
}

void CDltWrapper::append(const int8_t value)
{
    dlt_user_log_write_int8(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);

}

void CDltWrapper::append(const uint8_t value)
{
    dlt_user_log_write_uint8(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const int16_t value)
{
    dlt_user_log_write_int16(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const int32_t value)
{
    dlt_user_log_write_int32(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const uint16_t value)
{
    dlt_user_log_write_uint16(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const uint32_t value)
{
    dlt_user_log_write_uint32(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const char*& value)
{
    dlt_user_log_write_string(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const std::string& value)
{
    try
    {
        dlt_user_log_write_string(static_cast<DltContextData *>(mDltContextData.GetDltContextData()), value.c_str());
    }
    catch(std::exception& e)
    {
        std::string errorReport = " [CDltWrapper::append(): exception is caught: ";
        errorReport += (&e)->what();
        errorReport += "] ";
        dlt_user_log_write_string(static_cast<DltContextData *>(mDltContextData.GetDltContextData()), errorReport.c_str());
    }
    catch(...)
    {
        std::string errorReport = " [CDltWrapper::append(): unexpected exception is caught!] ";
        dlt_user_log_write_string(static_cast<DltContextData *>(mDltContextData.GetDltContextData()), errorReport.c_str());
    }
}

void CDltWrapper::append(const bool value)
{
    dlt_user_log_write_bool(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), static_cast<uint8_t>(value));
}

void CDltWrapper::append(const int64_t value)
{
    dlt_user_log_write_int64(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const uint64_t value)
{
    dlt_user_log_write_uint64(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const unsigned long long value)
{
    dlt_user_log_write_uint64(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const float value)
{
    dlt_user_log_write_float32(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

void CDltWrapper::append(const double value)
{
    dlt_user_log_write_float64(static_cast<DltContextData*>(mDltContextData.GetDltContextData()), value);
}

CDltWrapper::~CDltWrapper()
{
    unregisterContext(mDltContext);
    unregisterContext(mUIContext);
    dlt_unregister_app();
    dlt_free();
}

void LogInitialize(const char *appID, const char *description, const bool isEnableLocalPrint, const bool isUseFile)
{
    if(isEnableLocalPrint){
        //DLT_ENABLE_LOCAL_PRINT();
        dlt_enable_local_print();
    }

    //dlt_set_application_ll_ts_limit();
    dlt_verbose_mode();

    CDltWrapperappID = appID;
    CDltWrapper::instance(isEnableLocalPrint)->registerApp(appID,description);

}

void LogInitialize(const char *appID, const char *description, const bool isEnableLocalPrint)
{
    LogInitialize(appID, description, isEnableLocalPrint, true);
}

void LogInitialize(const char *appID, const bool isEnableLocalPrint)
{
    LogInitialize(appID, appID, isEnableLocalPrint);
}

void RegisterContext(LogContext &handle, const char *contextid, const char *description)
{
    getWrapper()->registerContext(handle,contextid,description);
}

void RegisterContext(LogContext &handle, const char *contextid)
{
    RegisterContext(handle,contextid,contextid);
}

void UnregisterContext(LogContext &handle)
{
    getWrapper()->unregisterContext(handle);
}

void dltEnableVerboseMode(void)
{
    dlt_verbose_mode();
}

void dltDisableVerboseMode(void)
{
    dlt_nonverbose_mode();
}

LogContext::LogContext()
: isRegistered(false)
{
    dltContext = std::shared_ptr<void>(new DltContext, [](void* pVoid){delete static_cast<DltContext*>(pVoid);});
}

LogContext::~LogContext()
{
}

void* LogContext::GetDltContext()
{
    return dltContext.get();
}

void LogContext::SetRegistered(bool isRegistered)
{
    this->isRegistered = isRegistered;
}

bool LogContext::IsRegistered()const
{
    return isRegistered;
}

LogContextData::LogContextData()
{
    dltContextData = std::shared_ptr<void>(new DltContextData, [](void* pVoid){delete static_cast<DltContextData*>(pVoid);});
}

LogContextData::~LogContextData()
{
}

void* LogContextData::GetDltContextData()
{
    return dltContextData.get();
}
