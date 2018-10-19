/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file UniqueID.h
/// @brief contains 
/// 
/// Created by zs on 2018/08/30.
///
/// this file contains the definition of class UniqueID
/// 

#ifndef UNIQUEID_H
#define UNIQUEID_H

extern "C" unsigned long long GetUniqueID(); // returns a non-zero ID. this fucntion is thread-safe. ID is unique within current process

#ifdef __cplusplus
class UniqueID{
public:
    UniqueID():id(0){};

    unsigned long long GetNewID(); // returns a non-zero ID

private:
    unsigned long long id;
};
#endif // __cplusplus

#endif // UNIQUEID_H
