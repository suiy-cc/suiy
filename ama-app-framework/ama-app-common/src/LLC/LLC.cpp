/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LLC.cpp
/// @brief contains the implementation of class LLC
/// 
/// Created by zs on 2017/03/27.
///
/// this file contains the implementation of class LLC
///

#include "LLC/LLC.h"

LLC::LLC()
{
    // initialize LLC connection
    LLC_Result initError = LLC_Init();
    isInitOK = (initError==LLC_RESULT_SUCCESS);

    // listen to unit state
    LLC_Result listenError = Listen2UnitStateChange();
    isInitOK = (listenError==LLC_RESULT_SUCCESS);
}

LLC::~LLC()
{
    LLC_Result result = LLC_Destroy();
}

static std::mutex mutex;

LLC *LLC::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    std::unique_lock<std::mutex> lock(mutex);

    static LLC instance;
    return &instance;
}

LLC_Result LLC::Start(const std::string &unitName, const char **argv, void *userData)
{
    return LLC_StartUnit(unitName.c_str(), argv, &LLC::startUnitCB, userData);
}

LLC_Result LLC::Stop(const std::string &unitName, void *userData)
{
    return LLC_StopUnit(unitName.c_str(), &LLC::stopUnitCB, userData);
}

LLC_Result LLC::GetState(const std::string &unitName, bool &isActive) const
{
    return LLC_GetUnitStatus(unitName.c_str(), &isActive);
}

LLC_Result LLC::Listen2UnitStateChange()
{
    return LLC_ListenUnitStatus(&LLC::unitStateChangeCB);
}

void LLC::startUnitCB(_Bool result, uint32_t pid, void *userData)
{
    GetInstance()->startObserver.NotifyAll([=](LLC_StartCb callback){
        callback(result, pid, userData);
    });
}

void LLC::stopUnitCB(_Bool result, void *userData)
{
    GetInstance()->stopObserver.NotifyAll([=](LLC_StopCb callback){
        callback(result, userData);
    });
}

void LLC::unitStateChangeCB(const char *unitName, _Bool status)
{
    GetInstance()->statusObserver.NotifyAll([=](LLC_UnitChangedCb callback){
        callback(unitName, status);
    });
}
