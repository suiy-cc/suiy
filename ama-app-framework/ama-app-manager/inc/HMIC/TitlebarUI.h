/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TitlebarUI.h
/// @brief contains class TitlebarUI
///
/// Created by zs on 2016/8/29.
/// this file contains the definition of class TitlebarUI
///

#ifndef TITLEBARUI_H
#define TITLEBARUI_H

#include "AppUIHMI.h"

class TitlebarUI
: public AppUIHMI
{
public:
    TitlebarUI();
    TitlebarUI(const Surface &surface);
    virtual ~TitlebarUI();

public:
    virtual void OnExpand();
    virtual void OnShrink();

    virtual std::vector<Surface> GetSynchronizableSurfaces()const;

protected:
    virtual void _ConfigSurface(Surface &surface, const Section &section, const unsigned int height);
    virtual void _ConfigSurface(Surface &surface, const Section &section);

protected:
    static const unsigned int titlebarHeight;
};


#endif // TITLEBARUI_H
