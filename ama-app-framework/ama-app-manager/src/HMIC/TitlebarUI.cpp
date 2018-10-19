/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TitlebarUI.cpp
/// @brief contains the implementation of class TitlebarUI
///
/// Created by zs on 2016/8/29.
/// this file contains the implementation of class TitlebarUI
///

#include "ILMException.h"
#include "AMGRLogContext.h"
#include "TitlebarUI.h"

const unsigned int TitlebarUI::titlebarHeight = 70;

TitlebarUI::TitlebarUI()
{
    ;
}

TitlebarUI::TitlebarUI(const Surface &surface)
    : AppUIHMI(surface)
{
    ;
}

TitlebarUI::~TitlebarUI()
{

}

void TitlebarUI::OnExpand()
{
    logVerbose(HMIC, "TitlebarUI::OnExpand()--->IN");

    try
    {
        Section section = _GetRectangle();
        _ConfigSurface(GetSurfaces().front(), section, section.height);
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in TitlebarUI::OnExpand()!");
    }

    logVerbose(HMIC, "TitlebarUI::OnExpand()--->OUT");
}

void TitlebarUI::OnShrink()
{
    logVerbose(HMIC, "TitlebarUI::OnShrink()--->IN");

    try
    {
        _ConfigSurface(GetSurfaces().front(), _GetRectangle(), titlebarHeight);
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in TitlebarUI::OnShrink()!");
    }

    logVerbose(HMIC, "TitlebarUI::OnShrink()--->OUT");
}

std::vector<Surface> TitlebarUI::GetSynchronizableSurfaces()const
{
    return std::vector<Surface>();
}

void TitlebarUI::_ConfigSurface(Surface &surface, const Section &section, const unsigned int height)
{
    DomainVerboseLog chatter(HMIC, formatText("TitlebarUI::_ConfigSurface(%#x,%d)", surface.GetID(), height));
    try
    {
        Layer layer( surface.GetLayerID() );

        // set surface size
        Section sec = section;
        sec.height = height;
        surface.SetBothRectangle(sec);

        //    // add surface to layer
        //    layer.AddSurface(surface);
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
}

void TitlebarUI::_ConfigSurface(Surface &surface, const Section &section)
{
    DomainVerboseLog chatter(HMIC, formatText("TitlebarUI::_ConfigSurface(%#x)", surface.GetID()));

    // if you don't set source/destination rectangle here, titlebar will flikers on cluster!!!
    _ConfigSurface(surface, section, titlebarHeight);
}
