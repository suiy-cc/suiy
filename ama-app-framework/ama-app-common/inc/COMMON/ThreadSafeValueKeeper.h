/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
//
// Created by zs on 17-1-6.
//

#ifndef THREADSAFEVALUEKEEPER_H
#define THREADSAFEVALUEKEEPER_H

#include <mutex>

template<class T>
class ThreadSafeValueKeeper
{
public:
    ThreadSafeValueKeeper();
    explicit ThreadSafeValueKeeper(T);
    virtual ~ThreadSafeValueKeeper();

    void SetValue(T v);
    T GetValue()const;

    void Operate(const std::function<void(T&)>& _operator);
    void OperateConst(const std::function<void(const T&)>& _operator)const;

private:
    ThreadSafeValueKeeper(const ThreadSafeValueKeeper<T>&){}; // don't copy objects of this class
    void operator=(const ThreadSafeValueKeeper<T>& ){};

private:
    T value;
    mutable std::mutex _mutex;
};

template<class T>
ThreadSafeValueKeeper<T>::ThreadSafeValueKeeper()
{

}

template<class T>
ThreadSafeValueKeeper<T>::ThreadSafeValueKeeper(T value)
: value(value)
{

}

template<class T>
ThreadSafeValueKeeper<T>::~ThreadSafeValueKeeper()
{

}

template<class T>
void ThreadSafeValueKeeper<T>::SetValue(T v)
{
    std::unique_lock<std::mutex> lock(_mutex);

    value = v;
}

template<class T>
T ThreadSafeValueKeeper<T>::GetValue() const
{
    std::unique_lock<std::mutex> lock(_mutex);

    return value;
}

template<class T>
void ThreadSafeValueKeeper<T>::Operate(const std::function<void(T&)>& _operator)
{
    std::unique_lock<std::mutex> lock(_mutex);

    _operator(value);
}

template<class T>
void ThreadSafeValueKeeper<T>::OperateConst(const std::function<void(const T&)>& _operator)const
{
    std::unique_lock<std::mutex> lock(_mutex);

    _operator(value);
}

#endif // THREADSAFEVALUEKEEPER_H
