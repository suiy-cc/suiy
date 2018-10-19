/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file IMEUI.h
/// @brief contains class IMEUI
///
/// Created by zs on 2016/10/10.
/// this file contains the definition of class IMEUI
///

#ifndef IMEUI_H
#define IMEUI_H

#include "AppUIHMI.h"

class IMEUI
: public AppUIHMI
{
public:
    IMEUI();
    explicit IMEUI(const Surface &surface);
    virtual ~IMEUI();

    //virtual void SetVisible(const bool isVisible = true)override;
    virtual LayerID GetLayerID()const override;
    virtual std::vector<Surface> GetSynchronizableSurfaces()const;
    virtual void AddSurfaces2Layer() override;
    virtual void RemoveSurfacesFromLayer() override;

    /// @name getters & setters
    /// @{
    AMPID GetTargetApp() const;
    void SetTargetApp(AMPID targetApp);
    void SetIMEHeight(unsigned int IMEHeight);
    unsigned int GetIMEHeight();
    /// @}

// delete this so that rectangle of IME is set by HMIC instead of IME
//protected:
//    virtual void _ConfigSurface(Surface &surface, const Section &section) override;

private:
    AMPID targetApp;
    unsigned int IMEHeight;
};


#endif // IMEUI_H
