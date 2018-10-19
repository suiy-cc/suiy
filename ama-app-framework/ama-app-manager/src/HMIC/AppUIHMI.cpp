/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppUIHMI.cpp
/// @brief contains the implementation of class AppUIHMI
///
/// Created by zs on 2016/6/20.
/// this file contains the implementation of class AppUIHMI
///

#include "HMICTask.h"
#include "ProcessInfo.h"
#include "AppUIHMI.h"
#include "Layer.h"
#include "ILMClient.h"
#include "ILMException.h"
#include "PLCTask.h"
#include "TaskDispatcher.h"
#include "AppState.h"
#include "ReleaseLayer.h"

#ifdef ENABLE_DEBUG_MODE
#include "AppProfileManager.h"
#endif // ENABLE_DEBUG_MODE

AppUIHMI::AppUIHMI()
: isMainSurfaceCreated(false)
, isResumeUIVisible(false)
, isStartUIVisible(false)
, resumeUI()
, startUI()
, isTitlebarVisible(true)
, titlebarVisibilitySwitchStyle(0)
, isTouchable(true)
{
    ;
}

AppUIHMI::~AppUIHMI()
{
    ;
}

AppUIHMI::AppUIHMI(const Surface& surface)
: isMainSurfaceCreated(false)
, isResumeUIVisible(false)
, isStartUIVisible(false)
, resumeUI()
, startUI()
, isTitlebarVisible(true)
, titlebarVisibilitySwitchStyle(0)
, isTouchable(true)
{
    surfaces.push_back(surface);
}

Surface AppUIHMI::GetResumeUI()const
{
    return resumeUI;
}

void AppUIHMI::SetResumeUI(const Surface &surface)
{
    resumeUI = surface;
}

Surface AppUIHMI::GetStartUI()const
{
    return startUI;
}

void AppUIHMI::SetStartUI(const Surface &surface)
{
    startUI = surface;
}

std::vector<Surface> AppUIHMI::GetSynchronizableSurfaces()const
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    std::vector<Surface> surfaces;

    surfaces.push_back(*this->surfaces.begin());

    return std::move(surfaces);
}

void AppUIHMI::SetVisible(const bool isVisible)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        for(auto &i: surfaces)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_surface_visible in");
#endif // ENABLE_DEBUG_MODE
            i.SetVisible(isVisible);
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_surface_visible out");
#endif // ENABLE_DEBUG_MODE
        }

        if(!isVisible)
        {
            if(startUI != INVALID_SURFACE)
                startUI.SetVisible(false);
            if(resumeUI != INVALID_SURFACE)
                resumeUI.SetVisible(false);
        }
        else
        {
            if(startUI != INVALID_SURFACE)
                startUI.SetVisible(IsStartUIVisible());
            if(resumeUI != INVALID_SURFACE)
                resumeUI.SetVisible(IsResumeUIVisible());
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::SetVisible()!");
    }
}

void AppUIHMI::SetOpacity(Alpha alpha)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        for(auto& i: surfaces)
            i.SetOpacity(alpha);

        if(startUI != INVALID_SURFACE)
            startUI.SetOpacity(alpha);
        if(resumeUI != INVALID_SURFACE)
            resumeUI.SetOpacity(alpha);

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::SetOpacity()!");
    }
}

bool AppUIHMI::IsOnLayer()const
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::IsOnLayer()");

    try
    {
        if(surfaces.empty())
            return false;
        else
        {
            if(surfaces.begin()->IsExist())
                return surfaces.begin()->GetContainerLayer()!=INVALID_ID;
            else
                return false;
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::IsOnLayer()!");
    }

    return false;
}

bool AppUIHMI::IsVisible()const
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        if(surfaces.empty())
            return false;
        else
            return surfaces.begin()->IsVisible();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::IsVisible()!");
    }

    return false;
}

void AppUIHMI::SetSourceRectangle(const Section& section)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        for(auto& i:surfaces)
            i.SetSourceRectangle(section);

        if(resumeUI != INVALID_SURFACE)
            resumeUI.SetSourceRectangle(section);
        if(startUI != INVALID_SURFACE)
            startUI.SetSourceRectangle(section);

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::SetSourceRectangle()!");
    }
}

