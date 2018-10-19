/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef STATEMANAGERPROXY_H
#define STATEMANAGERPROXY_H

#include "log.h"
#include "AppSDK.h"
#include <mutex>

extern LOG_DECLARE_CONTEXT(COS);

class StateManagerProxy
{
public:
    static StateManagerProxy* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static StateManagerProxy instance;
        return &instance;
    }
    ~StateManagerProxy();

    void init();
    void setSystemMuteSta(int seatID,bool isMute);
protected:
    static void stateChanged_cb(uint16_t ampid_req, uint16_t ampid_reply, std::string  key,  std::string  value);
private:
    StateManagerProxy();
private:

};

#endif//STATEMANAGERPROXY_H
