/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APPSDK_H
#define APPSDK_H

#include <ama_types.h>
#include <ama_audioTypes.h>
#include <ama_setTypes.h>
#include <AppAudioStream.h>
#include <AudioStream.h>
#include <AppPLC.h>
#include <AppSM.h>
#include <AppCS.h>
#include <AppUI.h>
#include <AppTools.h>
#include <AppShare.h>
#include <AppAudio.h>
#include <AppSet.h>
#include <AppPower.h>
#include <AppBroadcast.h>

class SDKProperty{
public:
    const char* appLogID;
    E_SEAT seatOfAnonymousProcess;
    const char* appDescription;

public:
    SDKProperty();
};

void AppSDKInit(); // DEPRECATED!!!
void AppSDKInit(const char* appLogID); // DEPRECATED!!!
void AppSDKInit(const char* appLogID, const char* appDescription);
void AppSDKInit(const SDKProperty& property);
void AppSDKInitAsync(const SDKProperty& property);
void AppSDKInitJoin();

#endif /* APPSDK_H */
