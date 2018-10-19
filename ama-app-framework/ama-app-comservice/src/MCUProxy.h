/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef MCUPROXY_H
#define MCUPROXY_H

#include "AppSDK.h"
#include "ama_audioTypes.h"
#include <mutex>

#include "CommonServiceLog.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <librpc.h>
#ifdef __cplusplus
}
#endif

class MCUProxy
{
public:
    static MCUProxy* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static MCUProxy instance;
        return &instance;
    }
    ~MCUProxy();

    void startListeningEvents(void);

    //excuting in callback function
    void responeFactoryCMDResult(bool isSuccessed);

    //excuting in project module thread
    void sendActiveMediaStreamTypeToMCU(ama_streamType_t activeMediaStreamType);
    void sendMediaOffToMCU(void);
    void handleFactoryAppStarted(void);
protected:
    static void cb_rpc(librpc_rx_t *rxbuf, void *user);
private:
    MCUProxy();
    void initRPC(void);
private:
    librpc_t* m_pRPC;
    librpc_tx_t t_RPC;
};

#endif//MCUPROXY_H
