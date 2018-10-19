/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUI.h
/// @brief contains class AppUI
///
/// Created by zs on 2016/6/20
/// this file contains the definition of class AppUI
///

#ifndef APPUI_H
#define APPUI_H

#include "SurfaceID.h"

/// @class AppUI
/// @brief abstracts the UI of an app.
class AppUI
{
protected:
    std::vector<Surface> surfaces;
public:
    virtual ~AppUI();

public:
    AppUI();// you will not create any object of this class
    virtual void AddSurface(const Surface& surface);
    virtual void RemoveSurface(const Surface& surface);
    virtual bool IsInThisApp(const Surface& surface)const;
    virtual std::vector<Surface> GetSurfaces()const;
    virtual E_SEAT GetSeat()const;
    virtual AMPID GetAMPID()const;
    virtual LayerID GetLayerID()const;
    virtual bool IsTheSame(const shared_ptr<const AppUI> UI) const;
};

#endif // APPUI_H