void AppUIHMI::SetDestinationRectangle(const Section& section)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        for(auto& i:surfaces)
            i.SetDestinationRectangle(section);

        if(resumeUI != INVALID_SURFACE)
            resumeUI.SetDestinationRectangle(section);
        if(startUI != INVALID_SURFACE)
            startUI.SetDestinationRectangle(section);

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::SetDestinationRectangle()!");
    }
}

void AppUIHMI::SetOrientation(const Orientation orientation)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        for(auto& i:surfaces)
            i.SetOrientation(orientation);

        if(startUI != INVALID_SURFACE)
            startUI.SetOrientation(orientation);
        if(resumeUI != INVALID_SURFACE)
            resumeUI.SetOrientation(orientation);

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::SetOrientation()!");
    }
}

void AppUIHMI::TakeScreenShot(const std::string& path)
{
    FUNCTION_SCOPE_LOG_C(HMIC);

    try
    {
        if(!surfaces.empty())
            surfaces.begin()->TakeScreenShot(path);

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::TakeScreenShot()!");
    }
}

void AppUIHMI::SetResumeUIVisible(const bool isVisible)
{
    isResumeUIVisible = isVisible;
    if(IsVisible())
        resumeUI.SetVisible(isVisible);

    ILMClient::Commit();
}

bool AppUIHMI::IsResumeUIVisible()const
{
    return isResumeUIVisible;
}

void AppUIHMI::SetStartUIVisible(const bool isVisible)
{
    isStartUIVisible = isVisible;
    if(IsVisible())
        startUI.SetVisible(isVisible);

    ILMClient::Commit();
}

bool AppUIHMI::IsStartUIVisible()const
{
    return isStartUIVisible;
}

bool AppUIHMI::IsTitlebarVisible() const
{
    return isTitlebarVisible;
}

void AppUIHMI::SetTitlebarVisible(bool isVisible)
{
    isTitlebarVisible = isVisible;
}

int AppUIHMI::GetTitlebarVisibilitySwitchStyle()const
{
    return titlebarVisibilitySwitchStyle;
}

void AppUIHMI::SetTitlebarVisibilitySwitchStyle(int style)
{
    titlebarVisibilitySwitchStyle = style;
}

bool AppUIHMI::IsTouchable()const
{
    return isTouchable;
}

void AppUIHMI::SetTouchable(bool enable)
{
    isTouchable = enable;
}

bool AppUIHMI::IsMainSurface(const Surface &surface)const
{
    return surface==GetSurfaces().front();
}

bool AppUIHMI::IsReady2Start()const
{
    return isMainSurfaceCreated;
}

bool AppUIHMI::IsExpectedSurface(const Surface &surface) const
{
    if(IsMainSurface(surface))
        return true;

    return false;
}

void AppUIHMI::RegisterSurface(Surface &surface)
{
    if(surface==GetSurfaces().front())
    {
        isMainSurfaceCreated = true;
        logDebug(HMIC, "AppUIHMI::RegisterSurface(): main surface is registered.");
    }
}

void AppUIHMI::UnregisterSurface(Surface &surface)
{
    if(surface==GetSurfaces().front())
    {
        isMainSurfaceCreated = false;
        logDebug(HMIC, "AppUIHMI::UnregisterSurface(): main surface is unregistered.");
    }
}

void AppUIHMI::AddSurfaces2Layer()
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::AddSurfaces2Layer()");

    try
    {
        bool isCommit = false;
        for(auto& surface: surfaces)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property in");
#endif // ENABLE_DEBUG_MODE
            LayerID containorLayer = surface.GetContainerLayer();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property out");
#endif // ENABLE_DEBUG_MODE
            logDebug(HMIC, "AppUIHMI::AddSurfaces2Layer(): surface ", ToHEX(surface.GetID()), "'s layer is ", ToHEX(containorLayer));
            if(containorLayer==INVALID_ID)
            {
                logDebug(HMIC, "AppUIHMI::AddSurfaces2Layer(): surface ", ToHEX(surface.GetID()), " is going to be added to layer ", ToHEX(surface.GetLayerID()), ".");
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer in");
#endif // ENABLE_DEBUG_MODE
                Layer layer(surface.GetLayerID());
                layer.AddSurface(surface);
                isCommit = true;
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer out");
#endif // ENABLE_DEBUG_MODE
                logDebug(HMIC, "AppUIHMI::AddSurfaces2Layer(): surface ", ToHEX(surface.GetID()), " is added to layer ", ToHEX(surface.GetLayerID()), ".");
            }
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
        logWarn(HMIC, e.what(), " caught in AppUIHMI::AddSurfaces2Layer()!");
    }
}

