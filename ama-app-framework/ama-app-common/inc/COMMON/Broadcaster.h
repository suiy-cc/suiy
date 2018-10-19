/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Broadcaster.h
/// @brief contains class Broadcaster
/// 
/// Created by zs on 2017/03/10.
///
/// this file contains the definition of class Broadcaster
/// 

#ifndef BROADCASTER_H
#define BROADCASTER_H

#include <set>

#include <COMMON/ThreadSafeValueKeeper.h>
#include <COMMON/HandleManagement.h>

template <class T>
class Broadcaster
{
public:
    typedef std::set<T> CallbackSetType;

    void Register(T callback)
    {
        callbackSet.Operate([&](CallbackSetType& cbSet){
            cbSet.insert(callback);
        });
    }
    void Unregister(T callback)
    {
        callbackSet.Operate([&](CallbackSetType& cbSet){
            cbSet.erase(callback);
        });
    }
    bool IsRegistered(T callback)const
    {
        bool isRegistered = false;
        callbackSet.OperateConst([=,&isRegistered](const CallbackSetType& cbSet){
            isRegistered = cbSet.find(callback)!=cbSet.end();
        });

        return isRegistered;
    }
    void NotifyAll(std::function<void(T)> Invoker)
    {
        callbackSet.Operate([&](CallbackSetType& cbSet){
            for(auto& callback: cbSet)
                Invoker(callback);
        });
    }
    unsigned int GetSize()const
    {
        return callbackSet.GetValue().size();
    }

private:
    ThreadSafeValueKeeper<CallbackSetType> callbackSet;
};

template <class T, class ...U>
class Broadcaster<T(U...)>
{
public:
    typedef std::function<T(U...)> CallbackType;
    typedef HandleManager<CallbackType> CallbackSetType;

    unsigned int Register(CallbackType callback)
    {
        unsigned int handle = callbackSet.GetMinAvailableHandle();
        callbackSet.AssociateObject2Handle(handle, callback);
        return handle;
    }
    void Unregister(unsigned int handle)
    {
        callbackSet.DeleteHandle(handle);
    }
    bool IsRegistered(unsigned int handle)const
    {
        return callbackSet.IsHandleExists(handle);
    }
    void NotifyAll(std::function<void(CallbackType)> Invoker)
    {
        callbackSet.Iterate([&Invoker](unsigned int handle, CallbackType& callback){
            Invoker(callback);
        });
    }
    void NotifyAll(std::function<void(unsigned int, CallbackType)> Invoker)
    {
        callbackSet.Iterate([&Invoker](unsigned int handle, CallbackType& callback){
            Invoker(handle, callback);
        });
    }
    unsigned int GetSize()const
    {
        return callbackSet.GetSize();
    }

private:
    CallbackSetType callbackSet;
};


#endif // BROADCASTER_H
