/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <assert.h>

#include "PMCtrl.h"
#include "ama_types.h"
#include "PMCTask.h"
#include "ama_enum.h"
#include "wd.h"
#include "BootListener.h"
#include "LineTool.h"
#include "TaskDispatcher.h"
#include "PowerManager.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

PowerManagerController::PowerManagerController()
:mIVIPowerStateFlag(E_PS_POWER_INIT)
,mLRSEPowerStateFlag(E_PS_POWER_INIT)
,mRRSEPowerStateFlag(E_PS_POWER_INIT)
{
    logVerbose(PMC,"PowerManagerController::PowerManagerController()-->IN");
    //todo read conf to check if coredump
    logVerbose(PMC,"PowerManagerController::PowerManagerController()-->OUT");
}

PowerManagerController::~PowerManagerController()
{
    logVerbose(PMC,"PowerManagerController::~PowerManagerController()-->IN");
    delete(mpIVIPowerDevice);
    delete(mpLRSEPowerDevice);
    delete(mpRRSEPowerDevice);
    //todo save normal shutdown

    logVerbose(PMC,"PowerManagerController::~PowerManagerController()-->OUT");
}

void PowerManagerController::earlyInit(void)
{
    logVerbose(PMC,"PowerManagerController::earlyInit()-->IN");

    // mpIVIPowerDevice = IVIPowerDevice::GetInstance();
    // mpLRSEPowerDevice = new RSEPowerDevice(E_SEAT_RSE1);
    // mpRRSEPowerDevice = new RSEPowerDevice(E_SEAT_RSE2);
    //
    // PowerManager::GetInstance()->Init();
    // LineTool::GetInstance()->init();
    // BootListener::GetInstance()->startListenBootSta();

    logVerbose(PMC,"PowerManagerController::earlyInit()-->OUT");
}

void PowerManagerController::Init(void)
{
    logVerbose(PMC,"PowerManagerController::Init()-->IN");

    BootListener::GetInstance()->IVCInit();  

    mpIVIPowerDevice = IVIPowerDevice::GetInstance();
    mpLRSEPowerDevice = new RSEPowerDevice(E_SEAT_RSE1);
    mpRRSEPowerDevice = new RSEPowerDevice(E_SEAT_RSE2);

    PowerManager::GetInstance()->Init();
    LineTool::GetInstance()->init();
    BootListener::GetInstance()->startListenBootSta();

    logVerbose(PMC,"PowerManagerController::Init()-->OUT");
}


void PowerManagerController::doTask(shared_ptr<Task> task)
{
    logVerbose(PMC,"PowerManagerController::doTask()-->IN");
    assert(task.get());
    auto pTask = dynamic_pointer_cast<PMCTask>(task);

    // feed the dog
    if (pTask->IsWatchDotTask()) {
      wdSetState(E_TASK_ID_PMC, WATCHDOG_SUCCESS);
      return ;
    }

    logInfo(PMC,"doTask==>""taskType=",pTask->getTaskTypeString());
    switch(pTask->GetType()){
    case HANDLE_MODULE_INIT:
        Init();
        break;
    case HANDLE_PM_SIGNAL:
        mpIVIPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        mpLRSEPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        mpRRSEPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        break;
    case HANDLE_BOOT_SIGNAL:
        mpIVIPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        break;
    case HANDLE_APP_AWAKE_SIGNAL:
        mpIVIPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        break;
    case HANDLE_LCD_SIGNAL:
        if (E_SEAT_IVI==pTask->getSeatID())
        {
            mpIVIPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        }
        else if (E_SEAT_RSE1==pTask->getSeatID())
        {
            mpLRSEPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        }
        else if (E_SEAT_RSE2==pTask->getSeatID())
        {
            mpRRSEPowerDevice->handlePowerSignal(pTask->getPowerSignal());
        }
        else
        {
            logError(PMC,"HANDLE_LCD_SIGNAL get invalid seat number!");
        }
        break;
    case HANDLE_APP_REQ_SET_SCREENON:
        HandleAppRequest();
        break;
    default: logError(PMC,"Error task type for PowerManagerController");
        break;
    };
    logVerbose(PMC,"PowerManagerController::doTask()-->OUT");
}
void PowerManagerController::HandleAppRequest()
{
    logVerbose(PMC,"PowerManagerController::HandleAppRequest()-->IN ");

    PowerManager::GetInstance()->SetScreenOnByApp();

    logVerbose(PMC,"PowerManagerController::HandleAppRequest()-->OUT");
}


bool PowerManagerController::isIVIDevicePowerOn(void)
{
    return mpIVIPowerDevice->isPowerOn();
}

ama_PowerState_e PowerManagerController::getPowerState(uint32_t seat)
{
    logVerbose(PMC,"PowerManagerController::getPowerState()-->IN");
    switch(seat){
    case E_SEAT_IVI:
        return mIVIPowerStateFlag;
        break;
    case E_SEAT_RSE1:
        return mLRSEPowerStateFlag;
        break;
    case E_SEAT_RSE2:
        return mRRSEPowerStateFlag;
        break;
    default:
        logError(PMC,"Error seat");
        break;
    }
    logVerbose(PMC,"PowerManagerController::getPowerState()-->OUT");
}

void PowerManagerController::updatePowerStateByDevice(uint32_t seat,ama_PowerState_e powerState)
{
    logVerbose(PMC,"PowerManagerController::updatePowerStateByDevice()-->IN");
    switch(seat){
    case E_SEAT_IVI:
        mIVIPowerStateFlag = powerState;
        break;
    case E_SEAT_RSE1:
        mLRSEPowerStateFlag = powerState;
        break;
    case E_SEAT_RSE2:
        mRRSEPowerStateFlag = powerState;
        break;
    default:
        logError(PMC,"Error seat");
        break;
    }
    logVerbose(PMC,"PowerManagerController::updatePowerStateByDevice()-->OUT");
}

void sendInitTaskToPMC(void)
{
    auto pTask = std::make_shared<PMCTask>();
    pTask->SetType(HANDLE_MODULE_INIT);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}
