/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file RSEScreen.cpp
/// @brief contains the implementation of class RSEScreen
///
/// Created by zs on 2016/11/29.
/// this file contains the implementation of class RSEScreen
///

#include <algorithm>

#include "ILM/RSEScreen.h"
#include "ILM/VirtualScreenManager.h"

RSEScreen::RSEScreen()
: Screen()
, virtualScreenID(0)
, section()
{
    ;
}

RSEScreen::RSEScreen(const ScreenID ID)
: virtualScreenID(ID)
{
    SetID(ID);
}

RSEScreen::~RSEScreen()
{
    ;
}

Resolution RSEScreen::GetResolution() const
{
    Resolution resolution{section.width, section.height};
    return resolution;
}

shared_ptr<ScreenProperties> RSEScreen::GetProperties() const
{
    shared_ptr<ScreenProperties> properties = Screen::GetProperties();
    properties->screenWidth = section.width;
    properties->screenHeight = section.height;
    properties->layerCount = GetLayerCount();
    properties->layerIds = nullptr; // TODO: to be implemented
    return properties;
}

shared_ptr<ScreenExtendedProperties> RSEScreen::GetExtendedProperties() const
{
    auto extProperties = Screen::GetExtendedProperties();
    extProperties->x = section.left;
    extProperties->y = section.top;
    return extProperties;
}

unsigned int RSEScreen::GetNumberOfHardwareLayers() const
{
    return Screen::GetNumberOfHardwareLayers();
}

unsigned int RSEScreen::GetLayerCount() const
{
    return VirtualScreenManager::GetInstance()->GetRenderOrder(virtualScreenID).size();
}

std::vector<Layer> RSEScreen::GetAllLayers()const
{
    return VirtualScreenManager::GetInstance()->GetRenderOrder(virtualScreenID);
}

void RSEScreen::TakeScreenShot(const std::string &path) const
{
    Screen::TakeScreenShot(path);
}

Size RSEScreen::GetSize() const
{
    Size size{section.width, section.height};
    return size;
}

Section RSEScreen::GetSection() const
{
    return section;
}

Section RSEScreen::GetRectangle() const
{
    Size size = GetSize();

    Section rseSection = Screen::GetSection();
    Section rectangle;
    rectangle.left = section.left - rseSection.left;
    rectangle.top = section.top - rseSection.top;
    rectangle.width = size.width;
    rectangle.height = size.height;

    return rectangle;
}

void RSEScreen::SetRenderOrder(const std::vector<Layer>& layers)
{
    // get existing layers
    auto existedlayers = Screen::GetAllLayers();

    // delete old layers
    auto oldLayers = VirtualScreenManager::GetInstance()->GetRenderOrder(virtualScreenID);
    for(auto& oldLayer: oldLayers)
        existedlayers.erase(std::find(existedlayers.begin(), existedlayers.end(), oldLayer));

    // add new layers to the top of existing render order
    VirtualScreenManager::GetInstance()->SetRenderOrder(virtualScreenID, layers);
    for(auto& newLayer: layers)
        existedlayers.push_back(newLayer);

    // set new render
    Screen::SetRenderOrder(existedlayers);
}

ScreenID RSEScreen::GetID() const
{
    return virtualScreenID;
}

void RSEScreen::SetID(const ScreenID ID)
{
    virtualScreenID = ID;
    Screen::SetID(VirtualScreenManager::GetInstance()->GetReal(virtualScreenID));
    section = VirtualScreenManager::GetInstance()->GetSection(virtualScreenID);
}
