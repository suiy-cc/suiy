/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FadeInThread.cpp
/// @brief contains the implementation of class FadeInThread
///
/// Created by wangqi on 2017/08/03.
/// this file contains the implementation of class FadeInThread
///

#include "FadeInThread.h"

FadeInThread::FadeInThread(HandlerFuncType handler)
:mSetVolumeHandler(handler)
,mIsSetToStop(false)
,mIsRunning(false)
,mIsDoingTask(false)
,mIsInterruptTask(false)
,mStepCnt(0)
,mStepInterval_ms(SYSTEM_DEFAULT_INTERVAL)
{
    logVerbose(AC,"FadeInThread::FadeInThread()-->IN");

    logVerbose(AC,"FadeInThread::FadeInThread()-->OUT");
}

FadeInThread::~FadeInThread()
{
    logVerbose(AC,"FadeInThread::~FadeInThread()-->IN");

    logVerbose(AC,"FadeInThread::~FadeInThread()-->OUT");
}

void FadeInThread::startDoingTask()
{
    logVerbose(AC,"FadeInThread::startDoingTask()-->IN");

    while(!mIsDoingTask){
        mConditionVariable.notify_one();
        usleep(500);
    }
    logDebug(AC,"mIsDoingTask:",mIsDoingTask);

    logVerbose(AC,"FadeInThread::startDoingTask()-->OUT");
}

void FadeInThread::stopCurrentTask()
{
    logVerbose(AC,"FadeInThread::stopCurrentTask()-->IN");

    //check is doing FadeIn task
    if(mIsDoingTask){
        //interrupt FadeIn Task
        mIsInterruptTask = true;
        //wait interrupt successed
        while(mIsDoingTask){
            usleep(500);
        }
        logDebug(AC,"mIsDoingTask:",mIsDoingTask);
        mIsInterruptTask = false;
    }

    logVerbose(AC,"FadeInThread::stopCurrentTask()-->OUT");
}

void FadeInThread::setVolumeByFadeIn(ama_FadeInType_e fadeInType,int durations_ms,int fromGain,int toGain)
{
    logVerbose(AC,"FadeInThread::setVolumeByFadeIn()-->IN fromGain:",fromGain," toGain:",toGain);

    stopCurrentTask();

    if((fromGain==AM_MUTE_DB)||(toGain==AM_MUTE_DB)){
        mSetVolumeHandler(toGain);
        logInfo(AC,"AudioManaget handle this situation:(fromGain==AM_MUTE_DB)||(toGain==AM_MUTE_DB)");
        return;
    }

    if((fromGain<-800)||(toGain<-800)){
        logError(AC,"Wrong situation:((fromGain<-800)||(toGain<-800))");
        return;
    }

    switch(fadeInType){
    case E_SYSTEM_DEF:
        planVolStepBySysDef(fromGain,toGain);
        break;
    case E_DUCK_CARPLAY:
        planVolStepByDuck(durations_ms,fromGain,toGain);
        break;
    default:
        logError(AC,"fadeInType is't defined");
        break;
    }

    //wake up thread to do fade in task
    startDoingTask();

    logVerbose(AC,"FadeInThread::setVolumeByFadeIn()-->OUT");
}

