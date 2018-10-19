/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TaskHandlerManager.cpp
/// @brief contains 
/// 
/// Created by zs on 2018/03/15.
///

/// this file contains the implementation of class TaskHandlerManager
///

#include <log.h>
#include <TASK/TaskDispatcher.h>
#include "TASK/TaskHandlerManager.h"

extern LOG_DECLARE_CONTEXT(TASK);
void InitializeLogContext();
void DestroyLogContext();

std::shared_ptr<TaskHandlerManager> TaskHandlerManager::globalObj = nullptr;

TaskHandlerManager::TaskHandlerManager()
{
    InitializeLogContext();
    DomainVerboseLog chatter(TASK, "TaskHandlerManager::TaskHandlerManager()");
}

TaskHandlerManager::~TaskHandlerManager()
{
    DomainVerboseLog chatter(TASK, "TaskHandlerManager::~TaskHandlerManager()");

    std::unique_lock<std::mutex> lock(mutex4Members);

    for(auto& name: bootSequence)
        _StopHandler(name);
}

void TaskHandlerManager::_StartHandler(const std::string &name, std::shared_ptr<TaskHandler> handler)
{
    DomainVerboseLog chatter(TASK, formatText("TaskHandlerManager::StartHandler(%s)", name.c_str()));

    if(!handler)
    {
        logWarn(TASK, "TaskHandlerManager::StartHandler(): handler is null!");
        return;
    }

    handler->Start();
    logInfo(TASK, "TaskHandlerManager::StartHandler(): [", name, "] handler is started.");
    TaskDispatcher::GetInstance()->RegisterTaskHandler(handler);

    handlers.insert(std::make_pair(name, handler));
    bootSequence.push_back(name);
}

void TaskHandlerManager::StartHandler(const std::string &name, std::shared_ptr<TaskHandler> handler)
{
    DomainVerboseLog chatter(TASK, formatText("TaskHandlerManager::StartHandler(%s)", name.c_str()));

    std::unique_lock<std::mutex> lock(mutex4Members);

    _StartHandler(name, handler);
}

void TaskHandlerManager::_StopHandler(const std::string &name)
{
    DomainVerboseLog chatter(TASK, formatText("TaskHandlerManager::_StopHandler(%s)", name.c_str()));

    if(handlers.find(name)==handlers.end())
    {
        logWarn(TASK, "TaskHandlerManager::StopHandler(): unknown handler name: ", name);
        return;
    }

    TaskDispatcher::GetInstance()->UnregisterTaskHandler(handlers[name]);
    handlers[name]->End();
    logInfo(TASK, "TaskHandlerManager::StopHandler(): [", name, "] handler is stopped.");

    handlers.erase(name);
    bootSequence.erase(std::remove(bootSequence.begin(), bootSequence.end(), name), bootSequence.end());
}

void TaskHandlerManager::StopHandler(const std::string &name)
{
    DomainVerboseLog chatter(TASK, formatText("TaskHandlerManager::StopHandler(%s)", name.c_str()));

    std::unique_lock<std::mutex> lock(mutex4Members);

    _StopHandler(name);
}

void TaskHandlerManager::RestartHandler(const std::string& name, std::function<std::shared_ptr<TaskHandler>(void)> handlerGen)
{
    DomainVerboseLog chatter(TASK, formatText("TaskHandlerManager::RestartHandler(%s)", name.c_str()));

    std::unique_lock<std::mutex> lock(mutex4Members);

    if(handlers.find(name)==handlers.end())
    {
        logWarn(TASK, "TaskHandlerManager::RestartHandler(): unknown handler name: ", name);
        return;
    }

    if(handlerGen==nullptr)
    {
        logWarn(TASK, "TaskHandlerManager::RestartHandler(): null handler generator!");
        return;
    }

    TaskDispatcher::GetInstance()->UnregisterTaskHandler(handlers[name]);
    handlers[name]->End();
    logInfo(TASK, "TaskHandlerManager::RestartHandler(): [", name, "] handler is stopped.");
    handlers.erase(name);

    auto handler = handlerGen();
    handler->Start();
    logInfo(TASK, "TaskHandlerManager::RestartHandler(): [", name, "] handler is started.");
    TaskDispatcher::GetInstance()->RegisterTaskHandler(handler);
    handlers[name] = handler;
}

void TaskHandlerManager::RegisterGlobalObjet(std::shared_ptr<TaskHandlerManager> obj)
{
    if(globalObj && obj)
        logError(TASK, "TaskHandlerManager::RegisterGlobalObjet(): current global object is being overwriten!");

    if(globalObj && obj==nullptr)
        logInfo(TASK, "TaskHandlerManager::RegisterGlobalObjet(): global object is being desroyed.");

    globalObj = obj;
}

std::shared_ptr<TaskHandlerManager> TaskHandlerManager::GetInstance()
{
    return globalObj;
}
