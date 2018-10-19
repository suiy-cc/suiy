/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file HMICCtrl.h
/// @brief this file contains class HMIController
///
/// Creator and date is not traceable
/// this file contains the definition of class HMIController
///

#ifndef HMICCTRL_H
#define HMICCTRL_H

#include "Task.h"
#include "Screen.h"
#include "SurfaceCreater.h"
#include "COMMON/Broadcaster.h"
#include "PNG.h"
#include "AnimationPlayerManager.h"
#include "InputPeripheral.h"
#include "SMCtrl.h"

#include <memory>

using namespace std;

class ActiveApp;
class UIProcess;
class AppUIHMI;
class TouchRecognizer;
class PNG;
class HMICTask;
class ZOrder;

typedef map<E_SEAT, shared_ptr<PNG>> PngMap;
typedef map<E_SEAT, shared_ptr<thread>> SurfaceHostMap;
typedef map<E_SEAT, shared_ptr<TouchRecognizer>> GRMap;

/// @class HMIController
/// @brief the abstraction of HMIC
class HMIController
{
public:
    HMIController();
    virtual ~HMIController();

    /// called by TaskHost
    void doTask(shared_ptr<Task> task);

private:
    friend class ServiceHMIC;

    static HMIController* GetInstance();
    static void SetInstance(HMIController* hmic);
    /// @name initialization-related
    /// @{
    ama_Error_e Initialize();
    void Destroy();
    void CreateTouchForbidder();
    void DeleteTouchForbidder();
    void CreateBackGround();
    void DeleteBackGround();
    void StartDCS();
    void StopDCS();
    void CreateGRSurface();
    void DeleteGRSurface();
    void CreateIVIMaskSurface();
    void DeleteIVIMaskSurface();
    void CreateAPSurface();
    void DeleteAPSurface();
    void UpdateTouchDevice();
    void TitlebarDropDownStateInit();
    void ScreenCreate();
    void LayerCreate();
    void RegisterInputDeviceChangeCallback();
    void RegisterSurfaceChangeCallback();
    void RegisterGestureCallback();
    void SetRenderOrder(E_SEAT seat, shared_ptr<Screen> screen);
    void CreateGestureRecognizer();
    void DeleteGestureRecognizer();
    void CreateAPPicture();
    void DeleteAPPicture();
    /// @}

    /// @name callbacks
    /// @{
    void ILMFatalErrorCB(unsigned int error);
    static void callbackFunction(ilmObjectType object, t_ilm_uint id, t_ilm_bool created);
    static void surfaceCallback(SurfaceID surface, SurfaceProperties* properties, NotificationMask mask);
    static void touchCallback(SurfaceID surfaceID, E_TOUCH touchType, float x, float y, int fingerID, unsigned int id);
    static void doubleTapCallback(SurfaceID surfaceID);
    static void singleTapCallback(SurfaceID surfaceID);
    static void drawPicture(E_SEAT seat, unsigned int *pixels, unsigned int width, unsigned int height, const ThreadSafeValueKeeper<PngMap> &pngMap);
    static void drawTouchForbidderPictureIVI(unsigned int *pixels, unsigned int width, unsigned int height);
    static void drawTouchForbidderPictureRSE1(unsigned int *pixels, unsigned int width, unsigned int height);
    static void drawTouchForbidderPictureRSE2(unsigned int *pixels, unsigned int width, unsigned int height);
    static void drawAPPictureIVI(unsigned int *pixels, unsigned int width, unsigned int height);
    static void drawAPPictureRSE1(unsigned int *pixels, unsigned int width, unsigned int height);
    static void drawAPPictureRSE2(unsigned int *pixels, unsigned int width, unsigned int height);
    static void SystemLanguageChangedCB(const std::string& key, const SMValue& value, const SMValueObserver::E_EVENT event);
    void TouchDeviceCreatedCB(const std::string &deviceName, bool isCreate);
    /// @}

    void SetExtraSurfaceOriginalSize(Surface& surface);
    void SetAMGRSurfaceOriginalSize(Surface& surface);
    void SetSurfaceOriginalSize(Surface& surface);
    void ConfigureExtraSurface(Surface& surface);
    void ConfigureAMGRSurface(Surface& surface);
    void ConfigureSurface(Surface& surface);
    void DestroySurface(Surface& surface);

    /// @name zorder operations
    /// @{
    virtual void SetTopApp(AMPID app);
    virtual AMPID GetTopApp(E_SEAT seat) const;
    virtual void RemoveApp(AMPID app);
    virtual void ClearZOrder(E_SEAT seat);
    /// @}

