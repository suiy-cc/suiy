/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppSDK.cpp
/// @brief contains the implementation of APPSDK's APIs
///
/// Created by zs on 2016/8/4.
/// this file contains the implementation of APPSDK's APIs
///

#include <thread>

#include "AppSDK.h"
#include "AppManager.h"
#include "BootArguments.h"
#include "log.h"
#include "CMDOption.h"

#include "ama_mem.h"

static CMDOption CMDAMPID("AMPID");

SDKProperty::SDKProperty()
: appLogID(nullptr)
, appDescription(nullptr)
, seatOfAnonymousProcess(E_SEAT_NONE)
{
    ;
}

void AppSDKInit()
{
    AppSDKInit(nullptr);
}

void AppSDKInit(const char* appLogID)
{
    SDKProperty property;
    property.appLogID = appLogID;

    AppSDKInit(property);
}

void AppSDKInit(const char* appLogID, const char* appDescription)
{
    SDKProperty property;
    property.appLogID = appLogID;
    property.appDescription = appDescription;

    AppSDKInit(property);
}

void AppSDKInit(const SDKProperty& property)
{
    unsigned int AMPID = 0;
    int retryCount = 256;
    std::vector<std::string> argv;
    while(retryCount>0)
    {
        argv = GetBootArguments();

        unsigned int AMPIDOptionCount = 0;
        for(auto &option: argv)
        {
            std::cout << option << std::endl;
            if(::CMDAMPID.FromString(option))
            {
                AMPID = static_cast<unsigned int>(std::stoul(::CMDAMPID.GetValue(), 0, 0));
                ++AMPIDOptionCount;
            }
        }

        // output the count of option "AMPID"
        std::cout << "\"AMPID\" option appears " << AMPIDOptionCount << " time(s)." << std::endl;
        if(AMPIDOptionCount==1)
            break;
        else if(AMPIDOptionCount==0)
        {
            std::cout << "no \"AMPID\" option is found, use anonymous AMPID." << std::endl;
            AppManager::GetInstance()->UseAnonymousAMPID();
            break;
        }

        // output countdown
        --retryCount;
        std::cout << "retry countdown = " << retryCount << std::endl;
    }

    AppManager::GetInstance()->SetAMPID(AMPID);
    AppManager::GetInstance()->SetAppLogID(property.appLogID);
    AppManager::GetInstance()->SetSeatHint(property.seatOfAnonymousProcess);
    AppManager::GetInstance()->SetAppDescription(property.appDescription);
    AppManager::GetInstance()->Initialize();

    if(AppManager::GetInstance()->GetAMPID()==0)
    {
        logError("App SDK is unable to get necessary boot arguments.");
        logError("argument list:");
        for(auto line: argv)
            logError(line);

        logError("ama-app can't boot without valid boot arguments, exit.");
        exit(-1);
    }
    AppManager::GetInstance()->SendBootLog();
}

static void CopyProperty(const SDKProperty& src, SDKProperty& des)
{
    if(src.appLogID)
    {
        char* tmp = new char[strlen(src.appLogID)+1];
        memset(tmp, 0, strlen(src.appLogID)+1);
        strncpy(tmp, src.appLogID, strlen(src.appLogID)+1);
        des.appLogID = tmp;
    }

    if(src.appDescription)
    {
        char* tmp = new char[strlen(src.appDescription)+1];
        memset(tmp, 0, strlen(src.appDescription)+1);
        strncpy(tmp, src.appDescription, strlen(src.appDescription)+1);
        des.appDescription = tmp;
    }

    des.seatOfAnonymousProcess = src.seatOfAnonymousProcess;
}

static std::mutex mutex4InitThread;
static std::shared_ptr<std::thread> initThread = nullptr;
void AppSDKInitAsync(const SDKProperty& property)
{
    std::unique_lock<std::mutex> lock(mutex4InitThread);

    if(!initThread)
    {
        SDKProperty runtimeProperty;
        CopyProperty(property, runtimeProperty);

        initThread = std::make_shared<std::thread>([runtimeProperty](){
            AppSDKInit(runtimeProperty);
            delete [] runtimeProperty.appLogID;
            delete [] runtimeProperty.appDescription;
        });
    }
}

void AppSDKInitJoin()
{
    std::unique_lock<std::mutex> lock(mutex4InitThread);

    if(initThread)
    {
        initThread->join();
        initThread = nullptr;
    }
}