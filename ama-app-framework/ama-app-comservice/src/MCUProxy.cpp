/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "MCUProxy.h"
#include "ProTask.h"
#include "AppBroadcast.h"

#define ZeroMemory(Destination,Length)      memset((Destination), 0, (Length))

#define LIBRPC_TXDATA_MAX                       (25)
#define LIBRPC_DEFAULT_CONTROL_CODE             (0x00)

#define LINETOOLS_OPCODE_RESPONE                (0x1200)
#define LINETOOLS_OPCODE_ENTER_OR_EXIT          (0x120D)
#define LINETOOLS_OPCODE_ACTIVE_MEDIA           (0x0C21)
#define LINETOOLS_DATA_ENTER_CMD                (0x00)
#define LINETOOLS_DATA_EXIT_CMD                 (0x01)
#define LINETOOLS_DATA_RESPONE_OK               (0x00)
#define LINETOOLS_DATA_RESPONE_NG               (0x01)

#define LINETOOLS_DATA_MEDIA_OFF                (0x00)
#define LINETOOLS_DATA_MEDIA_TUNER              (0x01)
#define LINETOOLS_DATA_MEDIA_MUSIC              (0x02)
#define LINETOOLS_DATA_MEDIA_VIDEO_AUDIO        (0x03)
#define LINETOOLS_DATA_MEDIA_BT_AUDIO           (0x04)
#define LINETOOLS_DATA_MEDIA_PHONELINK_AUDIO    (0x05)
#define LINETOOLS_DATA_MEDIA_3THAPP_AUDIO       (0x06)
#define LINETOOLS_DATA_MEDIA_FM                 (0x07)
#define LINETOOLS_DATA_MEDIA_AM                 (0x08)
#define LINETOOLS_DATA_MEDIA_IPOD_AUDIO         (0x09)
#define LINETOOLS_DATA_MEDIA_CARPLAY_AUDIO      (0x0A)
#define LINETOOLS_DATA_MEDIA_CARLIFE_AUDIO      (0x0B)
#define LINETOOLS_DATA_MEDIA_FM_SEC             (0x0C)
#define LINETOOLS_DATA_MEDIA_INVALID            (0xFF)

static bool sIsNeedResponeFactoryCMD = false;

