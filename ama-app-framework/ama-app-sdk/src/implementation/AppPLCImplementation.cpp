/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPLCImplementation.cpp
/// @brief contains the implementation of class AppPLCImplementation
///
/// Created by zs on 2016/7/26.
/// this file contains the implementation of class AppPLCImplementation
///

#include <cstdio>
#include <thread>
#include <mutex>
#include <cstring>

#include <signal.h>
#include <unistd.h>

#include "ama_types.h"
#include "AppPLCImplementation.h"
#include "AppPLC.h"
#include "AppManager.h"
#include "ZoomInAnimateStrategy.h"
#include "ILMClient.h"
#include "ILMException.h"
#include "LogContext.h"
#include "AppIME.h"
#include "SurfaceID.h"

using namespace std;

const unsigned int AppPLCImplementation::fastResumeDuration = 50;

AppPLCImplementation::AppPLCImplementation(AppPLC* appPLC)
    : surface(-1)
    , startupScreen(INVALID_SURFACE)
    , resumeScreen(INVALID_SURFACE)
    , plcInterface(appPLC)
    , isStartupScreenReady(false)
{
    ;
}

AppPLCImplementation::~AppPLCImplementation()
{
}

void AppPLCImplementation::OnNotifyEarlyInitialize()
{
    // send initialize notify to app
    //plcInterface->OnInitialize();
}

static void PlayStartupAnimation();
static void PlayZoomInAnimation(Surface &startupScreen, Size startSize, Position startPosition);
void AppPLCImplementation::OnNotifyInitialize(int startArg, bool visible)
{
    // send create notify to app
    plcInterface->OnCreate();
}


void AppPLCImplementation::OnNotifyStop(const std::string &stopCMD)
{
    // send stop notify to app
    plcInterface->OnStop();
}

static void ShowResumeScreen(Surface &resumeScreen, unsigned int fastResumeDuration, const Surface &mainSurface, const std::string &resumePicture, bool &isResuming, std::mutex &mutexForIsResuming);
void AppPLCImplementation::OnNotifyResume()
{
    // send resume notify to app
    plcInterface->OnResume();

}

void AppPLCImplementation::OnNotifyPreHide()
{
    // send pre-hide notify to app
    plcInterface->OnPreHide();

}

void AppPLCImplementation::OnNotifyHide()
{
    // taking a screen shot as resume picture

    plcInterface->OnHide();
}


void AppPLCImplementation::OnNotifyPreShow(int nArgs)
{
    DomainVerboseLog chatter(SDK_PLC, "AppPLCImplementation::OnNotifyPreShow()");

    //AppManager::GetInstance()->Move2BottomLayer();
    plcInterface->OnPreShow(nArgs);
    //AppManager::GetInstance()->Return2HomeLayer();

}
void AppPLCImplementation::OnNotifyActiveAppChange(unsigned int seatid)
{

    DomainVerboseLog chatter(SDK_PLC, "AppPLCImplementation::OnNotifyActiveAppChange()");
    plcInterface->OnActiveAppChanged(seatid);
}

void AppPLCImplementation::OnNotifyShow()
{
    DomainVerboseLog chatter(SDK_PLC, "AppPLCImplementation::OnNotifyShow()");

    plcInterface->OnShow();
}

bool AppPLCImplementation::Initialize(unsigned int ampid)
{
    if(ampid == 0)
        return false;

    // initialize surface id
    surface.SetID(GetSurfaceID());
    logDebug(SDK_PLC, "surface ID: ", ToHEX(surface.GetID()));

    // initialize connection to app-manager
    AppManager::GetInstance()->SetNotifyReceiver(this);

    // create surface for startup screen
//    if(plcInterface->ShowStartupScreen())
//        InitializeStartupScreen();

    return true;
}

void AppPLCImplementation::SetStartupPicture(const std::string& path)
{
    startupPicture = path;
}

AppPLC* AppPLCImplementation::GetPLCInterface()
{
    return plcInterface;
}

void PlayZoomInAnimation(Surface &startupScreen, Size startSize, Position startPosition)
{
    if(startupScreen!=INVALID_SURFACE)
    {
        try
        {
            const E_SEAT seat = GetSeat(startupScreen);
            Size               screenSize{1280, 720};// = screens[seat].GetSize();

            // configure animation
            auto animate = make_shared<ZoomInAnimateStrategy>();
            animate->SetStartSize(startSize);
            animate->SetEndSize(screenSize);
            animate->SetStartPosition(startPosition);

            // show startup screen
            startupScreen.SetVisible();
            // play animation
            animate->Play(
                [&startupScreen](const Section &section)
                {
                    startupScreen.SetDestinationRectangle(section);
                    ILMClient::Commit();
                }
                         );
        }
        catch(ILMException e)
        {
            logError(SDK_PLC, "exception is caught in PlayZoomInAnimation():", e.what(), e.GetWhere());
        }
        catch(...)
        {
            logError(SDK_PLC, "unexpected exception is caught in PlayZoomInAnimation()!");
        }

    }
}