void FadeInThread::run()
{
    logVerbose(AC,"FadeInThread::Run()-->IN");

    try
    {
        mIsRunning   = true;
        mIsSetToStop = false;
        bool isSuccessed = false;
        while(true)
        {
            {
                if(!mIsDoingTask)
                {
                    std::unique_lock<std::mutex> uLock(mMutex);
                    logDebug(AC, "->->wating to do duck task");
                    mConditionVariable.wait(uLock);
                    mIsDoingTask     = true;
                    mIsInterruptTask = false;
                    logDebug(AC, "->->start doing duck task");
                }
                //if set stop flag stop this thread
                if(mIsSetToStop)
                {
                    break;
                }
            }

            //handle the task
            if(mIsDoingTask)
            {
                logDebug(AC, "->->handle the duck in task");
                for(int i    = 0; i <= mStepCnt; i++)
                {
                    logDebug(AC, "->->handle i:", i);
                    logDebug(AC, "->->handle volStepArray[i]:", volStepArray[i]);
                    isSuccessed = mSetVolumeHandler(volStepArray[i]);
                    if(!isSuccessed)
                    {
                        logWarn(AC, "->->mSetVolumeHandler failed");
                        mIsDoingTask = false;
                        break;
                    }
                    if(mIsInterruptTask)
                    {
                        break;
                    }
                    usleep(mStepInterval_ms*1000);
                    if(mIsInterruptTask)
                    {
                        break;
                    }
                }
                mIsDoingTask = false;
                logDebug(AC, "->->handle the duck in task over");
            }
        }
        mIsRunning       = false;
    }
    catch(std::exception& e)
    {
        logError(AC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
    catch(...)
    {
        logError(AC, LOG_HEAD, "unexpected exception is caught!");
    }

    logVerbose(AC,"FadeInThread::Run()-->OUT");
}

void FadeInThread::stop()
{
    logVerbose(AC,"FadeInThread::stop()-->IN");

    while(mIsRunning){
        mIsSetToStop = true;
        // wake up fadein thread to let it finish.
        mConditionVariable.notify_one();
        usleep(500);
    }

    mThisThread.join();

    logVerbose(AC,"FadeInThread::stop()-->OUT");
}

void FadeInThread::start()
{
    logVerbose(AC,"FadeInThread::start()-->IN");
    if(!mIsRunning){
        mIsRunning = true;
        mThisThread = std::thread(std::bind(&FadeInThread::run, this));
    }else{
        logError(AC,"can't start because not stop thread");
    }
    logVerbose(AC,"FadeInThread::start()-->OUT");
}

void FadeInThread::planVolStepBySysDef(int fromGain,int toGain)
{
    logVerbose(AC,"FadeInThread::planVolStepBySysDef()-->IN");

    //10ms change 1dB
    if((toGain-fromGain)>0){
        mStepCnt = ((toGain-fromGain)/10);
        for(int i=0;i<=mStepCnt;i++){
            volStepArray[i] = fromGain+10*i;
        }
    }else{
        mStepCnt = ((fromGain-toGain)/10);
        for(int i=0;i<=mStepCnt;i++){
            volStepArray[i] = fromGain-10*i;
        }
    }

    mStepInterval_ms = SYSTEM_DEFAULT_INTERVAL;

    logVerbose(AC,"FadeInThread::planVolStepBySysDef()-->OUT");
}

void FadeInThread::planVolStepByDuck(int durations_ms,int fromGain,int toGain)
{
    logVerbose(AC,"FadeInThread::planVolStepByDuck()-->IN");

    fromGain = fromGain*100;
    toGain = toGain*100;

    //change volume every 50 ms
    mStepInterval_ms = 50;
    mStepCnt = (durations_ms/mStepInterval_ms);
    int volInterval;
    if((toGain-fromGain)>0){
        volInterval = (toGain-fromGain)/mStepCnt;
        for(int i=0;i<=mStepCnt;i++){
            volStepArray[i] = fromGain+volInterval*(i+1);
            volStepArray[i] = volStepArray[i]/100;
        }
    }else{
        volInterval = (fromGain-toGain)/mStepCnt;
        for(int i=0;i<=mStepCnt;i++){
            volStepArray[i] = fromGain-volInterval*(i+1);
            volStepArray[i] = volStepArray[i]/100;
        }
    }

    debugToPrintVolPlan();

    logVerbose(AC,"FadeInThread::planVolStepByDuck()-->OUT");
}

void FadeInThread::debugToPrintVolPlan(void)
{
    logVerbose(AC,"FadeInThread::debugToPrintVolPlan()-->IN");

    logDebug(AC,"mStepCnt:",mStepCnt);
    for(int i=0;i<=mStepCnt;i++){
        logDebugF(AC,"volStepArray[%d]=%d \n",i,volStepArray[i]);
    }

    logVerbose(AC,"FadeInThread::debugToPrintVolPlan()-->OUT");
}
