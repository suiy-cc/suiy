/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file STHTask.cpp
/// @brief contains class STHTask
///
/// Created by zs on 2017/03/11.
///

#include "STHTask.h"
#include "TaskDispatcher.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

void DispatchScreenShareTask(unsigned int app, E_SEAT seat, bool isMove, STHTaskType taskType, v0_1::org::neusoft::AppMgrStub::StopShareAppReqReply_t replay)
{
    auto parameters = new ScreenShareParameter(app, seat, isMove, replay);

    auto sthTask = make_shared<STHTask>();
    sthTask->SetType(taskType);
    sthTask->SetUserData(reinterpret_cast<unsigned long long>(parameters));
    TaskDispatcher::GetInstance()->Dispatch(sthTask);
}
