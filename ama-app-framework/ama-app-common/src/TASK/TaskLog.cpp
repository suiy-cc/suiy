/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskLog.cpp
/// @brief contains log context of task module
/// 
/// Created by zs on 2017/09/08.
///

#include "COMMON/DomainInvoker.h"
#include "log.h"

LOG_DECLARE_CONTEXT(TASK);

static std::shared_ptr<DomainInvoker> lc = nullptr;

void InitializeLogContext()
{
    if(!lc)
        lc = std::make_shared<DomainInvoker>([TASK](){RegisterContext(TASK, "TASK");}
                                             , [TASK](){UnregisterContext(TASK);});
}

void DestroyLogContext()
{
    lc->InvokeDestroyFunction();
}