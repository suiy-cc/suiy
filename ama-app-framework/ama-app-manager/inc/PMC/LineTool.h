/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef LINETOOL_H
#define LINETOOL_H

#include "ama_enum.h"

#include <mutex>
#ifdef __cplusplus
extern "C"
{
#endif
#include <librpc.h>
#ifdef __cplusplus
}
#endif

class LineTool
{
public:
    static LineTool* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static LineTool instance;
        return &instance;
    }
    ~LineTool();

    void init();
    void sendLineToolResult(bool isSuccessed);
    void sendLineToolResult(int lineToolOpcode,bool isSuccessed);
    void sendTaskToPMC(bool isSetPowerOn);
    void handlePowerStaChanged(ama_PowerState_e powerState);
protected:
    static void cb_rpc(librpc_rx_t *rxbuf, void *user);
private:
    LineTool();
    void initRPC(void);
private:
    librpc_t* m_pRPC;
    librpc_tx_t t_RPC;
    bool mIsPowerOnNeedRespone;
    bool mIsPowerOffNeedRespone;
};

#endif//LINETOOL_H
