/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */


#include <cassert>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/poll.h>
#include <time.h>
#include <algorithm>
#include <features.h>
#include <csignal>
#include <unistd.h>
#include <vector>

#include "CTimerHandler.h"
#include "CDltWrapper.h"

CTimerHandler::CTimerHandler() :
        mIsRunning(0),//
        mListTimer(), //
        mListActiveTimer(), //
        mLastInsertedHandle(0), //
        mStartTime() //
{
    
}

CTimerHandler::~CTimerHandler()
{
}


//todo: maybe have some: give me more time returned?
/**
 * start the block listening for filedescriptors. This is the mainloop.
 */
void CTimerHandler::startRunning()
{
    logInfo("CTimerHandler::startRunning()-->>");
    mIsRunning = 1;

    clock_gettime(CLOCK_MONOTONIC, &mStartTime);
    while (mIsRunning)
    {
        timerCorrection();

        //this was a timer event, we need to take care about the timers
        timerUp();
    }
}

/**
 * exits the loop
 */
void CTimerHandler::stopRunning()
{
    mIsRunning = 0;

    //this is for all running timers only - we need to handle the additional offset here
    if (!mListActiveTimer.empty())
    {
        timespec currentTime, correctionTime;
        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        correctionTime = timespecSub(currentTime, mStartTime);
        std::for_each(mListActiveTimer.begin(), mListActiveTimer.end(), CSubstractTime(correctionTime));
    }

}

/**
 * adds a timer to the list of timers. The callback will be fired when the timer is up.
 * This is not a high precise timer, it is very coarse. It is meant to be used for timeouts when waiting
 * for an answer via a filedescriptor.
 * One time timer. If you need again a timer, you need to add a new timer in the callback of the old one.
 * @param timeouts timeouts time until the callback is fired
 * @param callback callback the callback
 * @param handle handle the handle that is created for the timer is returned. Can be used to remove the timer
 * @param userData pointer always passed with the call
 * @return E_OK in case of success
 */
E_ERROR CTimerHandler::addTimer(const timespec timeouts, ITimerCallBack* callback, timerHandle_t& handle, void * userData)
{
    assert(!((timeouts.tv_sec==0) && (timeouts.tv_nsec==0)));
    assert(callback!=NULL);

    timer_s timerItem;

    //create a new handle for the timer
    handle = ++mLastInsertedHandle; //todo: overflow ruling !o
    timerItem.handle = handle;
    timerItem.countdown = timeouts;
    timerItem.callback = callback;
    timerItem.userData = userData;

    mListTimer.push_back(timerItem);

    //we add here the time difference between startTime and currenttime, because this time will be substracted later on in timecorrection
    timespec currentTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    if (mIsRunning) //the mainloop is started
        timerItem.countdown = timespecAdd(timeouts, timespecSub(currentTime, mStartTime));

    mListActiveTimer.push_back(timerItem);
    mListActiveTimer.sort(compareCountdown);
    return (E_OK);
}

/**
 * removes a timer from the list of timers
 * @param handle the handle to the timer
 * @return E_OK in case of success, E_UNKNOWN if timer was not found.
 */
E_ERROR CTimerHandler::removeTimer(const timerHandle_t handle)
{
    assert(handle!=0);

    //stop the current timer
    stopTimer(handle);

    std::list<timer_s>::iterator it(mListTimer.begin());
    for (; it != mListTimer.end(); ++it)
    {
        if (it->handle == handle)
        {
            it = mListTimer.erase(it);
            return (E_OK);
        }
    }
    return (E_UNKNOWN);
}

/**
 * restarts a timer and updates with a new interva
 * @param handle handle to the timer
 * @param timeouts new timout time
 * @return E_OK on success, E_NON_EXISTENT if the handle was not found
 */
E_ERROR CTimerHandler::updateTimer(const timerHandle_t handle, const timespec timeouts)
{
    //update the mList ....
    timer_s timerItem;
    std::list<timer_s>::iterator it(mListTimer.begin()), activeIt(mListActiveTimer.begin());
    bool found(false);
    for (; it != mListTimer.end(); ++it)
    {
        if (it->handle == handle)
        {
            it->countdown = timeouts;
            timerItem = *it;
            found = true;
            break;
        }
    }
    if (!found)
        return (E_NON_EXISTENT);

    found = false;

    //we add here the time difference between startTime and currenttime, because this time will be substracted later on in timecorrection
    timespec currentTime, timeoutsCorrected;
    currentTime.tv_nsec=timeoutsCorrected.tv_nsec=0;
    currentTime.tv_sec=timeoutsCorrected.tv_sec=0;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    if (mIsRunning) //the mainloop is started
        timeoutsCorrected = timespecAdd(timeouts, timespecSub(currentTime, mStartTime));

    for (; activeIt != mListActiveTimer.end(); ++activeIt)
    {
        if (activeIt->handle == handle)
        {
            activeIt->countdown = timeoutsCorrected;
            found = true;
            break;
        }
    }

    if (!found)
        timerItem.countdown = timeoutsCorrected;
    mListActiveTimer.push_back(timerItem);

    mListActiveTimer.sort(compareCountdown);
    return (E_OK);
}

