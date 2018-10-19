/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file VideoUI.cpp
/// @brief contains the implementation of class VideoUI
///
/// Created by zs on 2016/8/23.
/// this file contains the implementation of class VideoUI
///

#include "ILM/ILMClient.h"
#include "ILM/ILMException.h"
#include "log.h"
#include "AMGRLogContext.h"
#include "ProcessInfo.h"
#include "VideoLikeUI.h"
#include "ReleaseLayer.h"

#ifdef ENABLE_DEBUG_MODE
#include "AppProfileManager.h"
#endif // ENABLE_DEBUG_MODE

VideoLikeUI::VideoLikeUI()
: isCenterUnderLayerSurface(true)
{
    ;
}

VideoLikeUI::VideoLikeUI(const Surface &surface)
    : isCenterUnderLayerSurface(true)
    , AppUIHMI(surface)
{
    ;
}

VideoLikeUI::~VideoLikeUI()
{

}

void VideoLikeUI::OnSurfaceCreate(Surface &surface)
{
    logVerbose(HMIC, "VideoLikeUI::OnSurfaceCreate(", ToHEX(surface.GetID()), ", ", ")--->IN");

    AppUIHMI::OnSurfaceCreate(surface);

    try
    {
        AMPID proc = ::GetAMPID(surface);
        auto procInfo = ProcessInfo::GetInstance();
        procInfo->Lock();
        auto procObj = procInfo->FindUIProc(proc);
        procInfo->Unlock();
        if(procObj && procObj->IsOperating() && procObj->GetState() && procObj->GetState()->IsVisibleState() && IsOnLayer())
        {
            AddSurfaces2Layer();
            SetVisible(procObj->GetState()->IsVisibleState());
        }
        else
        {
            if(playWindow!=INVALID_SURFACE)
            {
                ReleaseLayer layer(LAYERID(GetSeat(), E_LAYER_RELEASE_PLAY));

#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property in");
#endif // ENABLE_DEBUG_MODE
                LayerID containorLayer = playWindow.GetContainerLayer();
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property out");
#endif // ENABLE_DEBUG_MODE
                logDebug(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), "'s layer is ", ToHEX(containorLayer));
                if(containorLayer==INVALID_ID)
                {
                    logDebug(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), " is going to be added to layer ", ToHEX(layer.GetID()), ".");
#ifdef ENABLE_DEBUG_MODE
                    AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer in");
#endif // ENABLE_DEBUG_MODE
                    layer.AddSurface(playWindow);
                    ILMClient::Commit();
#ifdef ENABLE_DEBUG_MODE
                    AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer out");
#endif // ENABLE_DEBUG_MODE
                    logDebug(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), " is added to layer ", ToHEX(layer.GetID()), ".");
                }
            }
        }
    }
    catch (ILMException& e)
    {
        logError(HMIC, e.what(), " caught in VideoLikeUI::OnSurfaceCreate().");
    }
    catch (...)
    {
        logError(HMIC, "unexpected exception is caught in VideoLikeUI::OnSurfaceCreate().");
    }

    logVerbose(HMIC, "VideoLikeUI::OnSurfaceCreate(", ToHEX(surface.GetID()), ")--->OUT");
}

void VideoLikeUI::SetVisible(const bool isVisible)
{
    logVerbose(HMIC, "VideoLikeUI::SetVisible(", isVisible, ")--->IN");

    try
    {
        if(playWindow!=INVALID_SURFACE)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_surface_visible in");
#endif // ENABLE_DEBUG_MODE
            playWindow.SetVisible(isVisible);
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_surface_visible out");
#endif // ENABLE_DEBUG_MODE
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in VideoLikeUI::SetVisible()!");
    }

    try
    {
        if(backgroundWindow!=INVALID_SURFACE)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_surface_visible in");
#endif // ENABLE_DEBUG_MODE
            backgroundWindow.SetVisible(isVisible);
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_surface_visible out");
#endif // ENABLE_DEBUG_MODE
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in VideoLikeUI::SetVisible()!");
    }

    AppUIHMI::SetVisible(isVisible);

    logVerbose(HMIC, "VideoLikeUI::SetVisible(", isVisible, ")--->OUT");
}

