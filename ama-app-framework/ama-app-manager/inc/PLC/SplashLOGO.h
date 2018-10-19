/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file SplashLOGO.h
/// @brief contains class SplashLOGO
/// 
/// Created by zs on 2017/02/06.
/// this file contains the definition of class ActiveAppHome
///

#ifndef SPLASHLOGO_H
#define SPLASHLOGO_H

#include <string>

#include "ama_types.h"

class SplashLOGO
{
public:
    virtual ~SplashLOGO();

    static SplashLOGO* GetInstance();

    void SetStateFile(const std::string& value);
    std::string GetStateFile()const;

    void NotifyHomeReady(AMPID home);

private:
    SplashLOGO();

    static const std::string fileName;
};


#endif // SPLASHLOGO_H
