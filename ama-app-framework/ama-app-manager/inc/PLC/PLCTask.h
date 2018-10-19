/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PLCTASK_H
#define PLCTASK_H

#include <cassert>
#include <string>

#include "Task.h"
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "IntentInfo.h"
#include "AppPKGInfo.h"

enum PLC_CMD
{
    PLC_CMD_START = 0,
    PLC_CMD_STOP,
    PLC_CMD_SHOW,
    PLC_CMD_HIDE,
    PLC_CMD_KILL,
    PLC_CMD_INTENT,                 // used to drive an app to do sth for you
    PLC_CMD_NESTED_INTENT,          // start a mocking app
    PLC_CMD_REMOVE_APP_FROM_ZORDER, // remove app from zorder if current app is . if current app is visible, hide it.
    PLC_CMD_START_AUTO_BOOT_PROCESSES,
    PLC_CMD_RELEASE_FAST_BOOT_MANAGER,
    PLC_CMD_START_SERVICE,          // start a raw service
    PLC_CMD_UPDATE_APPLIST,

    PLC_NOTIFY_MODULE_STARTUP,      // means PLC module is started. this will always be the first task PLC received.

    PLC_NOTIFY_START_OK,
    PLC_NOTIFY_START_NG,
    PLC_NOTIFY_STOP_OK,
    PLC_NOTIFY_STOP_NG,
    PLC_NOTIFY_APPCRUSH,
    PLC_NOTIFY_IVI_SHUTDOWN,
    PLC_NOTIFY_APP_SHUTDOWN_OK,
    PLC_NOTIFY_APP_BOOT_TIMEOUT,
    PLC_NOTIFY_FAST_BOOT_APP_STARTED,
    PLC_SET_LAST_MODE,

    PLC_QUERY_APP_INFO_LIST,     // get app info list
    PLC_QUERY_IS_MODULE_INITIALIZED,// returns immediately if module initialization is over.
};

class PLCTask
        : public Task
{
public:
    PLCTask():ampid(INVALID_AMPID){}
    ~PLCTask(){}

    void SetAMPID(AMPID app)
    {
        ampid = app;
    }

    AMPID GetAMPID()
    {
        return ampid;
    }

    virtual std::string GetTaskTypeString()
    {
        switch(GetType()){
        case PLC_CMD_START:return "PLC_CMD_START";
        case PLC_CMD_STOP:return "PLC_CMD_STOP";
        case PLC_CMD_SHOW:return "PLC_CMD_SHOW";
        case PLC_CMD_HIDE:return "PLC_CMD_HIDE";
        case PLC_CMD_KILL:return "PLC_CMD_KILL";
        case PLC_CMD_INTENT:return "PLC_CMD_INTENT";
        case PLC_CMD_NESTED_INTENT:return "PLC_CMD_NESTED_INTENT";
        case PLC_CMD_REMOVE_APP_FROM_ZORDER:return "PLC_CMD_REMOVE_APP_FROM_ZORDER";
        case PLC_CMD_START_AUTO_BOOT_PROCESSES:return "PLC_CMD_START_AUTO_BOOT_PROCESSES";
        case PLC_CMD_RELEASE_FAST_BOOT_MANAGER:return "PLC_CMD_RELEASE_FAST_BOOT_MANAGER";
        case PLC_CMD_START_SERVICE:return "PLC_CMD_START_SERVICE";
        case PLC_CMD_UPDATE_APPLIST:return "PLC_CMD_UPDATE_APPLIST";
        case PLC_NOTIFY_MODULE_STARTUP:return "PLC_NOTIFY_MODULE_STARTUP";
        case PLC_NOTIFY_START_OK:return "PLC_NOTIFY_START_OK";
        case PLC_NOTIFY_START_NG:return "PLC_NOTIFY_START_NG";
        case PLC_NOTIFY_STOP_OK:return "PLC_NOTIFY_STOP_OK";
        case PLC_NOTIFY_STOP_NG:return "PLC_NOTIFY_STOP_NG";
        case PLC_NOTIFY_APPCRUSH:return "PLC_NOTIFY_APPCRUSH";
        case PLC_NOTIFY_IVI_SHUTDOWN:return "PLC_NOTIFY_IVI_SHUTDOWN";
        case PLC_NOTIFY_APP_SHUTDOWN_OK:return "PLC_NOTIFY_APP_SHUTDOWN_OK";
        case PLC_NOTIFY_APP_BOOT_TIMEOUT:return "PLC_NOTIFY_APP_BOOT_TIMEOUT";
        case PLC_NOTIFY_FAST_BOOT_APP_STARTED:return "PLC_NOTIFY_FAST_BOOT_APP_STARTED";
        case PLC_SET_LAST_MODE: return "PLC_SET_LAST_MODE";
        case PLC_QUERY_APP_INFO_LIST:return "PLC_QUERY_APP_INFO_LIST";
        case PLC_QUERY_IS_MODULE_INITIALIZED:return "PLC_QUERY_IS_MODULE_INITIALIZED";
        default: return "ERROR TASK TYPE";
        }
    }

private:
    AMPID ampid;
};

std::shared_ptr<PLCTask> MakeIntentTask(const AMPID app, const IntentInfo &intentInfo);
void DispatchIntentTask(const AMPID app, const IntentInfo &intentInfo);
void DispatchStartServiceTask(const std::string& unitName);
void DispatchStartServiceTask(std::string&& unitName);


#endif // PLCTASK_H