bool VideoLikeUI::IsExpectedSurface(const Surface &surface) const
{
    if(AppUIHMI::IsExpectedSurface(surface))
        return true;

    E_LAYER layer = GetLayer(surface);

    if(layer==E_LAYER_VIDEOPLAY)
        return true;

    if(layer==E_LAYER_BACKGROUND)
        return true;

    return false;
}

void VideoLikeUI::RegisterSurface(Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText("VideoLikeUI::RegisterSurface(%#x)", surface.GetID()));

    AppUIHMI::RegisterSurface(surface);

    E_LAYER layer = GetLayer(surface);
    if(layer==E_LAYER_VIDEOPLAY)
    {
        playWindow = surface;
        logDebug(HMIC, "VideoLikeUI::RegisterSurface(): play surface is registered.");
    }
    if(layer==E_LAYER_BACKGROUND)
    {
        backgroundWindow = surface;
        logDebug(HMIC, "VideoLikeUI::RegisterSurface(): background surface is registered.");
    }
}

void VideoLikeUI::UnregisterSurface(Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText("VideoLikeUI::UnregisterSurface(%#x)", surface.GetID()));

    AppUIHMI::UnregisterSurface(surface);

    if(playWindow==surface)
    {
        playWindow = INVALID_SURFACE;
        logDebug(HMIC, "VideoLikeUI::UnregisterSurface(): play surface is unregistered.");
    }
    if(backgroundWindow==surface)
    {
        backgroundWindow = INVALID_SURFACE;
        logDebug(HMIC, "VideoLikeUI::UnregisterSurface(): background surface is unregistered.");
    }
}

void VideoLikeUI::AddSurfaces2Layer()
{
    DomainVerboseLog chatter(HMIC, "VideoLikeUI::AddSurfaces2Layer()");

    bool isCommit = false;


    if(playWindow!=INVALID_SURFACE)
    {
        // add under-surface to under-layer
        try
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property in");
#endif // ENABLE_DEBUG_MODE
            auto containerLayerID = playWindow.GetContainerLayer();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property out");
#endif // ENABLE_DEBUG_MODE
            // add play window to play layer
            if(containerLayerID==INVALID_ID)
            {
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer in");
#endif // ENABLE_DEBUG_MODE
                Layer layer(playWindow.GetLayerID());
                layer.AddSurface(playWindow);
                isCommit = true;
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer out");
#endif // ENABLE_DEBUG_MODE

                logInfo(HMIC, LOG_HEAD, "play surface is added to play layer.");
            }
        }
        catch(ILMException& e)
        {
            logWarn(HMIC, e.what(), " caught in VideoLikeUI::AddSurfaces2Layer()!");
        }
    }

    // add backgroud surface to BKG layer
    try
    {
        if(backgroundWindow!=INVALID_SURFACE)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property in");
#endif // ENABLE_DEBUG_MODE
            auto containerLayerID = backgroundWindow.GetContainerLayer();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property out");
#endif // ENABLE_DEBUG_MODE
            if(containerLayerID==INVALID_ID)
            {
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer in");
#endif // ENABLE_DEBUG_MODE
                Layer layer(backgroundWindow.GetLayerID());
                layer.AddSurface(backgroundWindow);
                isCommit = true;
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer out");
#endif // ENABLE_DEBUG_MODE

                logDebug(HMIC, LOG_HEAD, "BKG surface is added to BKG layer.");
            }
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in VideoLikeUI::AddSurfaces2Layer()!");
    }

    // commit
    try
    {
        if(isCommit)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE commit in");
#endif // ENABLE_DEBUG_MODE
            ILMClient::Commit();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE commit out");
#endif // ENABLE_DEBUG_MODE
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in VideoLikeUI::AddSurfaces2Layer()!");
    }

    AppUIHMI::AddSurfaces2Layer();
}

