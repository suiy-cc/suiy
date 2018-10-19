/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file RSEScreen.h
/// @brief contains the definition of class RSEScreen
///
/// Created by zs on 2016/11/29.
/// this file contains the definition of class RSEScreen
////

#ifndef RSESCREEN_H
#define RSESCREEN_H

#include <ILM/Screen.h>

class RSEScreen
: public Screen
{
public:
    RSEScreen();
    RSEScreen(const ScreenID ID);
    virtual ~RSEScreen();

    virtual Resolution GetResolution() const override;
    virtual shared_ptr<ScreenProperties> GetProperties() const override;
    virtual shared_ptr<ScreenExtendedProperties> GetExtendedProperties() const override;
    virtual unsigned int GetNumberOfHardwareLayers() const override;
    virtual unsigned int GetLayerCount() const override;
    virtual std::vector<Layer> GetAllLayers() const override;
    virtual void TakeScreenShot(const std::string &path) const override;
    virtual Size GetSize() const override;
    virtual Section GetSection() const override;
    virtual Section GetRectangle() const override;
    virtual void SetRenderOrder(const std::vector<Layer>& layers) override;
    virtual ScreenID GetID() const override;
    virtual void SetID(const ScreenID ID) override;

private:
    ScreenID virtualScreenID;
    Section section;
};

#endif // RSESCREEN_H
