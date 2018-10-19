/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef STATEMANAGERIMPL_H_
#define STATEMANAGERIMPL_H_

#include <mutex>

#include "AppBroadcast.h"
#include "ama_types.h"
#include <CommonAPI/CommonAPI.hpp>

class StateManagerImpl
{
public:
    static StateManagerImpl* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static StateManagerImpl instance;
        return &instance;
    }
    virtual ~StateManagerImpl();

    //called by powerSDK API
    void init(void);
    void reqSendBroadcast(std::string event,std::string data);
    void registBroadcastCallback(handleBroadcastFun f);


    //called by commonAPI
    void handleBroadcast(std::string event,std::string data);
private:
    StateManagerImpl();

    bool mIsInited;

    handleBroadcastFun mBroadcastCallback;
};

#endif//STATEMANAGERIMPL_H_
