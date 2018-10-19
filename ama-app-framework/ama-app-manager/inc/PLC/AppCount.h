/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppCount.h
/// @brief contains 
/// 
/// Created by zs on 2018/08/22.
///
/// this file contains the definition of class AppCount
/// 

#ifndef APPCOUNT_H
#define APPCOUNT_H

#include "COMMON/RedundantFileIO.h"

class AppCount
{
public:
    AppCount();
    virtual ~AppCount();

    void SetCurrentAppCount(unsigned int count);
    bool IsThereMissingApps()const;

protected:
    void Serialize(const std::string& path);
    bool Deserialize(const std::string& path);

private:
    RedundantFileIO redundantFileIO;
    unsigned int lastAppCount;
    unsigned int currentAppCount;
};


#endif // APPCOUNT_H
