/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file STHTask.h
/// @brief contains class STHTask
/// 
/// Created by zs on 2017/03/11.
///

#ifndef STHTASK_H
#define STHTASK_H

#include "Task.h"
#include "ama_types.h"
#include "Capi_StubImpl.h"

enum STHTaskType{
    STH_START_SCREEN_SAHRE,
    STH_STOP_SCREEN_SAHRE,
};

class STHTask
: public Task
{
public:
    virtual string GetTaskTypeString() override
    {
        switch(GetType())
        {
            case STH_START_SCREEN_SAHRE: return "STH_START_SCREEN_SAHRE";
            case STH_STOP_SCREEN_SAHRE: return "STH_STOP_SCREEN_SAHRE";
            default: return "";
        }
    }
    unsigned long long int GetUserData() const
    {
        return userData;
    }
    void SetUserData(unsigned long long int userData)
    {
        STHTask::userData = userData;
    };

private:
    unsigned long long userData;
};

typedef std::tuple<unsigned int, E_SEAT, bool, v0_1::org::neusoft::AppMgrStub::StopShareAppReqReply_t> ScreenShareParameter;
void DispatchScreenShareTask(unsigned int app, E_SEAT seat, bool isMove, STHTaskType taskType, v0_1::org::neusoft::AppMgrStub::StopShareAppReqReply_t replay);

#endif // STHTASK_H