void AppUIHMI::RemoveSurfacesFromLayer()
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::RemoveSurfacesFromLayer()");

    try
    {
        for(auto& surface: surfaces)
        {
            Layer layer(surface.GetContainerLayer());
            layer.RemoveSurface(surface);
        }

        if(startUI != INVALID_SURFACE)
        {
            Layer layer(startUI.GetLayerID());
            layer.RemoveSurface(startUI);
        }
        if(resumeUI != INVALID_SURFACE)
        {
            Layer layer(resumeUI.GetLayerID());
            layer.RemoveSurface(resumeUI);
        }

        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, e.what(), " caught in AppUIHMI::RemoveSurfacesFromLayer()!");
    }
}

void AppUIHMI::AddSurfaces2PreShowLayer()
{
    _AddSurfaces2ReleaseLayer();
}

void AppUIHMI::RemoveSurfacesFromPreShowLayer()
{
    _RemoveSurfacesFromReleaseLayer();
}

void AppUIHMI::AddSurfaces2PreHideLayer()
{
    _AddSurfaces2ReleaseLayer();
}

void AppUIHMI::RemoveSurfacesFromPreHideLayer()
{
    _RemoveSurfacesFromReleaseLayer();
}

void AppUIHMI::OnSurfaceAvailable(Surface &surface, shared_ptr<const Screen> screen)
{
    DomainVerboseLog chatter(HMIC, formatText("AppUIHMI::OnSurfaceAvailable(%#x, %d)", surface.GetID(), screen->GetID()));

    try
    {
        if(IsExpectedSurface(surface))
        {
            surface.SetConfiguration(screen->GetSize());
            ILMClient::Commit();
            logDebug(HMIC, "AppUIHMI::OnSurfaceAvailable(): surface ", ToHEX(surface.GetID()), " is available.");
        }
        else
        {
            logWarn(HMIC, "unexpected surface ", ToHEX(surface.GetID()), " is available!");
        }
    }
    catch (ILMException& e)
    {
        logError(HMIC, e.what(), "Caught in AppUIHMI::OnSurfaceAvailable(", ToHEX(surface.GetID()), ")!");
    }
    catch (...)
    {
        logError(HMIC, "Unexpected exception caught in AppUIHMI::OnSurfaceAvailable()!");
    }
}

void AppUIHMI::OnSurfaceCreate(Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText("AppUIHMI::OnSurfaceCreate(%#x)", surface.GetID()));

    try
    {
        if(IsExpectedSurface(surface))
        {
            RegisterSurface(surface);
            _ConfigSurface(surface, _GetRectangle());
            logDebug(HMIC, "AppUIHMI::OnSurfaceCreate(): surface ", ToHEX(surface.GetID()), " is configured.");

            // calculate timing.
            // NOTE: the codes below is for apps, not services with UI.
            // for that not every service is to be shown right after its
            // UI is ready to show.
            bool isRightTiming = true;
            auto procInfo = ProcessInfo::GetInstance();
            procInfo->Lock();
            auto appObj = procInfo->FindApp(GetAMPID());
            procInfo->Unlock();
            if(appObj)
            {
                isRightTiming = appObj->IsStarting();
            }

            // start specified app
            if(IsReady2Start() && isRightTiming)
            {
                auto hmicTask = make_shared<HMICTask>();
                hmicTask->SetAMPID(GET_AMPID(surface.GetID()));
                hmicTask->SetType(HMIC_NOTIFY_APP_START);
                hmicTask->SetPriority(GetAppID(surface)==E_APPID_HOME?E_TASK_PRIORITY_HIGH:E_TASK_PRIORITY_NORMAL);
                TaskDispatcher::GetInstance()->Dispatch(hmicTask);
            }
        }
        else
        {
            logWarn(HMIC, "unexpected surface ", ToHEX(surface.GetID()), " is created!");
        }
    }
    catch (ILMException& e)
    {
        logError(HMIC, e.what(), "Caught in AppUIHMI::ConfigSurface(", ToHEX(surface.GetID()), ")!");
    }
    catch (...)
    {
        logError(HMIC, "Unexpected exception caught in AppUIHMI::ConfigSurface()!");
    }
}

