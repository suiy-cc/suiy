/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HMICTask.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/05/15.
///

#include "HMICTask.h"
#include "HMICSpy.h"

std::vector<E_SEAT> GetAvailableScreens(bool wait4ever/* = false*/)
{
    if(HMICSpy::isScreenStatusReady)
    {
        logInfo(HMIC, LOG_HEAD, "use HMIC spy.");
        return HMICSpy::screens;
    }

    auto hmicTask = GetScreenQueryTask();
    std::vector<E_SEAT>* screens = reinterpret_cast<std::vector<E_SEAT>*>(hmicTask->GetUserData());

    if(!SendQueryTaskNWait(hmicTask, wait4ever?0:300))
        logWarn(HMIC, "GetAvailableScreens(): query timeout!");
    else
        logInfo(HMIC, "GetAvailableScreens(): screen count = ", screens->size());

    return std::move(*screens);
}

void GetAvailableScreens(std::function<void(std::vector<E_SEAT>)> replyer)
{
    if(HMICSpy::isScreenStatusReady)
    {
        logInfo(HMIC, LOG_HEAD, "use HMIC spy.");
        replyer(HMICSpy::screens);
        return;
    }

    auto hmicTask = GetScreenQueryTask();

    SendQueryTaskNReplyAtTaskDone(hmicTask, [replyer](std::shared_ptr<Task> task){
        auto hmicTask = dynamic_pointer_cast<HMICTask>(task);
        if(!hmicTask)
            replyer(std::vector<E_SEAT>());
        std::vector<E_SEAT>* screens = reinterpret_cast<std::vector<E_SEAT>*>(hmicTask->GetUserData());
        if(screens)
            replyer(std::move(*screens));
        else
            replyer(std::vector<E_SEAT>());
    });
}