void VideoLikeUI::RemoveSurfacesFromLayer()
{
    logVerbose(HMIC, "VideoLikeUI::RemoveSurfacesFromLayer()--->IN");

    try
    {
        bool isCommit = false;
        if(playWindow!=INVALID_SURFACE)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE remove_surface_from_layer in");
#endif // ENABLE_DEBUG_MODE
            Layer layer(playWindow.GetLayerID());
            layer.RemoveSurface(playWindow);
            isCommit = true;
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE remove_surface_from_layer out");
#endif // ENABLE_DEBUG_MODE
        }

        if(backgroundWindow!=INVALID_SURFACE)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE remove_surface_from_layer in");
#endif // ENABLE_DEBUG_MODE
            Layer layer(backgroundWindow.GetLayerID());
            layer.RemoveSurface(backgroundWindow);
            isCommit = true;
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE remove_surface_from_layer out");
#endif // ENABLE_DEBUG_MODE
        }

        if(isCommit)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE commit in");
#endif // ENABLE_DEBUG_MODE
            ILMClient::Commit();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE commit out");
#endif // ENABLE_DEBUG_MODE
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in VideoLikeUI::RemoveSurfacesFromLayer()!");
    }

    AppUIHMI::RemoveSurfacesFromLayer();

    logVerbose(HMIC, "VideoLikeUI::RemoveSurfacesFromLayer()--->OUT");
}

void VideoLikeUI::OnSurfaceDestroy(Surface &surface)
{
    logVerbose(HMIC, "VideoLikeUI::OnSurfaceDestroy(", ToHEX(surface.GetID()), ")--->IN");
    try
    {
        if (surface == playWindow)
        {
            playWindow = INVALID_SURFACE;
            ReleaseLayer::DeleteRectCache(surface);

            logDebug(HMIC, "video's play surface is destroyed!");
        }
        else if (surface == backgroundWindow)
        {
            backgroundWindow = INVALID_SURFACE;

            logDebug(HMIC, "video's background surface is destroyed!");
        }
        else
            AppUIHMI::OnSurfaceDestroy(surface);
    }
    catch (ILMException& e)
    {
        logError(HMIC, "exception is caught in VideoLikeUI::OnSurfaceDestroy(", surface.GetID(), "): ", e.what());
    }
    logVerbose(HMIC, "VideoLikeUI::OnSurfaceDestroy(", ToHEX(surface.GetID()), ")--->OUT");
}

std::vector<Surface> VideoLikeUI::GetSynchronizableSurfaces()const
{
    std::vector<Surface> surfaces;

    if(backgroundWindow!=INVALID_SURFACE)
        surfaces.push_back(backgroundWindow);
    if(playWindow!=INVALID_SURFACE)
        surfaces.push_back(playWindow);

    auto uppperSurfaces = AppUIHMI::GetSynchronizableSurfaces();
    for(auto& surface: uppperSurfaces)
        surfaces.push_back(surface);

    return std::move(surfaces);
}

void VideoLikeUI::SetUnderLayerCenter(bool isCenter)
{
    isCenterUnderLayerSurface = isCenter;
}

bool VideoLikeUI::IsUnderLayerCenter()const
{
    return isCenterUnderLayerSurface;
}

