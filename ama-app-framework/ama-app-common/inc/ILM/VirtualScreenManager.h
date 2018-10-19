/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file VirtualScreenManager.h
/// @brief contains the definition of class VirtualScreenManager
///
/// Created by zs on 2016/11/30.
/// this file contains the definition of class VirtualScreenManager
////

#ifndef VIRTUALSCREENMANAGER_H
#define VIRTUALSCREENMANAGER_H

#include <vector>
#include <memory>
#include <map>

#include <ILM/Layer.h>
#include <ILM/HMIType.h>

class Screen;

class VirtualScreenManager
{
public:
    virtual ~VirtualScreenManager(){};

    static VirtualScreenManager* GetInstance();

    std::vector<std::shared_ptr<Screen>> GetVirtualScreens();
    ScreenID GetReal(ScreenID screen)const;
    Section GetSection(ScreenID screen)const;
    void SetRenderOrder(ScreenID screen, const std::vector<Layer>& renderOrder);
    std::vector<Layer> GetRenderOrder(ScreenID screen)const;

private:
    VirtualScreenManager(); // for singleton pattern
    void Initialize();

    bool IsVirtualizable(const Screen& screen)const;
    std::vector<std::shared_ptr<Screen>> Virtualize(const Screen& screen)const;

    std::map<ScreenID, ScreenID> virtual2real;
    std::map<ScreenID, Section> sections;
    std::map<ScreenID, std::vector<Layer>> renderOrder;
};


#endif // VIRTUALSCREENMANAGER_H
