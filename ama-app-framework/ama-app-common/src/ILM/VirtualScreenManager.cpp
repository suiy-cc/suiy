/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file VirtualScreenManager.cpp
/// @brief contains the implementation of class VirtualScreenManager
///
/// Created by zs on 2016/11/30.
/// this file contains the implementation of class VirtualScreenManager
///

#include <algorithm>

#include "ILM/VirtualScreenManager.h"
#include "ILM/RSEScreen.h"
#include "ILM/ScreenSetting.h"

#include "log.h"

LOG_DECLARE_CONTEXT(VTSM);

VirtualScreenManager *VirtualScreenManager::GetInstance()
{
    static VirtualScreenManager instance;

    return &instance;
}

std::vector<std::shared_ptr<Screen>> VirtualScreenManager::GetVirtualScreens()
{
    std::vector<std::shared_ptr<Screen>> virtualScreens;

    auto realScreens = GetAllScreens();
    for(auto& realScreen: realScreens)
    {
        // virtualize real screen
        if(IsVirtualizable(realScreen))
        {
            auto tmp = Virtualize(realScreen);
            for(auto& virtualScreen: tmp)
                virtualScreens.push_back(virtualScreen);
        }
        else // unvirtualizable screen can be count as a virtual screen
            virtualScreens.push_back(std::make_shared<Screen>(realScreen));
    }

    return std::move(virtualScreens);
}

ScreenID VirtualScreenManager::GetReal(ScreenID screen)const
{
    if(virtual2real.find(screen)==virtual2real.end())
        return 0;
    else
        return virtual2real.at(screen);
}

Section VirtualScreenManager::GetSection(ScreenID screen)const
{
    if(sections.find(screen)==sections.end())
        return Section();
    else
        return sections.at(screen);
}
void VirtualScreenManager::SetRenderOrder(ScreenID screen, const std::vector<Layer>& renderOrder)
{
    this->renderOrder[screen] = renderOrder;
}

std::vector<Layer> VirtualScreenManager::GetRenderOrder(ScreenID screen)const
{
    if(renderOrder.find(screen)==renderOrder.end())
        return std::vector<Layer>();
    else
        return renderOrder.at(screen);
}

VirtualScreenManager::VirtualScreenManager()
{
    Initialize();
}

void VirtualScreenManager::Initialize()
{
    DomainVerboseLog chatter(VTSM, "VirtualScreenManager::Initialize()");

    // initialize virtual screens
    auto realScreens = GetAllScreens();
    for(auto& screen: realScreens)
        if(screen.GetSection()==ScreenSettingRSE)
        {
            logDebug(VTSM, "VirtualScreenManager::Initialize(): screen RSE is found!");
            // get screen 2
            Section section = screen.GetSection();
            logInfoF(VTSM, "VirtualScreenManager::Initialize(): screenRSE's section: %d %d %d %d", section.left, section.top, section.width, section.height);

            // cut virtual screen 2 off
            Section section1 = ScreenSetting[E_SEAT_RSE1];
            virtual2real.insert(std::make_pair(2, screen.GetID()));
            sections.insert(std::make_pair(2, section1));
            logInfoF(VTSM, "VirtualScreenManager::Initialize(): virtual screen2's section: %d %d %d %d", section1.left, section1.top, section1.width, section1.height);

            // cut virtual screen 3 off
            Section section2 = ScreenSetting[E_SEAT_RSE2];
            virtual2real.insert(std::make_pair(3, screen.GetID()));
            sections.insert(std::make_pair(3, section2));
            logInfoF(VTSM, "VirtualScreenManager::Initialize(): virtual screen3's section: %d %d %d %d", section2.left, section2.top, section2.width, section2.height);
        }

    // initialize layers
    auto layers = ::GetAllLayers();
    for(auto& layer: layers)
    {
        ScreenID virtualScreenID = static_cast<ScreenID>(GET_SEAT(layer.GetID()));
        if(virtual2real.find(virtualScreenID)!=virtual2real.end())
        {
            renderOrder[virtualScreenID].push_back(layer);
        }
    }
}

bool VirtualScreenManager::IsVirtualizable(const Screen& screen)const
{
    auto iter = std::find_if(virtual2real.begin(), virtual2real.end(),
        [&](const std::pair<ScreenID, ScreenID >& pair)->bool{
            return pair.second==screen.GetID();
        });

    return iter!=virtual2real.end();
}

std::vector<std::shared_ptr<Screen>> VirtualScreenManager::Virtualize(const Screen& screen)const
{
    std::vector<std::shared_ptr<Screen>> virtualScreens;

    for(auto virtualScreenID: virtual2real)
        if(virtualScreenID.second==screen.GetID())
            virtualScreens.push_back(std::make_shared<RSEScreen>(virtualScreenID.first));

    return std::move(virtualScreens);
}