void VideoLikeUI::_ConfigSurface(Surface &surface, const Section &section)
{
    DomainVerboseLog chatter(HMIC, formatText("VideoLikeUI::_ConfigSurface(%#x)", surface.GetID()));

    if(isCenterUnderLayerSurface)
    {
        logDebug(HMIC, "VideoLikeUI::_ConfigSurface(): configure surface to center.");
        try
        {
            // get (left,top)
            unsigned int x = 0;
            unsigned int y = 0;
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_properties_of_surface in");
#endif // ENABLE_DEBUG_MODE
            const auto dimension = surface.GetDimension();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_properties_of_surface out");
#endif // ENABLE_DEBUG_MODE
            logDebug(HMIC, "VideoLikeUI::_ConfigSurface(): surface size is ", dimension.width, "*", dimension.height);
            if(dimension.width <= section.width && dimension.height <= section.height)
            {
                x = section.left + (section.width - dimension.width)/2;
                y = section.top + (section.height - dimension.height)/2;
            }
            else
                logWarn(HMIC, "VideoLikeUI::_ConfigSurface(): surface size is bigger than screen!");

            // get soruce rectangle & destination rectangle
            Section sourceRect = {0,0,dimension.width,dimension.height};
            Section destinationRect = {x,y,dimension.width,dimension.height};
            logDebug(HMIC, "VideoLikeUI::_ConfigSurface(): surface position: (", x, ",", y, ")");

#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_source&destination_rectangle in");
#endif // ENABLE_DEBUG_MODE
            // set soruce rectangle & destination rectangle
            surface.SetSourceRectangle(sourceRect);
            surface.SetDestinationRectangle(destinationRect);
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_source&destination_rectangle out");
#endif // ENABLE_DEBUG_MODE

            logDebug(HMIC, "VideoLikeUI::_ConfigSurface(): surface's position is set.");
        }
        catch(ILMException& e)
        {
            logWarn(HMIC, "VideoLikeUI::_ConfigSurface(): exception is caught: ", (&e)->what());
        }
    }
    else
    {
        logDebug(HMIC, "VideoLikeUI::_ConfigSurface(): configure surface to top-left.");
        AppUIHMI::_ConfigSurface(surface, section);
    }
}

void VideoLikeUI::_AddSurfaces2ReleaseLayer()
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        if(playWindow!=INVALID_SURFACE)
        {
            ReleaseLayer layer(LAYERID(GetSeat(), E_LAYER_RELEASE_PLAY));

            LayerID containorLayerID = playWindow.GetContainerLayer();
            logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), "'s layer is ", ToHEX(containorLayerID));

            if(layer.GetID()!=containorLayerID)
            {
                if(containorLayerID!=INVALID_ID)
                {
                    logWarn(HMIC, LOG_HEAD, "containor layer is not invalid! taking surface off...");
                    Layer containorLayer(layer.GetID());
                    containorLayer.RemoveSurface(playWindow);
                    logWarn(HMIC, LOG_HEAD, "surface is free now.");
                }

                if(containorLayerID==INVALID_ID)
                {
                    logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), " is going to be added to layer ", ToHEX(layer.GetID()), ".");
                    layer.AddSurface(playWindow);
                    logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), " is added to layer ", ToHEX(layer.GetID()), ".");
                }

                ILMClient::Commit();
            }
            else
                logInfo(HMIC, LOG_HEAD, "current layer is already on release layer.");
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in VideoLikeUI::_AddSurfaces2ReleaseLayer()!");
    }

    AppUIHMI::_AddSurfaces2ReleaseLayer();
}

void VideoLikeUI::_RemoveSurfacesFromReleaseLayer()
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        if(playWindow!=INVALID_SURFACE)
        {
            ReleaseLayer layer(playWindow.GetContainerLayer());

            if(layer!=Layer(LAYERID(GetSeat(), E_LAYER_RELEASE_PLAY)))
            {
                logWarn(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), "is on layer ", ToHEX(layer.GetID()), " instead of under-background layer!");
                Layer commonLayer(layer.GetID());
                commonLayer.RemoveSurface(playWindow);
            }
            else
            {
                layer.RemoveSurface(playWindow);
            }
            ILMClient::Commit();

            logDebug(HMIC, LOG_HEAD, "surface ", ToHEX(playWindow.GetID()), " is removed from layer ", ToHEX(layer.GetID()));
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, LOG_HEAD, "exception is caught: ", e.what());
    }

    AppUIHMI::_RemoveSurfacesFromReleaseLayer();
}
