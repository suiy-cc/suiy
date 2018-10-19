/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file NotifyWaiter.h
/// @brief contains declaration of functions used to do asynchronise operation
///
/// Created by zs on 2016/9/13.
///
///

#ifndef NOTIFYWAITER_H
#define NOTIFYWAITER_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// expected usage:
// register -> wait -> notify -> un-register
// register -> notify -> wait -> un-register
// see? invoke each function only once.

void RegisterWaiterID(unsigned long long id);

// NOTE: don't wait one id more than once.
//       if you do need to do that, re-register.
#ifdef __cplusplus
bool Wait(unsigned long long id, unsigned int timeout_ms = 300);
#else
bool Wait(unsigned long long id, unsigned int timeout_ms);
#endif

void Notify(unsigned long long id);

void UnregisterWaiterID(unsigned long long id);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // NOTIFYWAITER_H
