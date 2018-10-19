/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "ClusterProxy.h"
#include "ProTask.h"
#include "ama_types.h"

#include <if.h>
#include <ivc_types.h>


ClusterProxy::ClusterProxy()
:mIsConnected(false)
{
    RegisterContext(CLTP,"CLTP", "Cluster proxy log context");
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>IN");

    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>OUT");
}

ClusterProxy::~ClusterProxy()
{
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>IN");
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>OUT");
}

void ClusterProxy::tryToConnectWithCluster(void)
{
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>IN");

    char* name = "IVI_COMMON_SERVICE";
    cluster_connection_init();
    ivc_signal_connect("start_ok",ivcStartedOkCb,NULL);
    cluster_connection_start(name);

    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>OUT");
}

void ClusterProxy::ivcStartedOkCb(void *data,void* userData)
{
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>IN");

    ClusterProxy::GetInstance()->setConnectWithCluster();
    ClusterProxy::GetInstance()->notifyClusterConnected();

    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>OUT");
}

void ClusterProxy::notifyClusterConnected(void)
{
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_RPO_TASK_CLUSTER_CONNECTED);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>OUT");
}

void ClusterProxy::sendIVIPowerStateToCluster(ama_PowerState_e iviPowerState)
{
    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>IN");

    if(mIsConnected){
        switch(iviPowerState){
        case E_PS_ACCOFF:
            noticePowerStatus(IVC_PS_ACCOFF);
            break;
        case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
            noticePowerStatus(IVC_PS_ACCON_ABNORMAL_SOCVOLTAGE);
            break;
        case E_PS_ACCON_ABNORMAL_SOCTEMP:
            noticePowerStatus(IVC_PS_ACCON_ABNORMAL_SOCTEMP);
            break;
        case E_PS_ACCON_NORMAL_POWEROFF:
            noticePowerStatus(IVC_PS_ACCON_NORMAL_POWEROFF);
            break;
        case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
            noticePowerStatus(IVC_PS_ACCON_ABNORMAL_SCRTEMP_HIGH);
            break;
        case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
            noticePowerStatus(IVC_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER);
            break;
        case E_PS_ACCON_NORMAL_POWERON_SCRON:
            noticePowerStatus(IVC_PS_ACCON_NORMAL_POWERON_SCRON);
            break;
        case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
            noticePowerStatus(IVC_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE);
            break;
        case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
            noticePowerStatus(IVC_PS_ACCON_NORMAL_POWEROFF_AWAKE);
            break;
        case E_PS_ACCON_NORMAL_POWERON_SCROFF:
            noticePowerStatus(IVC_PS_ACCON_NORMAL_POWERON_SCROFF);
            break;
        default:
            logWarn(CLTP,"powerStateFlag is not notify to cluster");
            break;
        }
    }

    logVerbose(CLTP,"ClusterProxy::",__FUNCTION__,"-->>OUT");
}
