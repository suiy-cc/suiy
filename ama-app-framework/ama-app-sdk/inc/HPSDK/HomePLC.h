/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HPAppPLC.h
/// @brief contains High-priorty AppPLC APIs
///
/// Created by zs on 2016/8/16.
/// this file contains the declaration of High-priorty AppPLC APIs
///

#ifndef HOMEPLC_H
#define HOMEPLC_H

#include "AppPLC.h"
#include "AppPKG.h"

#include <vector>

class AppPKGInfo;

class HomePLC
: public AppPLC
{
public:
    HomePLC();
    virtual ~HomePLC();

protected:
    friend class HomePLCImplementation;
    virtual void OnInstalledPKGsChanged()=0;
private:
    virtual void *GetImplementation()override;

};

// app operation
void StopApp(unsigned int app);
void HideApp(unsigned int app);

// package operation
std::vector<AppPKG> GetAppList();
AppPKG GetAppPKGInfo(unsigned int);
std::vector<AppPKGEx> GetAppListEx();
AppPKGEx GetAppPKGInfoEx(unsigned int);
AppPKGEx ChangeToAppPKGInfoEx(AppPKGInfo appPKGInfo);
AppPKG ChangeToAppPKGInfo(AppPKGInfo appPKGInfo);

#endif // HOMEPLC_H
