/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SynchronizedTaskHandler.h
/// @brief contains class SynchronizedTaskHandler
/// 
/// Created by zs on 2017/03/11.
///

/// this file contains the definition of class SynchronizedTaskHandler
/// 

#ifndef SYNCHRONIZEDTASKHANDLER_H
#define SYNCHRONIZEDTASKHANDLER_H

#include "STHTask.h"

/// @class SynchronizedTaskHandler
/// @brief this class is used to handle the assembly tasks
/// which needs to wait until its sub-tasks is done.
class SynchronizedTaskHandler
{
public:
    SynchronizedTaskHandler();
    virtual ~SynchronizedTaskHandler();

    void doTask(shared_ptr<Task> task);

    void OnStartScreenShare(shared_ptr<STHTask> task);
    void OnStopScreenShare(shared_ptr<STHTask> task);
};


#endif // SYNCHRONIZEDTASKHANDLER_H