void AppUIHMI::OnSurfaceDestroy(Surface &surface)
{
    DomainVerboseLog chatter(HMIC, formatText("AppUIHMI::OnSurfaceDestroy(%#x)", surface.GetID()));

    try
    {
        if (surface == GetSurfaces().front())
        {
            UnregisterSurface(surface);
            logDebug(HMIC, "AppUIHMI::OnSurfaceDestroy(): main surface is destroyed.");
        }
        else
        {
            logWarn(HMIC, "unexpected surface is destroyed!", "surface ID:", ToHEX(surface.GetID()));
        }
    }
    catch (ILMException& e)
    {
        logWarn(HMIC, "exception is caught in AppUIHMI::OnSurfaceDestroy(", ToHEX(surface.GetID()), "): ", e.what());
    }
}

void AppUIHMI::ConfigureExistedSurfaces()
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::ConfigureExistedSurfaces()");

    Surface mainSurface = GetSurfaces().front();
    AMPID ampid = ::GetAMPID(mainSurface);
#ifdef ENABLE_DEBUG_MODE
    AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_all_surfaces in");
#endif // ENABLE_DEBUG_MODE
    try
    {
        auto surfaces = GetAllSurfaces();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
#ifdef ENABLE_DEBUG_MODE
    AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_all_surfaces out");
#endif // ENABLE_DEBUG_MODE
    for(auto& surface: surfaces)
    {
        try
        {
            if(::GetAMPID(surface)==ampid && surface!=mainSurface && surface.GetDimension().width!=0 && surface.GetDimension().height!=0)
                OnSurfaceCreate(surface);
        }
        catch(ILMException& e)
        {
            logWarn(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
        }
    }
}

void AppUIHMI::_ConfigSurface(Surface &surface, const Section &section)
{
    logVerbose(HMIC, "AppUIHMI::_ConfigSurface(", ToHEX(surface.GetID()), ")--->IN");

    try
    {
        Layer layer(surface.GetLayerID());

#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_source&destination_rectangle in");
#endif // ENABLE_DEBUG_MODE
        // set surface size
        surface.SetBothRectangle(section);
#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE set_source&destination_rectangle out");
#endif // ENABLE_DEBUG_MODE

        //    // add surface to layer
        //    if(layer.GetLayer()!=E_LAYER_NONE)
        //        layer.AddSurface(surface);

        //    // set surface z-order
        //    vector<Surface> zorder;
        //    if(isMainSurfaceCreated)
        //        zorder.push_back(GetSurfaces().front());
        //    if(resumeUI!=INVALID_SURFACE)
        //        zorder.push_back(resumeUI);
        //    if(startUI!=INVALID_SURFACE)
        //        zorder.push_back(startUI);
        //    layer.SetRelativeRenderOrder(zorder);

        // commit changes
        ILMClient::Commit();
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }

    logVerbose(HMIC, "AppUIHMI::_ConfigSurface(", ToHEX(surface.GetID()), ")--->OUT");
}

void AppUIHMI::_AddSurfaces2ReleaseLayer()
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::_AddSurfaces2ReleaseLayer()");

    try
    {
        bool isCommit = false;
        ReleaseLayer layer(LAYERID(GetSeat(), E_LAYER_RELEASE_HMI));
        for(auto& surface: surfaces)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property in");
#endif // ENABLE_DEBUG_MODE
            LayerID containorLayer = surface.GetContainerLayer();
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property out");
#endif // ENABLE_DEBUG_MODE

            logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), "is on layer ", ToHEX(containorLayer));
            if(containorLayer==layer.GetID())
            {
                logInfo(HMIC, LOG_HEAD, "surface is already on release layer. operation abort!");
                continue;
            }

            if(containorLayer!=INVALID_ID)
            {
                logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " is going to be removed from layer ", ToHEX(containorLayer), ".");
                if(GET_LAYER(containorLayer)==E_LAYER_RELEASE_PLAY || GET_LAYER(containorLayer)==E_LAYER_RELEASE_HMI)
                {
                    ReleaseLayer layer(containorLayer);
                    layer.RemoveSurface(surface);
                    isCommit = true;
                }
                else
                {
                    Layer layer(containorLayer);
                    layer.RemoveSurface(surface);
                    isCommit = true;
                }
                logInfo(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " is removed from layer ", ToHEX(containorLayer), ".");
            }

            if(containorLayer==INVALID_ID)
            {
                logDebug(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " is going to be added to layer ", ToHEX(layer.GetID()), ".");
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer in");
#endif // ENABLE_DEBUG_MODE
                layer.AddSurface(surface);
                isCommit = true;
#ifdef ENABLE_DEBUG_MODE
                AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE add_surface_to_layer out");
#endif // ENABLE_DEBUG_MODE
                logDebug(HMIC, LOG_HEAD, "surface ", ToHEX(surface.GetID()), " is added to layer ", ToHEX(layer.GetID()), ".");
            }
            else
                logWarn(HMIC, LOG_HEAD, "surface is not free to be added to release layer!");
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
        logWarn(HMIC, "AppUIHMI::_AddSurfaces2ReleaseLayer(): exception is caught! error: ", (&e)->what());
    }
}

