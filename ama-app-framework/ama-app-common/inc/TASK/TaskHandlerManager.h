/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskHandlerManager.h
/// @brief contains class TaskHandlerManager
/// 
/// Created by zs on 2018/03/15.
///

/// this file contains the definition of class TaskHandlerManager
/// 

#ifndef TASKHANDLERMANAGER_H
#define TASKHANDLERMANAGER_H


#include <memory>
#include <unordered_map>
#include <vector>

#include <TASK/TaskHandler.h>

class TaskHandlerManager
{
public:
    TaskHandlerManager();
    virtual ~TaskHandlerManager();

    void StartHandler(const std::string& name, std::shared_ptr<TaskHandler> handler);
    void StopHandler(const std::string& name);
    void RestartHandler(const std::string& name, std::function<std::shared_ptr<TaskHandler>(void)> handlerGen);

    static void RegisterGlobalObjet(std::shared_ptr<TaskHandlerManager> obj);
    static std::shared_ptr<TaskHandlerManager> GetInstance();

protected:
    void _StartHandler(const std::string& name, std::shared_ptr<TaskHandler> handler);
    void _StopHandler(const std::string& name);

private:
    static std::shared_ptr<TaskHandlerManager> globalObj;
    std::unordered_map<std::string, std::shared_ptr<TaskHandler>> handlers;
    std::vector<std::string> bootSequence;
    std::mutex mutex4Members;
};


#endif // TASKHANDLERMANAGER_H
