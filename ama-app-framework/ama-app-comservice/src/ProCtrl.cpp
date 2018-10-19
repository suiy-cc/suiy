/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AudioCtrl.cpp
/// @brief contains the implementation of class AudioController
///
/// Created by wangqi on 2016/6/16.
/// this file contains the implementation of class AudioController
///

#include <assert.h>

#include "ProCtrl.h"
#include "ProTask.h"
#include "CommonServiceLog.h"
#include "StateManagerProxy.h"
#include "AppManagerProxy.h"
#include "ClusterProxy.h"
#include "MCUProxy.h"
#include "checkWeLinkDevice.h"

#include "AppBroadcast.h"

ProjectController::ProjectController()
:mCurIVIPowerSta(E_PS_POWER_INIT)
,mIsWelinkIOSDeviceAttached(false)
,mIsWelinkSelectedInSyssetting(false)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

ProjectController::~ProjectController()
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::doTask(shared_ptr<Task> task)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");
    assert(task.get());
    auto pTask = dynamic_pointer_cast<ProjectTask>(task);

    int taskID = pTask->GetID();
    logInfo(PRO,"doTask==>""taskType=",pTask->GetTaskTypeString());

    switch(pTask->GetType()){
    case E_RPO_TASK_INIT:
        ProjectControllerInit();
        break;
    case E_RPO_TASK_CLUSTER_CONNECTED:
        notifyIVIPowerStaToCluster();
        break;
    case E_RPO_TASK_IVI_POWER_CHANGED:
        handleIVIPowerStaChanged(static_cast<ama_PowerState_e>(pTask->getInt32Arg(0)));
        break;
    case E_PRO_TASK_CHANGE_OF_POWER:
        handleChangeOfPowerChanged(static_cast<uint8_t>(pTask->getInt32Arg(0)),static_cast<ama_changeOfPowerSta_e>(pTask->getInt32Arg(1)));
        break;
    case E_PRO_TASK_AUDIOFOCUS_CHANGED:
        handleAudioFocusChanged(static_cast<ama_connectID_t>(pTask->getInt32Arg(0)),static_cast<ama_focusSta_t>(pTask->getInt32Arg(1))
        ,static_cast<ama_focusSta_t>(pTask->getInt32Arg(2)),static_cast<ama_streamID_t>(pTask->getInt32Arg(3)));
        break;
    case E_PRO_TASK_APPSTATE_CHANGED:
        handleAppStateChanged(static_cast<AMPID>(pTask->getInt32Arg(0)),static_cast<AppStatus>(pTask->getInt32Arg(1)));
        break;
    case E_PRO_TASK_REQ_START_FACTORYAPP:
        AppManagerProxy::GetInstance()->startFactoryApp();
        break;
    case E_PRO_TASK_WELINK_IOS_DEVICE_ATTACH_STA_CHANGED:
        handleWelinkIOSDeviceAttachedState(pTask->getBoolArg(0));
        break;
    case E_PRO_TASK_WELINK_SELECTED_STA_CHANGED:
        handleWelinkSelectedStateInSyssetting(pTask->getBoolArg(0));
        break;
    default: logError(PRO,"Error task type for ProjectController");
        break;
    };
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::ProjectControllerInit()
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    // TODO:start caplife app quiet,make sure carLife start quickly
    AppManagerProxy::GetInstance()->startCarlifeApp();

    startScaningWelinkIOSDevice();

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::notifyIVIPowerStaToCluster(void)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    ClusterProxy::GetInstance()->sendIVIPowerStateToCluster(mCurIVIPowerSta);

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::handleIVIPowerStaChanged(ama_PowerState_e iviPowerState)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    mCurIVIPowerSta = iviPowerState;
    ClusterProxy::GetInstance()->sendIVIPowerStateToCluster(mCurIVIPowerSta);
    AppManagerProxy::GetInstance()->handleIGSingal(mCurIVIPowerSta);
    AppManagerProxy::GetInstance()->handleAbnormalSignal(mCurIVIPowerSta);

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::handleChangeOfPowerChanged(uint8_t seatID,ama_changeOfPowerSta_e changeOfPowerSta)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");
    logDebugF(PRO,"-->>seatID:%x,changeOfPowerSta:%x",seatID,changeOfPowerSta);

    if(E_POWER_CHANGE_TO_ON==changeOfPowerSta){
        StateManagerProxy::GetInstance()->setSystemMuteSta(seatID,false);
    }

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::handleAudioFocusChanged(
        ama_connectID_t connectID,ama_focusSta_t oldFocusSta,ama_focusSta_t newFocusSta,ama_streamID_t applyStreamID)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");
    logDebugF(PRO,"-->>connectID:%x,oldFocusSta:%x,newFocusSta:%x \n",connectID,oldFocusSta,newFocusSta);

    ama_focusType_t focusType = GET_FOCUSTYPE_FROM_STREAMID(connectID);
    ama_streamType_t streamType = GET_STREAMTYPE_FROM_CONNECTID(connectID);
    if(E_FOCUS_TYPE_MEDIA==focusType){
        //we only handle unmute because play action from media app
        if((E_FOUCS_LOSE==oldFocusSta)&&(E_FOUCS_GAIN==newFocusSta)){
            sendBroadcast("autoTest.ama-app-comservice.setUnmute.whenMediaGetFocus","");
            StateManagerProxy::GetInstance()->setSystemMuteSta(GET_DEVICETYPE_FROM_CONNECTID(connectID),false);
        }
        //notify media streamType to MCU
        if(E_FOUCS_GAIN==newFocusSta){
            MCUProxy::GetInstance()->sendActiveMediaStreamTypeToMCU(streamType);
        }else if(E_FOUCS_LOSE==newFocusSta){
            //becaust there is only one media stream can active
            //and it must send focus loss first
            MCUProxy::GetInstance()->sendMediaOffToMCU();
        }
    }

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::handleAppStateChanged(const AMPID ampid, const AppStatus appStatus)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");
    logDebugF(PRO,"-->>ampid:%x,appStatus:%x \n",ampid,appStatus);

    std::string itemID = GetItemID(ampid);
    if("com.hs7.factory.scr1"==itemID){
        if(IS_VISIBLE(appStatus)){
            MCUProxy::GetInstance()->handleFactoryAppStarted();
        }
    }

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void ProjectController::handleWelinkIOSDeviceAttachedState(bool isAttached)
{
    mIsWelinkIOSDeviceAttached = isAttached;
    if(mIsWelinkIOSDeviceAttached && mIsWelinkSelectedInSyssetting){
        AppManagerProxy::GetInstance()->startWelinkApp();
    }
}

void ProjectController::handleWelinkSelectedStateInSyssetting(bool isSelected)
{
    mIsWelinkSelectedInSyssetting = isSelected;
    if(mIsWelinkIOSDeviceAttached && mIsWelinkSelectedInSyssetting){
        AppManagerProxy::GetInstance()->startWelinkApp();
    }
}

void sendInitTaskToProjectModule(void)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_RPO_TASK_INIT);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void updateWelinkIOSDeviceAttacheStaToPro(bool isAttached)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_PRO_TASK_WELINK_IOS_DEVICE_ATTACH_STA_CHANGED);
    pTask->setBoolArg(isAttached);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}

void updateWelinkSelectedStaToPro(bool isSelected)
{
    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>IN");

    auto pTask = std::make_shared<ProjectTask>();
    pTask->SetType(E_PRO_TASK_WELINK_SELECTED_STA_CHANGED);
    pTask->setBoolArg(isSelected);
    TaskDispatcher::GetInstance()->Dispatch(pTask);

    logVerbose(PRO,"ProjectController::",__FUNCTION__,"-->>OUT");
}
