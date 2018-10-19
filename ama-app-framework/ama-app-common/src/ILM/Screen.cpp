/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Screen.cpp
/// @brief contains the implementation of class Screen
///
/// Created by zs on 2016/6/15.
/// this file contains the implementation of class Screen
///

#include <ilm_control.h>
#include <cassert>

#include "ILM/Screen.h"
#include "ILM/ILMException.h"
#include "log.h"

//Be sure to put it at the end of statement
#include "ama_mem.h"

LOG_DECLARE_CONTEXT(SCRN);

Screen::Screen()
: screenID(0)
{
    ;
}

Screen::Screen(const ScreenID ID)
: screenID(ID)
{
    ;
}

Screen::~Screen()
{
    ;
}

bool Screen::operator==(const Screen &rhs) const
{
    return screenID == rhs.screenID;
}

bool Screen::operator!=(const Screen &rhs) const
{
    return !(rhs == *this);
}

Resolution Screen::GetResolution()const
{
    Resolution resolution;
    ilmErrorTypes errorTypes = ilm_getScreenResolution(screenID, &resolution.width, &resolution.height);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Screen::GetResolution()");
        throw e;
    }

    return resolution;
}

void Screen::_GetProperties(ScreenProperties *screenProperties)const
{
    ilmErrorTypes errorTypes = ilm_getPropertiesOfScreen(screenID, screenProperties);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Screen::_GetProperties()");
        throw e;
    }
}

shared_ptr<ScreenProperties> Screen::GetProperties()const
{
    std::shared_ptr<ScreenProperties> screenProperties(new ScreenProperties, [](ScreenProperties *properties){
        if(properties->layerIds)
            ama_free(properties->layerIds);
        delete properties;
    });
    _GetProperties(screenProperties.get());

    return screenProperties;
}

void Screen::_GetExtendedProperties(ScreenExtendedProperties* screenExtendedProperties)const
{
    DomainVerboseLog chatter(SCRN, formatText("_GetExtendedProperties(%d)", screenID));
    ilmErrorTypes errorTypes = ilm_getExtendedPropertiesOfScreen(screenID, screenExtendedProperties);
    logInfoF(SCRN, "Screen::_GetExtendedProperties(): x=%d y=%d w=%d h=%d",
        screenExtendedProperties->x,
        screenExtendedProperties->y,
        screenExtendedProperties->physical_width,
        screenExtendedProperties->physical_height);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Screen::_GetExtendedProperties()");
        throw e;
    }
}

shared_ptr<ScreenExtendedProperties> Screen::GetExtendedProperties()const
{
    auto screenExtProperties = std::make_shared<ScreenExtendedProperties>();
    _GetExtendedProperties(screenExtProperties.get());

    return screenExtProperties;
}

unsigned int Screen::GetNumberOfHardwareLayers()const
{

    ScreenProperties screenProperties;
    _GetProperties(&screenProperties);
    ama_free(screenProperties.layerIds);

    return screenProperties.harwareLayerCount;
}

unsigned int Screen::GetLayerCount()const
{
    ScreenProperties screenProperties;
    _GetProperties(&screenProperties);
    ama_free(screenProperties.layerIds);

    return screenProperties.layerCount;
}

std::vector<Layer> Screen::GetAllLayers()const
{
    int count = 0;
    t_ilm_layer* array = nullptr;
    ilmErrorTypes errorTypes = ilm_getLayerIDsOnScreen(screenID, &count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Screen::GetAllLayers()");
        throw e;
    }

    std::vector<Layer> layers;
    layers.reserve(count);
    for(int i=0; i<count; ++i)
        layers.push_back( Layer(array[i]) );

    ama_free(array);
    return std::move(layers);
}

void Screen::TakeScreenShot(const std::string& path)const
{
    ilmErrorTypes errorTypes = ilm_takeScreenshot(screenID, path.c_str());
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Screen::TakeScreenShot()");
        throw e;
    }
}
void Screen::TakeScreenShotEx(unsigned int screen,char*defaultname,
                              unsigned int x, unsigned int y , unsigned int w, unsigned int h,
                              const char * format)
{
    ilmErrorTypes errorTypes =  ilm_takeScreenshotEx(screen,defaultname,  x,y,w,h,format);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "Screen::TakeScreenShotEx()");
        throw e;
    }
}
Size Screen::GetSize()const
{
    ScreenProperties screenProperties;
    _GetProperties(&screenProperties);
    ama_free(screenProperties.layerIds);

    Size size;
    size.width = screenProperties.screenWidth;
    size.height = screenProperties.screenHeight;

    return size;
}

Section Screen::GetSection()const
{
    // get ext property
    ScreenExtendedProperties screenExtendedProperties;
    _GetExtendedProperties(&screenExtendedProperties);

    // get size
    Size size = GetSize();

    // set section
    Section section;
    section.left = screenExtendedProperties.x;
    section.top = screenExtendedProperties.y;
    section.width = size.width;
    section.height = size.height;

    return section;
}

Section Screen::GetRectangle()const
{
    Size size = GetSize();

    Section rectangle;
    rectangle.left = 0;
    rectangle.top = 0;
    rectangle.width = size.width;
    rectangle.height = size.height;

    return rectangle;
}

void Screen::SetRenderOrder(const std::vector<Layer>& layers)
{
    assert(layers.size());
    if(layers.empty())
    {
        return;
    }

    t_ilm_uint count = static_cast<t_ilm_uint>(layers.size());
    t_ilm_layer* array = new t_ilm_layer[count];

    for(int i=0; i<count; ++i)
        array[i] = layers[i].GetID();

    ilmErrorTypes errorTypes = ilm_displaySetRenderOrder(screenID, array, count);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "SetRenderOrder()");
        throw e;
    }

    delete []array;
}

ScreenID Screen::GetID()const
{
    return screenID;
}

void Screen::SetID(const ScreenID ID)
{
    screenID = ID;
}

unsigned int GetScreenCount()
{
    unsigned int count = 0;
    t_ilm_uint *array = nullptr;
    ilmErrorTypes errorTypes = ilm_getScreenIDs(&count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "GetScreenCount()");
        throw e;
    }

    ama_free(array);
    return count;
}

std::vector<Screen> GetAllScreens()
{
    unsigned int count = 0;
    t_ilm_uint *array = nullptr;
    ilmErrorTypes errorTypes = ilm_getScreenIDs(&count, &array);
    if(errorTypes!=ILM_SUCCESS)
    {
        ILMException e(errorTypes, "GetAllScreens()");
        throw e;
    }

    std::vector<Screen> screens;
    screens.reserve(count);
    for(int i=0; i<count; ++i)
        screens.push_back( Screen(array[i]) );

    ama_free(array);
    return std::move(screens);
}
