/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "LineTool.h"
#include "AMGRLogContext.h"
#include "PMCTask.h"
#include "TaskDispatcher.h"
#include "IVIPowerDevice.h"

#define ZeroMemory(Destination,Length)      memset((Destination), 0, (Length))

#define LIBRPC_TXDATA_MAX                       (25)

#define LINETOOLS_OPCODE_POWEROFF           (0x1207)
#define LINETOOLS_OPCODE_POWERON            (0x1208)
#define LINTTOLLS_DATA_POWEROFF_CMD         (0x01)
#define LINTTOLLS_DATA_POWERON_CMD          (0x01)
#define LINETOOLS_OPCODE_RESPONE            (0x1200)
#define LINETOOLS_DATA_RESPONE_OK           (0x00)
#define LINETOOLS_DATA_RESPONE_NG           (0x01)

#define _OK                             (true)
#define _NG                             (false)

LineTool::LineTool()
:m_pRPC(nullptr)
,mIsPowerOnNeedRespone(false)
,mIsPowerOffNeedRespone(false)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

LineTool::~LineTool()
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

void LineTool::init(void)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");
    initRPC();
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

void LineTool::cb_rpc(librpc_rx_t *rxbuf, void *user)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");

    if (rxbuf == NULL){
        logWarn(PMC,"CB_RPC()-->>input librpc_rx_t is null");
        return;
    }

    if(LINETOOLS_OPCODE_POWERON==(rxbuf->opcode)){
        logDebug(PMC,"recive opcode LINETOOLS_OPCODE_POWERON");
        if(LINTTOLLS_DATA_POWERON_CMD==rxbuf->data[0]){
            logDebug(PMC,"recive cmd LINTTOLLS_DATA_POWERON_CMD");
            if(IVIPowerDevice::GetInstance()->isPowerOn()){
                LineTool::GetInstance()->sendLineToolResult(_OK);
            }else{
                //TODO:send a task to PMC
                LineTool::GetInstance()->sendTaskToPMC(true);
            }
        }
    }else if(LINETOOLS_OPCODE_POWEROFF==(rxbuf->opcode)){
        logDebug(PMC,"recive opcode LINETOOLS_OPCODE_POWEROFF");
        if(LINTTOLLS_DATA_POWEROFF_CMD==rxbuf->data[0]){
            logDebug(PMC,"recive cmd LINTTOLLS_DATA_POWEROFF_CMD");
            if(IVIPowerDevice::GetInstance()->isPowerOn()){
                //TODO:send a task to PMC
                LineTool::GetInstance()->sendTaskToPMC(false);
            }else{
                LineTool::GetInstance()->sendLineToolResult(_OK);
            }
        }
    }

    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

void LineTool::sendTaskToPMC(bool isSetPowerOn)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");

    //send a boot signal to PMC
    auto pmcTask = make_shared<PMCTask>();
    pmcTask->SetType(HANDLE_PM_SIGNAL);
    if(isSetPowerOn){
        mIsPowerOnNeedRespone = true;
        pmcTask->setPowerSignal(E_PM_POWER_ON);
    }else{
        mIsPowerOffNeedRespone = true;
        pmcTask->setPowerSignal(E_PM_POWER_OFF);
    }
    TaskDispatcher::GetInstance()->Dispatch(pmcTask);

    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

void LineTool::handlePowerStaChanged(ama_PowerState_e powerState)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");
    switch(powerState){
    case E_PS_ACCON_ABNORMAL_SOCVOLTAGE:
    case E_PS_ACCON_ABNORMAL_SOCTEMP:
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGH:
    case E_PS_ACCON_ABNORMAL_SCRTEMP_HIGHER:
    case E_PS_GOODBYE_MODE:
        logDebug(PMC,"LineTool don't handle those powerState");
        break;
    case E_PS_ACCON_NORMAL_POWERON_SCRON:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF:
    case E_PS_ACCON_NORMAL_POWERON_SCROFF_AWAKE:
        sendLineToolResult(LINETOOLS_OPCODE_POWERON,true);
        break;
    case E_PS_ACCOFF:
    case E_PS_ACCON_NORMAL_POWEROFF:
    case E_PS_ACCON_NORMAL_POWEROFF_AWAKE:
        sendLineToolResult(LINETOOLS_OPCODE_POWEROFF,true);
        break;
    default:
        logError(PMC,"powerStateFlag is not defined");
        break;
    }
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

void LineTool::sendLineToolResult(bool isSuccessed)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");

    ZeroMemory(t_RPC.data, LIBRPC_TXDATA_MAX);
    t_RPC.control = 0x00;				/**< control 暂时未使用，缺省值为0x00*/
    t_RPC.datalen = LIBRPC_TXDATA_MAX;	/**< date中数据的实际大小*/

    t_RPC.opcode = LINETOOLS_OPCODE_RESPONE;
    if(isSuccessed){
        t_RPC.data[0] = (uint8_t)LINETOOLS_DATA_RESPONE_OK;
    }else{
        t_RPC.data[0] = (uint8_t)LINETOOLS_DATA_RESPONE_NG;
    }

    int nRes = librpc_send(m_pRPC, &t_RPC);
    if (nRes == 0){
        logDebug(PMC,"sendLineToolResult-->>send rpc data ok");
    }else{
        logWarn(PMC,"sendLineToolResult-->>send rpc data failed");
    }

    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}

void LineTool::sendLineToolResult(int lineToolOpcode,bool isSuccessed)
{
    if(LINETOOLS_OPCODE_POWERON==lineToolOpcode){
        if(mIsPowerOnNeedRespone&&isSuccessed){
            sendLineToolResult(_OK);
            mIsPowerOnNeedRespone = false;
        }
    }else if(LINETOOLS_OPCODE_POWEROFF==lineToolOpcode){
        if(mIsPowerOffNeedRespone&&isSuccessed){
            sendLineToolResult(_OK);
            mIsPowerOffNeedRespone = false;
        }
    }
}

void LineTool::initRPC(void)
{
    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>IN");

    int nRes = librpc_ref(&m_pRPC);
    if (nRes != 0){
        logWarn(PMC,"librpc_ref() failed");
    }

    nRes = librpc_when_data(m_pRPC,cb_rpc,(void*)this);
    if (nRes != 0){
        logWarn(PMC,"librpc_when_data() failed");
    }

    logVerbose(PMC,"LineTool::",__FUNCTION__,"-->>OUT");
}
