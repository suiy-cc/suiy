/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file IMEUI.cpp
/// @brief contains the implementation of class IMEUI
///
/// Created by zs on 2016/10/10.
/// this file contains the implementation of class IMEUI
///

#include "ILMException.h"
#include "IMEUI.h"
#include "log.h"
#include "ILMClient.h"
#include "AMGRLogContext.h"
#include "ID.h"

IMEUI::IMEUI()
: targetApp(INVALID_AMPID)
, IMEHeight(400)
{}

IMEUI::IMEUI(const Surface &surface)
    : targetApp(INVALID_AMPID)
    , IMEHeight(400)
    , AppUIHMI(surface)
{}

IMEUI::~IMEUI()
{

}

//void IMEUI::SetVisible(const bool isVisible)
//{
//    DomainVerboseLog chatter(HMIC, formatText("IMEUI::SetVisible(%s)", isVisible?"true":"false"));
//
//    try
//    {
//        if(GetSurfaces().size()>0)
//        {
//            GetSurfaces().front().SetVisible(isVisible);
//
//            ILMClient::Commit();
//        }
//    }
//    catch(ILMException e)
//    {
//        logWarn(HMIC, e.what(), " caught in IMEUI::SetVisible()!");
//    }
//
//    AppUIHMI::SetVisible(isVisible);
//}

LayerID IMEUI::GetLayerID()const
{
    if(targetApp==INVALID_AMPID)
        return INVALID_ID;
    else
        return LAYERID(GET_SEAT(targetApp), GetLayer(GET_APPID(targetApp))+1);
}

std::vector<Surface> IMEUI::GetSynchronizableSurfaces()const
{
    return std::vector<Surface>();
}

AMPID IMEUI::GetTargetApp() const
{
    return targetApp;
}

void IMEUI::SetTargetApp(AMPID targetApp)
{
    IMEUI::targetApp = targetApp;
}

unsigned int IMEUI::GetIMEHeight()
{
    return IMEHeight;
}

void IMEUI::SetIMEHeight(unsigned int IMEHeight)
{
    IMEUI::IMEHeight = IMEHeight;
}

//void IMEUI::_ConfigSurface(Surface &surface, const Section &sec)
//{
//    logVerbose(HMIC, "IMEUI::_ConfigSurface(", ToHEX(surface.GetID()), ")--->IN");
//
//    // app size is up to IME app now.
////    // set surface size
////    Section sourceSection = sec;
////    Section destinationSection = sec;
////    sourceSection.height = IMEHeight;
////    destinationSection.height = IMEHeight;
////    destinationSection.top = static_cast<unsigned int>(labs(static_cast<long>(sec.height)-IMEHeight));
////    surface.SetSourceRectangle(sourceSection);
////    surface.SetDestinationRectangle(destinationSection);
////
////    // commit changes
////    ILMClient::Commit();
//
//    logVerbose(HMIC, "IMEUI::_ConfigSurface(", ToHEX(surface.GetID()), ")--->OUT");
//}

void IMEUI::AddSurfaces2Layer()
{
    DomainVerboseLog chatter(HMIC, "IMEUI::AddSurfaces2Layer()");

    try
    {
        for(auto& surface: surfaces)
        {
            LayerID layerID = GetLayerID();
            if(layerID==INVALID_ID)
                continue;

            Layer layer(layerID);
            layer.AddSurface(surface);
        }

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in IMEUI::AddSurfaces2Layer()!");
    }
}

void IMEUI::RemoveSurfacesFromLayer()
{
    DomainVerboseLog chatter(HMIC, "IMEUI::RemoveSurfacesFromLayer()");

    try
    {
        for(auto& surface: surfaces)
        {
            LayerID layerID = GetLayerID();
            if(layerID==INVALID_ID)
                continue;

            Layer layer(layerID);
            layer.RemoveSurface(surface);
        }

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in IMEUI::RemoveSurfacesFromLayer()!");
    }
}