    /// @name notify handler
    /// @{
    virtual void OnModuleInitialized(shared_ptr<HMICTask> pTask);
    virtual void OnAppShow(AMPID ampid);
    virtual void OnAppHide(AMPID ampid);
    virtual void OnAppStart(shared_ptr<HMICTask> pTask);
    virtual void OnAppStop(AMPID ampid);
    virtual void OnUIProcessCrush(AMPID ampid);
    virtual void OnTitlebarExpand(AMPID titlebar);
    virtual void OnTitlebarShrink(AMPID titlebar);
    virtual void OnAppSwitchTitlebarVisibility(shared_ptr<HMICTask> pTask);
    virtual void OnTitlebarDropDownStateChange(shared_ptr<HMICTask> pTask);
    virtual void OnILMOBJChanged(shared_ptr<HMICTask> pTask);
    virtual void OnSurfaceAvailable(shared_ptr<HMICTask> pTask);
    virtual void OnSurfaceCreated(shared_ptr<HMICTask> pTask);
    virtual void OnSurfaceDestroyed(shared_ptr<HMICTask> pTask);
    virtual void OnSurfaceRectangleChangedOnReleaseLayer(shared_ptr<HMICTask> pTask);
    virtual void OnPlayStartupAnimation(AMPID ampid);
    virtual void OnStopStartupAnimation(AMPID ampid);
    virtual void OnShowIME(shared_ptr<HMICTask> pTask);
    virtual void OnStopIME(shared_ptr<HMICTask> pTask);
    virtual void OnTouchDeviceChanged(shared_ptr<HMICTask> pTask);
    virtual void OnSynchronizeAppStart(shared_ptr<HMICTask> pTask);
    virtual void OnSynchronizeAppEnd(shared_ptr<HMICTask> pTask);
    virtual void OnDropDownTitlebar(shared_ptr<HMICTask> pTask);
    virtual void OnRemoveAppFromZOrder(shared_ptr<HMICTask> pTask);
    virtual void OnAddExtraSurface(shared_ptr<HMICTask> pTask);
    virtual void OnStartScreenMonitor(shared_ptr<HMICTask> pTask);
    virtual void OnCheckScreenStatus(shared_ptr<HMICTask> pTask);
    virtual void OnTitlebarVisibilityQueryForScreen(shared_ptr<HMICTask> pTask);
    virtual void OnTitlebarDropDownStateQuery(shared_ptr<HMICTask> pTask);
    virtual void OnGetCurrentShare(shared_ptr<HMICTask> pTask);
    virtual void OnGetSourceSurfacesQuery(shared_ptr<HMICTask> pTask);
    virtual void OnGetWorkingLayersQuery(shared_ptr<HMICTask> pTask);
    virtual void OnGetAvailableScreens(shared_ptr<HMICTask> pTask);
    virtual void OnGetScreenSize(shared_ptr<HMICTask> pTask);
    virtual void OnEnableTouchApp(shared_ptr<HMICTask> pTask);
    virtual void OnEnableChildSafetyLock(shared_ptr<HMICTask> pTask);
    virtual void OnEnableTouchScreen(shared_ptr<HMICTask> pTask);
    virtual void OnEnableGR(shared_ptr<HMICTask> pTask);
    virtual void OnILMFatalError(shared_ptr<HMICTask> pTask);
    virtual void OnSetMaskSurfaceVisible(shared_ptr<HMICTask> pTask);
    /// @}

    /// @name helper functions
    /// @{
    static shared_ptr<ActiveApp> GetApp(AMPID ampid);
    static shared_ptr<UIProcess> GetUIProc(AMPID ampid);
    static shared_ptr<AppUIHMI> GetHMI(AMPID ampid);
    static void Show(AMPID uiProc);
    static void Hide(AMPID uiProc);
    void SwitchTitlebarVisibility(AMPID app, bool isShow, int style);
    static void EnableTouchApp(AMPID, bool enable);
    static void EnableTouch(E_SEAT seat, E_LAYER layer, bool enable = true);
    // return available seats
    std::vector<E_SEAT> GetAvailableScreens();
    void SetPicture4TouchForbidder(E_SEAT seat, string png);
    void PlayStartupAnimation(AMPID proc);
    void StopStartupAnimation(AMPID proc);
    void SwitchAP(AMPID old, AMPID neW);
    void SetInputDevice(Surface& surface);
    /// @}
    //for screen shot
    void GetAllScreenShot();
    int HMICmkdir(const char* path);
    void CheckScreenShotNumber(const char* path);

private:
    std::map<E_SEAT, shared_ptr<ZOrder>> zOrder;
    std::map<E_SEAT, shared_ptr<Screen>> screens;
    std::map<E_SEAT, InputPeripheral> touchPanels; // touch-screen
    std::map<E_SEAT, bool> titlebarDropDownState;

    //SurfaceHostMap normalAppTouchForbidder;
    SurfaceHostMap childSafetyLockTF;
    SurfaceHostMap globalTouchForbidder;
    SurfaceHostMap background;
    SurfaceHostMap gestureRecognizer;
    SurfaceHostMap animationPlayer;

    shared_ptr<thread> mIVIMaskSurfaceThread;

    // Double-click-scanner's visibility can be changed
    // by other module before its configuration.
    // once such thing happens, they modify this so that
    // their modification will not be abandoned.
    std::map<E_SEAT, bool> DCSConfigureTimeVisibility;

    GRMap doubleTapRecognizer;
    GRMap singleTapRecognizer;

    // singlton pattern
    static HMIController* instance;

    // for RSE touch Forbidder
    ThreadSafeValueKeeper<PngMap>                   touchForbidderPicture;
    static const std::map<E_SEAT, RedrawCallback>   touchForbidderRedrawCB;
    // for animation player
    ThreadSafeValueKeeper<PngMap> animationPlayerPicture;
    static const std::map<E_SEAT, RedrawCallback> animationPlayerRedrawCB;

    AnimationPlayerManager APManager;

    // extra surface management
    std::unordered_map<Surface, Layer> extraSurfaceMapping;

    // screen monitor
    shared_ptr<thread> screenMonitor;
    unsigned long long screenMonitorHandle;

    // ILM object existance callback's handle
    unsigned long long objExistanceCBHandle;
};

#endif // HMICCTRL_H