MCUProxy::MCUProxy()
:m_pRPC(nullptr)
{
    RegisterContext(MCUP,"MCUP", "MCU proxy log context");
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");
    initRPC();
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

MCUProxy::~MCUProxy()
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::startListeningEvents(void)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");
    int nRes = librpc_when_data(m_pRPC,cb_rpc,(void *)this);
    if(nRes != 0){
        logWarn(MCUP,"librpc_when_data() failed");
    }
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::handleFactoryAppStarted(void)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");

    if(sIsNeedResponeFactoryCMD){
        responeFactoryCMDResult(true);
        sIsNeedResponeFactoryCMD = false;
    }

    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::cb_rpc(librpc_rx_t *rxbuf, void *user)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");

    if (rxbuf == NULL){
        logWarn(MCUP,"CB_RPC()-->>input librpc_rx_t is null");
        return;
    }

    if(LINETOOLS_OPCODE_ENTER_OR_EXIT==(rxbuf->opcode)){
        logDebug(MCUP,"recive opcode LINETOOLS_OPCODE_ENTER_OR_EXIT");
        if(LINETOOLS_DATA_ENTER_CMD==rxbuf->data[0]){
            logDebug(MCUP,"recive cmd LINETOOLS_DATA_ENTER_CMD");
            sIsNeedResponeFactoryCMD = true;
            //TODO:send start factory app request to project module
            auto pTask = std::make_shared<ProjectTask>();
            pTask->SetType(E_PRO_TASK_REQ_START_FACTORYAPP);
            TaskDispatcher::GetInstance()->Dispatch(pTask);
        }else if(LINETOOLS_DATA_EXIT_CMD==rxbuf->data[0]){
            MCUProxy::GetInstance()->responeFactoryCMDResult(true);
        }
    }

    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::responeFactoryCMDResult(bool isSuccessed)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");

    ZeroMemory(t_RPC.data, LIBRPC_TXDATA_MAX);
    t_RPC.control = LIBRPC_DEFAULT_CONTROL_CODE;				/**< control 暂时未使用，缺省值为0x00*/
    t_RPC.datalen = LIBRPC_TXDATA_MAX;	/**< date中数据的实际大小*/

    t_RPC.opcode = LINETOOLS_OPCODE_RESPONE;
    if(isSuccessed){
        t_RPC.data[0] = (uint8_t)LINETOOLS_DATA_RESPONE_OK;
    }else{
        t_RPC.data[0] = (uint8_t)LINETOOLS_DATA_RESPONE_NG;
    }

    int nRes = librpc_send(m_pRPC, &t_RPC);
    if (nRes == 0){
        logDebug(MCUP,"responeFactoryCMDResult-->>send rpc data ok");
    }else{
        logDebug(MCUP,"responeFactoryCMDResult-->>send rpc data failed");
    }

    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::sendMediaOffToMCU(void)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");

    ZeroMemory(t_RPC.data, LIBRPC_TXDATA_MAX);
    t_RPC.control = LIBRPC_DEFAULT_CONTROL_CODE;				/**< control 暂时未使用，缺省值为0x00*/
    t_RPC.datalen = LIBRPC_TXDATA_MAX;	/**< date中数据的实际大小*/

    t_RPC.opcode = LINETOOLS_OPCODE_ACTIVE_MEDIA;
    t_RPC.data[0] = LINETOOLS_DATA_MEDIA_OFF;

    int nRes = librpc_send(m_pRPC, &t_RPC);
    if (nRes == 0){
        logDebug(MCUP,"sendMediaOffToMCU-->>send rpc data ok");
        sendBroadcast("autoTest.ama-app-comservice.sendMediaOffToMCU","");
    }else{
        logDebug(MCUP,"sendMediaOffToMCU-->>send rpc data failed");
    }

    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::sendActiveMediaStreamTypeToMCU(ama_streamType_t activeMediaStreamType)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");

    ZeroMemory(t_RPC.data, LIBRPC_TXDATA_MAX);
    t_RPC.control = LIBRPC_DEFAULT_CONTROL_CODE;				/**< control 暂时未使用，缺省值为0x00*/
    t_RPC.datalen = LIBRPC_TXDATA_MAX;	/**< date中数据的实际大小*/

    t_RPC.opcode = LINETOOLS_OPCODE_ACTIVE_MEDIA;

    switch(activeMediaStreamType){
    case E_STREAM_TUNER:            t_RPC.data[0] = LINETOOLS_DATA_MEDIA_TUNER;             break;
    case E_STREAM_MUSIC:            t_RPC.data[0] = LINETOOLS_DATA_MEDIA_MUSIC;             break;
    case E_STREAM_VIDEO_AUDIO:      t_RPC.data[0] = LINETOOLS_DATA_MEDIA_VIDEO_AUDIO;       break;
    case E_STREAM_BT_AUDIO:         t_RPC.data[0] = LINETOOLS_DATA_MEDIA_BT_AUDIO;          break;
    case E_STREAM_PHONELINK_AUDIO:  t_RPC.data[0] = LINETOOLS_DATA_MEDIA_PHONELINK_AUDIO;   break;
    case E_STREAM_3THAPP_AUDIO:     t_RPC.data[0] = LINETOOLS_DATA_MEDIA_3THAPP_AUDIO;      break;
    case E_STREAM_FM:               t_RPC.data[0] = LINETOOLS_DATA_MEDIA_FM;                break;
    case E_STREAM_AM:               t_RPC.data[0] = LINETOOLS_DATA_MEDIA_AM;                break;
    case E_STREAM_IPOD_AUDIO:       t_RPC.data[0] = LINETOOLS_DATA_MEDIA_IPOD_AUDIO;        break;
    case E_STREAM_CARPLAY_AUDIO:    t_RPC.data[0] = LINETOOLS_DATA_MEDIA_CARPLAY_AUDIO;     break;
    case E_STREAM_CARLIFE_AUDIO:    t_RPC.data[0] = LINETOOLS_DATA_MEDIA_CARLIFE_AUDIO;     break;
    case E_STREAM_FM_SEC:           t_RPC.data[0] = LINETOOLS_DATA_MEDIA_FM_SEC;            break;
    default:
        logError(MCUP,"activeMediaStreamType is not defined!");
        t_RPC.data[0] = LINETOOLS_DATA_MEDIA_INVALID;
        break;
    }

    int nRes = librpc_send(m_pRPC, &t_RPC);
    if (nRes == 0){
        logDebug(MCUP,"sendActiveMediaStreamTypeToMCU-->>send rpc data ok");
        sendBroadcast("autoTest.ama-app-comservice.sendActiveMediaStreamTypeTOMCU","");
    }else{
        logDebug(MCUP,"sendActiveMediaStreamTypeToMCU-->>send rpc data failed");
    }

    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}

void MCUProxy::initRPC(void)
{
    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>IN");

    int nRes = librpc_ref(&m_pRPC);
    if (nRes != 0){
        logWarn(MCUP,"librpc_ref() failed");
    }

    logVerbose(MCUP,"MCUProxy::",__FUNCTION__,"-->>OUT");
}
