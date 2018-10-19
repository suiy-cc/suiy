/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>
#include <atomic>

using namespace std;
using namespace std::chrono;

class Timer
{
public:
    Timer() : begin_sec_(0) { }
    void reset() { begin_sec_ = 0 /*high_resolution_clock::now()*/; }

    // millisecond
    // template<typename Duration = milliseconds>
    // int64_t elapsed() const
    // {
    //     return duration_cast<Duration>(high_resolution_clock::now() - begin_).count();
    // }
    // // microsecond
    // int64_t elapsed_micro() const
    // {
    //     return elapsed<microseconds>();
    // }
    // // nanosecond
    // int64_t elapsed_nono() const
    // {
    //     return elapsed<nanoseconds>();
    // }
    // second
    int64_t elapsed_seconds() const
    {
        return begin_sec_;
        //return elapsed<seconds>();
    }
    // // minute
    // int64_t elapsed_minutes() const
    // {
    //     return elapsed<minutes>();
    // }
    // // hour
    // int64_t elapsed_hours() const
    // {
    //     return elapsed<hours>();
    // }

    void tick()
    {
        ++begin_sec_;
    }

private:
    //time_point<high_resolution_clock> begin_;
    atomic_ulong begin_sec_;
};
#endif
