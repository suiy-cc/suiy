/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ComparableFunction.h
/// @brief contains class ComparableFunction
/// 
/// Created by zs on 2017/08/24.
///

#ifndef COMPARABLEFUNCTION_H
#define COMPARABLEFUNCTION_H

#include <functional>

template<typename T, typename... U>
size_t GetAddress(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

template<typename T>
class ComparableFunction{
    T functionObj;
public:
    ComparableFunction()
        : functionObj()
    {}
    ComparableFunction(const T &f)
        : functionObj(f)
    {}

    bool operator==(const ComparableFunction<T>& f)const
    {
        return GetAddress(functionObj)==GetAddress(f.Get());
    }

    bool operator<(const ComparableFunction<T>& f)const
    {
        return GetAddress(functionObj)<GetAddress(f.Get());
    }

    void Set(const T& f)
    {
        functionObj = f;
    }

    T Get()const
    {
        return functionObj;
    }
};

namespace std
{
template <typename T>
struct hash<ComparableFunction<T>>
{
    std::size_t operator()(const ComparableFunction<T> &key) const
    {
        return GetAddress(key.Get());
    }
};
}

#endif // COMPARABLEFUNCTION_H
