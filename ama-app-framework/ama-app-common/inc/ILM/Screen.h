/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Screen.h
/// @brief contains the definition of class Screen
///
/// Created by zs on 2016/6/15.
/// this file contains the definition of class Screen
///

#ifndef SCREEN_H
#define SCREEN_H

#include <memory>
#include <vector>
#include <string>

#include <ILM/Layer.h>


using std::shared_ptr;

/// @class Screen
/// @brief this class is an abstraction of screen in ILM
class Screen
{
public:
    /// @name constructors & destructors
    /// @{
    Screen();
    explicit Screen(const ScreenID ID);
    virtual ~Screen();
    /// @}

    virtual /// @name relational operations
    /// @{
    bool operator==(const Screen &rhs) const;
    virtual bool operator!=(const Screen &rhs) const;
    /// @}

    /// @name operations
    /// @{
    virtual Resolution GetResolution()const ;
    virtual shared_ptr<ScreenProperties> GetProperties()const;
    virtual shared_ptr<ScreenExtendedProperties> GetExtendedProperties()const;
    virtual unsigned int GetNumberOfHardwareLayers()const;
    virtual unsigned int GetLayerCount()const;
    virtual std::vector<Layer> GetAllLayers()const;
    virtual void TakeScreenShot(const std::string& path)const;
    virtual void TakeScreenShotEx(unsigned int screen,char*defaultname, unsigned int x, unsigned int y , unsigned int w, unsigned int h,const char * format);
    virtual Size GetSize()const;
    virtual Section GetSection()const;
    virtual Section GetRectangle()const;
    virtual void SetRenderOrder(const std::vector<Layer>& layers);
    /// @}

    /// @name getters & setters
    /// @{
    virtual ScreenID GetID()const;
    virtual void SetID(const ScreenID ID);
    /// @}
private:
    void _GetProperties(ScreenProperties *screenProperties)const;
    void _GetExtendedProperties(ScreenExtendedProperties* screenExtendedProperties)const;
private:
    ScreenID screenID;
};

unsigned int GetScreenCount();
std::vector<Screen> GetAllScreens();

#endif // SCREEN_H
