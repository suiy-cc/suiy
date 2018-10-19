/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file LLC.h
/// @brief contains class LLC
/// 
/// Created by zs on 2017/03/27.
///
/// this file contains the definition of class LLC
/// 

#ifndef LLC_H
#define LLC_H

#include <string>
#include <libLifecycle.hpp>

#include <COMMON/Broadcaster.h>

class LLC
{
private:
    LLC();
    virtual ~LLC();

public:
    static LLC* GetInstance();
    LLC_Result Start(const std::string &unitName, const char **argv, void *userData);
    LLC_Result Stop(const std::string &unitName, void *userData);
    LLC_Result GetState(const std::string &unitName, bool &isActive) const;
    bool IsInitOK()const{return isInitOK;};

    Broadcaster<LLC_StartCb> startObserver;
    Broadcaster<LLC_StopCb> stopObserver;
    Broadcaster<LLC_UnitChangedCb> statusObserver;

private:
    LLC_Result Listen2UnitStateChange();
    static void startUnitCB(_Bool result, uint32_t pid, void *userData);
    static void stopUnitCB(_Bool result, void *userData);
    static void unitStateChangeCB(const char *unitName, _Bool status);

    bool isInitOK;
};


#endif // LLC_H
