/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUI.cpp
/// @brief contains UI APIs
///
/// Created by zs on 2016/10/31.
/// this file contains the definition of UI APIs
///

#include "AppManager.h"
#include "AppUI.h"
#include "ILMClient.h"
#include "Surface.h"
#include "AppPLC.h"
#include "LogContext.h"

static bool isILMInitialized = false;

static void InitializeILM()
{
    if(isILMInitialized)
        return;

    ILMClient::Initialize();
    isILMInitialized = true;

    logInfo(SDK_AMGR, "ILMClient::Initialize() is invoked.");
}

bool GetDestinationRectangle(int &x, int &y, unsigned int &width, unsigned int &height)
{
    InitializeILM();

    unsigned int surfaceID = GetSurfaceID();
    Surface surface(surfaceID);
    auto properties = surface.GetProperties();

    x = properties->destX;
    y = properties->destY;
    width = properties->destWidth;
    height = properties->destHeight;

    return true;
}

void SetDestinationRectangle(const int& x, const int& y, const unsigned int& width, const unsigned int& height)
{
    InitializeILM();

    unsigned int surfaceID = GetSurfaceID();
    Surface surface(surfaceID);

    surface.SetDestinationRectangle(Section{x, y, width, height});
}

bool GetSourceRectangle(int &x, int &y, unsigned int &width, unsigned int &height)
{
    InitializeILM();

    unsigned int surfaceID = GetSurfaceID();
    Surface surface(surfaceID);
    auto properties = surface.GetProperties();

    x = properties->sourceX;
    y = properties->sourceY;
    width = properties->sourceWidth;
    height = properties->sourceHeight;

    return true;
}

void SetSourceRectangle(const int& x, const int& y, const unsigned int& width, const unsigned int& height)
{
    InitializeILM();

    unsigned int surfaceID = GetSurfaceID();
    Surface surface(surfaceID);

    surface.SetSourceRectangle(Section{x, y, width, height});
}

void SetBothRectangle(const int& x, const int& y, const unsigned int& width, const unsigned int& height)
{
    InitializeILM();

    unsigned int surfaceID = GetSurfaceID();
    Surface surface(surfaceID);

    surface.SetBothRectangle(Section{x, y, width, height});
}

void SetUnderLayerCenter(bool isCenter)
{
    AppManager::GetInstance()->SetUnderLayerCenter(isCenter);
}

bool IsUnderLayerCenter()
{
    AppManager::GetInstance()->IsUnderLayerCenter();
}

void SetTitlebarVisibility(bool isVisible, int style)
{
    unsigned int proc = GetAMPID();
    if(proc)
    {
        AppManager::GetInstance()->SetTitlebarVisibility(proc, isVisible, style);
    }
}

void ShowTitlebar(int style/* = 0*/)
{
    SetTitlebarVisibility(true, style);
}

void HideTitlebar(int style/* = 0*/)
{
    SetTitlebarVisibility(false, style);
}

bool IsTitlebarVisible()
{
    return AppManager::GetInstance()->IsTitlebarVisible();
}

void DropDownTitlebar(bool isDropDown/* = true*/, int style/* = 0*/)
{
    AppManager::GetInstance()->DropDownTitlebar(isDropDown, style);
}

bool IsTitlebarDropDown()
{
    return AppManager::GetInstance()->IsTitlebarDropDown();
}

void EnableTouch(bool enable)
{
    AppManager::GetInstance()->EnableTouch(enable);
}

std::vector<E_SEAT> GetScreens()
{
    return AppManager::GetInstance()->GetScreens();
}

void GetScreenSize(E_SEAT seat, unsigned int& width, unsigned int& height)
{
    AppManager::GetInstance()->GetScreenSize(seat, width, height);
}

void DontPopMe()
{
    AppManager::GetInstance()->RemoveFromZOrder(AppManager::GetInstance()->GetAMPID());
}

void DontPopApp(unsigned int app)
{
    AppManager::GetInstance()->RemoveFromZOrder(app);
}

void DontPopApp(const std::string& itemID)
{
    auto app = AppManager::GetInstance()->GetAMPID(itemID);

    if(app!=INVALID_AMPID)
        AppManager::GetInstance()->RemoveFromZOrder(app);
    else
        logError(SDK_AMGR, LOG_HEAD, "invalid item ID: ", itemID);
}

void AddExtraSurface(unsigned int surface, unsigned int layer)
{
    AppManager::GetInstance()->AddExtraSurface(surface, layer);
}