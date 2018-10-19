/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "BPMCtrl.h"
#include "wd.h"
#include "AppList.h"
#include "UIProcess.h"

BPMCtrl::BPMCtrl()
{

}
BPMCtrl::~BPMCtrl()
{

}
void BPMCtrl::Init(void)
{

}
void BPMCtrl::doTask(shared_ptr<Task> task)
{
    logVerbose(BPM,"BPMCtrl::doTask()-->IN");
    assert(task.get());
    auto pTask = dynamic_pointer_cast<BPMTask>(task);

    // feed the dog
//    if (pTask->IsWatchDotTask()) {
//        wdSetState(E_TASK_ID_BPM, WATCHDOG_SUCCESS);
//        return ;
//    }
    switch(pTask->GetType()){
    case BPM_CMD_NEVER_STOP:
        BPManager::GetInstance()->MakeMeAlive(pTask->getApmid(),true);
        break;
    case BPM_CMD_CAN_BE_STOP:
         BPManager::GetInstance()->MakeMeAlive(pTask->getApmid(),false);
        break;
    default: logError(BPM,"Error task type for BPMCtrl");
        break;
    };


    logVerbose(BPM,"BPMCtrl::doTask()-->OUT");
}
BPMCtrl* BPMCtrl::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    std::mutex Mutex;
    std::unique_lock<std::mutex> Lock(Mutex);

    static BPMCtrl instance;
    return &instance;
}
