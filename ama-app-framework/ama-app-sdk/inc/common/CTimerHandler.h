/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef __C_TIMER_HANDLER_H__
#define __C_TIMER_HANDLER_H__

#include <stdint.h>
#include <list>
#include <map>

#include <iostream> //todo: remove me

#include "ama_types.h"
#include "appCommon.h"

#define MAX_NS 1000000000L

typedef uint16_t timerHandle_t; //!<this is a handle for a timer to be used with the TimerHandler

/**
 * prototype for the timer callback
 */
class ITimerCallBack
{
public:
    virtual void Call(const timerHandle_t handle, void* userData)=0;
    virtual ~ITimerCallBack()
    {
    }
    ;
};

class CTimerHandler
{
public:
    CTimerHandler();
    ~CTimerHandler();

    E_ERROR addTimer(const timespec timeouts, ITimerCallBack* callback, timerHandle_t& handle, void* userData);
    E_ERROR removeTimer(const timerHandle_t handle);
    E_ERROR restartTimer(const timerHandle_t handle);
    E_ERROR updateTimer(const timerHandle_t handle, const timespec timeouts);
    E_ERROR stopTimer(const timerHandle_t handle);
    void startRunning();
    void stopRunning();

private:

    static CTimerHandler* mInstance;
    int mIsRunning; //this starts / stops the mainloop
    struct timer_s //!<struct that holds information of timers
    {
        timerHandle_t handle; //!<the handle of the timer
        timespec countdown; //!<the countdown, this value is decreased every time the timer is up
        ITimerCallBack* callback; //!<the callbackfunction
        void * userData; //!<saves a void pointer together with the rest.
    };

    typedef std::reverse_iterator<timer_s> rListTimerIter; //!<typedef for reverseiterator on timer lists

    void timerUp();
    void timerCorrection();
    timespec* insertTime(timespec& buffertime);

    /**
     * compares countdown values
     * @param a
     * @param b
     * @return true if b greater a
     */
    inline static bool compareCountdown(const timer_s& a, const timer_s& b)
    {
        return ((a.countdown.tv_sec == b.countdown.tv_sec) ? (a.countdown.tv_nsec < b.countdown.tv_nsec) : (a.countdown.tv_sec < b.countdown.tv_sec));
    }

    /**
     * Subtracts b from a
     * @param a
     * @param b
     * @return subtracted value
     */
    inline static timespec timespecSub(const timespec& a, const timespec& b)
    {
        timespec result;

        if ((a.tv_sec < b.tv_sec) || ((a.tv_sec == b.tv_sec) && (a.tv_nsec <= b.tv_nsec)))
        {
            result.tv_sec = result.tv_nsec = 0;
        }
        else
        {
            result.tv_sec = a.tv_sec - b.tv_sec;
            if (a.tv_nsec < b.tv_nsec)
            {
                result.tv_nsec = a.tv_nsec + MAX_NS - b.tv_nsec;
                result.tv_sec--; /* Borrow a second. */
            }
            else
            {
                result.tv_nsec = a.tv_nsec - b.tv_nsec;
            }
        }
        return (result);
    }

    /**
     * adds timespec values
     * @param a
     * @param b
     * @return the added values
     */
    inline timespec timespecAdd(const timespec& a, const timespec& b)
    {
        timespec result;
        result.tv_sec = a.tv_sec + b.tv_sec;
        result.tv_nsec = a.tv_nsec + b.tv_nsec;
        if (result.tv_nsec >= MAX_NS)
        {
            result.tv_sec++;
            result.tv_nsec = result.tv_nsec - MAX_NS;
        }
        return (result);
    }

    /**
     * comapares timespec values
     * @param a
     * @param b
     * @return
     */
    inline int timespecCompare(const timespec& a, const timespec& b)
    {
        //less
        if (a.tv_sec < b.tv_sec)
            return (-1);
        //greater
        else if (a.tv_sec > b.tv_sec)
            return (1);
        //less
        else if (a.tv_nsec < b.tv_nsec)
            return (-1);
        //greater
        else if (a.tv_nsec > b.tv_nsec)
            return (1);
        //equal
        else
            return (0);
    }



    class CCallTimer //!<functor to call a timer
    {
    public:
        CCallTimer()
        {
        }
        ;
        void operator()(timer_s& row)
        {
            row.callback->Call(row.handle, row.userData);
        }
    };

    class CCountdownUp //!<functor that checks if a timer is up
    {
    private:
        timespec mDiffTime;
    public:
        CCountdownUp(const timespec& differenceTime) :
                mDiffTime(differenceTime)
        {
        }
        ;
        bool operator()(const timer_s& row)
        {
            timespec sub = timespecSub(row.countdown, mDiffTime);
            if (sub.tv_nsec == 0 && sub.tv_sec == 0)
                return (true);
            return (false);
        }
    };

    class CCountdownZero //!<functor that checks if a timer is zero
    {
    public:
        CCountdownZero()
        {
        }
        ;
        bool operator()(const timer_s& row)
        {
            if (row.countdown.tv_nsec == 0 && row.countdown.tv_sec == 0)
                return (true);
            return (false);
        }
    };

    class CSubstractTime //!<functor to easy substract from each countdown value
    {
    private:
        timespec param;
    public:
        CSubstractTime(timespec param) :
                param(param)
        {
        }
        inline void operator()(timer_s& t)
        {
            t.countdown = timespecSub(t.countdown, param);
        }
    };

    std::list<timer_s> mListTimer; //!<list of all timers
    std::list<timer_s> mListActiveTimer; //!<list of all currently active timers
    timerHandle_t mLastInsertedHandle; //!<keeps track of the last inserted timer handle
    timespec mStartTime; //!<here the actual time is saved for timecorrection

//    void debugPrintTimerList ()
//    {
//        std::list<timer_s>::iterator it(mListActiveTimer.begin());
//        for(;it!=mListActiveTimer.end();++it)
//        {
//            std::cout<< "Handle " << it->handle << "sec " << it->countdown.tv_sec << "nsec " << it->countdown.tv_nsec<<std::endl;
//        }
//    }
}
;

/**
 * template to create the functor for a class
 */
template<class TClass> class TTimerCallBack: public ITimerCallBack
{
private:
    TClass* mInstance;
    void (TClass::*mFunction)(timerHandle_t handle, void* userData);

public:
    TTimerCallBack(TClass* instance, void (TClass::*function)(timerHandle_t handle, void* userData)) :
            mInstance(instance), //
            mFunction(function)
    {
    }
    ;

    virtual void Call(timerHandle_t handle, void* userData)
    {
        (*mInstance.*mFunction)(handle, userData);
    }
};



#endif /* __C_TIMER_HANDLER_H__ */
