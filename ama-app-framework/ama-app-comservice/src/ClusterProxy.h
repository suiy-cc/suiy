/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef CLUSTERRPROXY_H
#define CLUSTERRPROXY_H

#include "AppSDK.h"
#include "ama_audioTypes.h"
#include "ama_powerTypes.h"
#include <mutex>

#include "CommonServiceLog.h"

class ClusterProxy
{
public:
    static ClusterProxy* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static ClusterProxy instance;
        return &instance;
    }
    ~ClusterProxy();
    void tryToConnectWithCluster(void);
    void setConnectWithCluster(){
        mIsConnected = true;
    }

    //excuting in project module thread
    void sendIVIPowerStateToCluster(ama_PowerState_e iviPowerState);
protected:
    static void ivcStartedOkCb(void *data,void* userData);
private:
    ClusterProxy();
    void notifyClusterConnected(void);
private:
    bool mIsConnected;
};

#endif//CLUSTERRPROXY_H