/**
 * restarts a timer with the original value
 * @param handle
 * @return E_OK on success, E_NON_EXISTENT if the handle was not found
 */
E_ERROR CTimerHandler::restartTimer(const timerHandle_t handle)
{
    timer_s timerItem; //!<the original timer value
    //find the original value
    std::list<timer_s>::iterator it(mListTimer.begin()), activeIt(mListActiveTimer.begin());
    bool found(false);
    for (; it != mListTimer.end(); ++it)
    {
        if (it->handle == handle)
        {
            timerItem = *it;
            found = true;
            break;
        }
    }
    if (!found)
        return (E_NON_EXISTENT);

    found = false;

    //we add here the time difference between startTime and currenttime, because this time will be substracted later on in timecorrection
    timespec currentTime, timeoutsCorrected;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    if (mIsRunning) //the mainloop is started
    {
        timeoutsCorrected = timespecAdd(timerItem.countdown, timespecSub(currentTime, mStartTime));
        timerItem.countdown = timeoutsCorrected;
    }

    for (; activeIt != mListActiveTimer.end(); ++activeIt)
    {
        if (activeIt->handle == handle)
        {
            activeIt->countdown = timerItem.countdown;
            found = true;
            break;
        }
    }

    if (!found)
        mListActiveTimer.push_back(timerItem);

    mListActiveTimer.sort(compareCountdown);

    return (E_OK);
}

/**
 * stops a timer
 * @param handle
 * @return E_OK on success, E_NON_EXISTENT if the handle was not found
 */
E_ERROR CTimerHandler::stopTimer(const timerHandle_t handle)
{
    //go through the list and remove the timer with the handle
    std::list<timer_s>::iterator it(mListActiveTimer.begin());
    for (; it != mListActiveTimer.end(); ++it)
    {
        if (it->handle == handle)
        {
            it = mListActiveTimer.erase(it);
            return (E_OK);
        }
    }
    return (E_NON_EXISTENT);
}

/**
 * timer is up.
 */
void CTimerHandler::timerUp()
{
    //find out the timedifference to starttime
    timespec currentTime, diffTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    diffTime = timespecSub(currentTime, mStartTime);

    //now we need to substract the diffTime from all timers and see if they are up
    std::list<timer_s>::reverse_iterator overflowIter = std::find_if(mListActiveTimer.rbegin(), mListActiveTimer.rend(), CCountdownUp(diffTime));

    //copy all fired timers into a list
    std::vector<timer_s> tempList(overflowIter, mListActiveTimer.rend());

    //erase all fired timers
    std::list<timer_s>::iterator it(overflowIter.base());
    mListActiveTimer.erase(mListActiveTimer.begin(), it);

    //call the callbacks for the timers
    std::for_each(tempList.begin(), tempList.end(), CCallTimer());
}

/**
 * correct timers and fire the ones who are up
 */
void CTimerHandler::timerCorrection()
{
    //get the current time and calculate the correction value
    timespec currentTime, correctionTime;
    clock_gettime(CLOCK_MONOTONIC, &currentTime);
    correctionTime = timespecSub(currentTime, mStartTime);
    mStartTime = currentTime;

    if (!mListActiveTimer.empty())
    {

        //subtract the correction value from all items in the list
        std::for_each(mListActiveTimer.begin(), mListActiveTimer.end(), CSubstractTime(correctionTime));

        //find the last occurrence of zero -> timer overflowed
        std::list<timer_s>::reverse_iterator overflowIter = std::find_if(mListActiveTimer.rbegin(), mListActiveTimer.rend(), CCountdownZero());

        //only if a timer overflowed
        if (overflowIter != mListActiveTimer.rend())
        {
            //copy all timers that need to be called to a new list
            std::vector<timer_s> tempList(overflowIter, mListActiveTimer.rend());

            //erase all fired timers
            std::list<timer_s>::iterator it(overflowIter.base());
            mListActiveTimer.erase(mListActiveTimer.begin(), it);

            //call the callbacks for the timers
            std::for_each(tempList.begin(), tempList.end(), CCallTimer());
        }
    }
}

/**
 * is used to set the pointer for the ppoll command
 * @param buffertime
 * @return
 */
inline timespec* CTimerHandler::insertTime(timespec& buffertime)
{
    if (!mListActiveTimer.empty())
    {
        buffertime = mListActiveTimer.front().countdown;
        return (&buffertime);
    }
    else
    {
        return (NULL);
    }
}
