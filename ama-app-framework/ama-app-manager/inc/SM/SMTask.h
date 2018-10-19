/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef SMTASK_H
#define SMTASK_H

#include <cassert>
#include <string>

#include "Task.h"
#include "AMGRLogContext.h"
#include "AppUI.h"

enum E_SM_TASK{
    E_SM_TASK_INIT,
    E_SM_TASK_QUERY_SNAPSHOT_SM,

    E_SM_TASK_CMD_BROADCAST_SET_REQUEST,
    E_SM_TASK_CMD_BROADCAST_SET_REPLY,

    E_SM_TASK_REQUEST_BROADCAST,
};

class SMTask
: public Task
{
public:
    SMTask(){}
    ~SMTask(){}

	virtual string GetTaskTypeString()
    {
        switch(GetType())
        {
            case E_SM_TASK_INIT: return "E_SM_TASK_INIT";
            case E_SM_TASK_QUERY_SNAPSHOT_SM: return "E_SM_TASK_QUERY_SM_SNAPSHOT";
            case E_SM_TASK_CMD_BROADCAST_SET_REQUEST: return"E_SM_TASK_CMD_BROADCAST_SET_REQUEST";
            case E_SM_TASK_CMD_BROADCAST_SET_REPLY: return"E_SM_TASK_CMD_BROADCAST_SET_REPLY";
            case E_SM_TASK_REQUEST_BROADCAST: return"E_SM_TASK_REQUEST_BROADCAST";
            default: return "error task type or forget to update task type table.";
        }
    }

private:

};

void sendInitTaskToSM(void);

#endif // SMTASK_H