void AppUIHMI::_RemoveSurfacesFromReleaseLayer()
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::_RemoveSurfacesFromReleaseLayer()");

    try
    {
        bool isCommit = false;
        for(auto& surface: surfaces)
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property in");
#endif // ENABLE_DEBUG_MODE
            ReleaseLayer layer(surface.GetContainerLayer());
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_surface_ext_property out");
#endif // ENABLE_DEBUG_MODE
            if(layer!=Layer(LAYERID(GetSeat(), E_LAYER_RELEASE_HMI)))
                logWarn(HMIC, "AppUIHMI::_RemoveSurfacesFromReleaseLayer(): surface ", ToHEX(surface.GetID()), "is on layer ", ToHEX(layer.GetID()), " instead of under background layer!");

#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE remove_surface_from_layer in");
#endif // ENABLE_DEBUG_MODE
            layer.RemoveSurface(surface);
            isCommit = true;
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE remove_surface_from_layer out");
#endif // ENABLE_DEBUG_MODE
            logDebug(HMIC, "AppUIHMI::_RemoveSurfacesFromReleaseLayer(): surface ", ToHEX(surface.GetID()), " is removed from layer ", ToHEX(layer.GetID()));
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
        logWarn(HMIC, "AppUIHMI::_RemoveSurfacesFromReleaseLayer(): exception is caught! error: ", (&e)->what());
    }
}

Section AppUIHMI::_GetRectangle()const
{
    DomainVerboseLog chatter(HMIC, "AppUIHMI::_GetRectangle()");

    try
    {
        Layer layer( surfaces.front().GetLayerID() );
        Section defSection = {0,0,1280,720};

#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_all_layers in");
#endif // ENABLE_DEBUG_MODE
        // validate containor-layer
        auto layers = GetAllLayers();
#ifdef ENABLE_DEBUG_MODE
        AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_all_layers out");
#endif // ENABLE_DEBUG_MODE
        if(std::find(layers.begin(), layers.end(), layer)==layers.end())
        {
            logWarn(HMIC, "AppUIHMI::_GetRectangle(): containor layer ", ToHEX(layer.GetID()), " doesn't exist.");
            return defSection;
        }
        else
        {
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_properties_of_layer in");
#endif // ENABLE_DEBUG_MODE
            // layer's section
            auto rect = layer.GetRectangle();

            // in mentor environment, the size of a layer is {0,0}
            if(rect.width==0 || rect.height==0)
            {
                rect.width = defSection.width;
                rect.height = defSection.height;
            }
#ifdef ENABLE_DEBUG_MODE
            AppProfileManager::GetInstance()->AppendLog(GetAMPID(), "ILM INVOKE get_properties_of_layer out");
#endif // ENABLE_DEBUG_MODE
            return rect;
        }
    }
    catch(ILMException& e)
    {
        logWarn(HMIC, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
}
