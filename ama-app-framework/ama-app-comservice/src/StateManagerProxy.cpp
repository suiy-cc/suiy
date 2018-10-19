/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "StateManagerProxy.h"
#include "ProCtrl.h"
#include "ProTask.h"
#include "CommonServiceLog.h"

static const std::string PHONELINE_SET_TYPE = "syssetting_FAST_CHARGE_SET";
static const std::string WELINK_SET = "3";

StateManagerProxy::StateManagerProxy()
{
    RegisterContext(SMP,"SMP", "State Manager proxy log context");
    logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>IN");
    logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>OUT");
}

StateManagerProxy::~StateManagerProxy()
{
    logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>IN");
    logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>OUT");
}

void StateManagerProxy::init(void)
{
    logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>IN");

    SM_regCb4StateChanged(stateChanged_cb);

    if(SM_isStateExist(PHONELINE_SET_TYPE)){
        string stateValue = SM_getState(PHONELINE_SET_TYPE);
        if(stateValue==WELINK_SET){
            updateWelinkSelectedStaToPro(true);
        }
    }else{
        logError(SMP,"syssetting_FAST_CHANGE_SET state is not exist in StateManager");
    }

    logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>OUT");
}

void StateManagerProxy::setSystemMuteSta(int seatID,bool isMute)
{
    string setKey;
    string setValue;
    if(isMute){
        setValue = "1";
    }else{
        setValue = "0";
    }

    switch(seatID){
    case E_SEAT_IVI:
        setKey = "syssetting_IVI_MUTE_SWITCH";
        SM_req2SetState(setKey,setValue);
        break;
    case E_SEAT_RSE1:
        setKey = "syssetting_RSE1_MUTE_SWITCH";
        SM_req2SetState(setKey,setValue);
        break;
    case E_SEAT_RSE2:
        setKey = "syssetting_RSE2_MUTE_SWITCH";
        SM_req2SetState(setKey,setValue);
        break;
    default:
        logWarn(SMP,"seatID is not defined!");
        break;
    }
}

void StateManagerProxy::stateChanged_cb(uint16_t ampid_req, uint16_t ampid_reply, std::string  key,  std::string  value)
{
    // logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>IN");

    if(PHONELINE_SET_TYPE==key){
        logDebug(SMP,"stateChanged_cb()-->>value:",value);
        if(value==WELINK_SET){
            updateWelinkSelectedStaToPro(true);
        }else{
            updateWelinkSelectedStaToPro(false);
        }
    }

    // logVerbose(SMP,"StateManagerProxy::",__FUNCTION__,"-->>OUT");
}
