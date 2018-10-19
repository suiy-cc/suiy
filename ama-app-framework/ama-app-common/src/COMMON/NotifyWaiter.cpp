/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file NotifyWaiter.cpp
/// @brief contains definition of functions used to do asynchronise operation
///
/// Created by zs on 2016/9/13.
///
///

#include <mutex>
#include <condition_variable>
#include <unordered_map>

#include "COMMON/NotifyWaiter.h"
#include "log.h"

LOG_DECLARE_CONTEXT(NWLC)

enum E_SYNCHRONISE_STATE{
    E_SYNCHRONISE_STATE_NONE,
    E_SYNCHRONISE_STATE_WAITING,
    E_SYNCHRONISE_STATE_RELEASED,
};

struct EventInfo
{
    E_SYNCHRONISE_STATE state;
    std::shared_ptr<std::mutex> mutex4State;
    std::shared_ptr<std::condition_variable> cv;
};

static std::mutex mutex4EventRecords;
static std::unordered_map<unsigned long long, EventInfo> eventRecords;

void RegisterLogContext()
{
    static bool isInitialized = false;

    if(!isInitialized)
    {
        RegisterContext(NWLC, "NWLC");
        isInitialized = true;
    }
}

extern "C" void RegisterWaiterID(unsigned long long id)
{
    RegisterLogContext();
    DomainVerboseLog chatter(NWLC, formatText(__STR_FUNCTION__+"(%#x)", id));

    // always lock the map at first.
    std::unique_lock<std::mutex> mapLock(mutex4EventRecords);

    // create mutex for waiting-flag
    auto mutex = std::make_shared<std::mutex>();

    // prepare EventInfo
    EventInfo ei;
    ei.mutex4State = mutex;
    ei.state = E_SYNCHRONISE_STATE_NONE;
    ei.cv = std::make_shared<std::condition_variable>();

    // record EventInfo
    eventRecords.insert(std::make_pair(id, ei));
}

extern "C" void UnregisterWaiterID(unsigned long long id)
{
    DomainVerboseLog chatter(NWLC, formatText(__STR_FUNCTION__+"(%#x)", id));

    // always lock the map at first.
    std::unique_lock<std::mutex> mapLock(mutex4EventRecords);

    // look for specified record
    auto iter = eventRecords.find(id);

    // delete record
    if(iter!=eventRecords.end())
    {
        eventRecords.erase(iter);
    }
}

extern "C" bool Wait(unsigned long long id, unsigned int timeout_ms)
{
    DomainVerboseLog chatter(NWLC, formatText(__STR_FUNCTION__+"(%#x, %d)", id, timeout_ms));

    // always lock the map at first.
    std::unique_lock<std::mutex> mapLock(mutex4EventRecords);

    // look for specified record
    EventInfo ei;
    auto iter = eventRecords.find(id);
    if(iter==eventRecords.end())
    {
        logWarn(NWLC, "Wait(", ToHEX(id), "): record not found!");
        return false;
    }
    else
        ei = iter->second;

    // suspend current thread
    std::unique_lock<std::mutex> mutexLock(*ei.mutex4State);
    if(ei.state==E_SYNCHRONISE_STATE_NONE
        || ei.state==E_SYNCHRONISE_STATE_WAITING)
    {
        ei.state = E_SYNCHRONISE_STATE_WAITING;
        iter->second = ei;
        mapLock.unlock();
        if(timeout_ms==0)
        {
            ei.cv->wait(mutexLock);
            return true;
        }
        else
        {
            auto timeout = ei.cv->wait_for(mutexLock, std::chrono::milliseconds(timeout_ms));
            return timeout==std::cv_status::no_timeout;
        }
    }
    else if(ei.state==E_SYNCHRONISE_STATE_RELEASED)
    {
        ei.state = E_SYNCHRONISE_STATE_NONE;
        iter->second = ei;
        return true;
    }
    else
    {
        return true;
    }
}

extern "C" void Notify(unsigned long long id)
{
    DomainVerboseLog chatter(NWLC, formatText(__STR_FUNCTION__+"(%#x)", id));

    // always lock the map at first.
    std::unique_lock<std::mutex> mapLock(mutex4EventRecords);

    // look for specified record
    EventInfo ei;
    auto iter = eventRecords.find(id);
    if(iter==eventRecords.end())
    {
        logWarn(NWLC, "Notify(", ToHEX(id), "): record not found!");
        return;
    }
    else
        ei = iter->second;

    std::unique_lock<std::mutex> mutexLock(*ei.mutex4State);
    if(ei.state==E_SYNCHRONISE_STATE_WAITING)
    {
        // wake up suspended thread
        ei.cv->notify_all();
        ei.state = E_SYNCHRONISE_STATE_RELEASED;

        // store the data
        iter->second = ei;

        logInfo(NWLC, "Notify(", ToHEX(id), "): wait -> release");
    }
    else if(ei.state==E_SYNCHRONISE_STATE_NONE)
    {
        ei.state = E_SYNCHRONISE_STATE_RELEASED;

        // store the data
        iter->second = ei;

        logInfo(NWLC, "Notify(", ToHEX(id), "): none -> release");
    }
    else
    {
        logWarn(NWLC, "Notify(", ToHEX(id), "): release -> release");
    }
}
