/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef HMICTASK_H
#define HMICTASK_H

#include "Task.h"
#include "AMGRLogContext.h"
#include "ActiveApp.h"
#include "wd.h"
#include "TaskDispatcher.h"
#include "ID.h"
#include "HMIType.h"
#include "ama_types.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

enum HMIC_NOTIFY
{
    HMIC_NOTIFY_MODULE_INITIALIZED,
    HMIC_NOTIFY_APP_START,
    HMIC_NOTIFY_APP_STOP,
    HMIC_NOTIFY_APP_SHOW,
    HMIC_NOTIFY_APP_HIDE,
    HMIC_NOTIFY_APP_CRUSH,
    HMIC_NOTIFY_APP_SWITCH_TITLEBAR_VISIBILITY,
    HMIC_NOTIFY_TITLEBAR_DROPDOWNSTATECHANGE,
    HMIC_NOTIFY_TITLEBAR_EXPAND,
    HMIC_NOTIFY_TITLEBAR_SHRINK,
    HMIC_NOTIFY_ILM_OBJ_CHANGED,
    HMIC_NOTIFY_SURFACE_AVAILABLE,
    HMIC_NOTIFY_SURFACE_CREATED,
    HMIC_NOTIFY_SURFACE_DESTROYED,
    HMIC_NOTIFY_SURFACE_DESTINATION_RECTANGLE_CHANGED_ON_RELEASE_LAYER,
    HMIC_NOTIFY_ANIMATION_STARTUP_PLAY,
    HMIC_NOTIFY_ANIMATION_STARTUP_STOP,
    HMIC_NOTIFY_APP_PRESHOW, // for pre-show interact
    HMIC_NOTIFY_IME_SHOW,
    HMIC_NOTIFY_IME_HIDE,
    HMIC_NOTIFY_TOUCHDEVICE_CREATED,
    HMIC_NOTIFY_TOUCHDEVICE_DESTROYED,
    HMIC_NOTIFY_ILM_FATALERROR, // notify HMIC that ILM encounters fatal error.

    HMIC_CMD_SYNC_APP_START,
    HMIC_CMD_SYNC_APP_END,
    HMIC_CMD_ENABLE_TOUCH_APP,
    HMIC_CMD_ENABLE_CHILD_SAFETY_LOCK,
    HMIC_CMD_ENABLE_TOUCH_SCREEN,
    HMIC_CMD_SET_MASK_SUR_VISIBLE,
    HMIC_CMD_ENABLE_GESTURE_RECOGNIZER,
    HMIC_CMD_TITLEBAR_DROPDOWN,
    HMIC_CMD_REMOVE_APP_FROM_ZORDER,
    HMIC_CMD_ADD_EXTRA_SURFACE,
    HMIC_CMD_START_SCREEN_MONITOR,
    HMIC_CMD_CHECK_SCREEN_STATUS, // check if screen is empty, if so, start home app on this screen

    HMIC_QUERY_TITLEBAR_VISIBILITY_FOR_SCREEN,
    HMIC_QUERY_TITLEBAR_DROPDOWNSTATE,
    HMIC_QUERY_SYNC_GET_CURRENT_SHARE,
    HMIC_QUERY_SYNC_SOURCE_SURFACES,
    HMIC_QUERY_SYNC_WORKING_LAYERS,
    HMIC_QUERY_GET_AVAILABLE_SCREENS,
    HMIC_QUERY_GET_SCREEN_SIZE,
    HMIC_REQUEST_ALL_SCREEN_SHOT,
};

class HMICTask
        : public Task
{
public:
    HMICTask():app(0),surfaceID(0){}
    ~HMICTask(){}
    void SetAMPID(AMPID app)
    {
        this->app = app;
    }
    AMPID GetAMPID()const
    {
        return app;
    }
    SurfaceID GetSurfaceID() const
    {
        return surfaceID;
    }
    void SetSurfaceID(SurfaceID surfaceID)
    {
        HMICTask::surfaceID = surfaceID;
    }

    virtual string GetTaskTypeString()
    {
        switch(GetType())
        {
            case HMIC_NOTIFY_MODULE_INITIALIZED:return "HMIC_NOTIFY_MODULE_INITIALIZED";
            case HMIC_NOTIFY_APP_START:return "HMIC_NOTIFY_APP_START";
            case HMIC_NOTIFY_APP_STOP:return "HMIC_NOTIFY_APP_STOP";
            case HMIC_NOTIFY_APP_SHOW:return "HMIC_NOTIFY_APP_SHOW";
            case HMIC_NOTIFY_APP_HIDE:return "HMIC_NOTIFY_APP_HIDE";
            case HMIC_NOTIFY_APP_CRUSH:return "HMIC_NOTIFY_APP_CRUSH";
            case HMIC_NOTIFY_APP_SWITCH_TITLEBAR_VISIBILITY:return "HMIC_NOTIFY_APP_SWITCH_TITLEBAR_VISIBILITY";
            case HMIC_NOTIFY_TITLEBAR_DROPDOWNSTATECHANGE:return "HMIC_NOTIFY_TITLEBAR_DROPDOWNSTATECHANGE";
            case HMIC_NOTIFY_TITLEBAR_EXPAND:return "HMIC_NOTIFY_TITLEBAR_EXPAND";
            case HMIC_NOTIFY_TITLEBAR_SHRINK:return "HMIC_NOTIFY_TITLEBAR_SHRINK";
            case HMIC_NOTIFY_ILM_OBJ_CHANGED:return "HMIC_NOTIFY_ILM_OBJ_CHANGED";
            case HMIC_NOTIFY_SURFACE_AVAILABLE:return "HMIC_NOTIFY_SURFACE_AVAILABLE";
            case HMIC_NOTIFY_SURFACE_CREATED:return "HMIC_NOTIFY_SURFACE_CREATED";
            case HMIC_NOTIFY_SURFACE_DESTROYED:return "HMIC_NOTIFY_SURFACE_DESTROYED";
            case HMIC_NOTIFY_SURFACE_DESTINATION_RECTANGLE_CHANGED_ON_RELEASE_LAYER:return "HMIC_NOTIFY_SURFACE_DESTINATION_RECTANGLE_CHANGED_ON_RELEASE_LAYER";
            case HMIC_NOTIFY_ANIMATION_STARTUP_PLAY:return "HMIC_NOTIFY_ANIMATION_STARTUP_PLAY";
            case HMIC_NOTIFY_ANIMATION_STARTUP_STOP:return "HMIC_NOTIFY_ANIMATION_STARTUP_STOP";
            case HMIC_NOTIFY_APP_PRESHOW:return "HMIC_NOTIFY_APP_PRESHOW";
            case HMIC_NOTIFY_IME_SHOW:return "HMIC_NOTIFY_IME_SHOW";
            case HMIC_NOTIFY_IME_HIDE:return "HMIC_NOTIFY_IME_HIDE";
            case HMIC_NOTIFY_TOUCHDEVICE_CREATED:return "HMIC_NOTIFY_TOUCHDEVICE_CREATED";
            case HMIC_NOTIFY_TOUCHDEVICE_DESTROYED:return "HMIC_NOTIFY_TOUCHDEVICE_DESTROYED";
            case HMIC_NOTIFY_ILM_FATALERROR:return "HMIC_NOTIFY_ILM_FATALERROR";
            case HMIC_CMD_SYNC_APP_START:return "HMIC_CMD_SYNC_APP_START";
            case HMIC_CMD_SYNC_APP_END:return "HMIC_CMD_SYNC_APP_END";
            case HMIC_CMD_ENABLE_TOUCH_APP:return "HMIC_CMD_ENABLE_TOUCH_APP";
            case HMIC_CMD_ENABLE_CHILD_SAFETY_LOCK:return "HMIC_CMD_ENABLE_CHILD_SAFETY_LOCK";
            case HMIC_CMD_ENABLE_TOUCH_SCREEN:return "HMIC_CMD_ENABLE_TOUCH_SCREEN";
            case HMIC_CMD_SET_MASK_SUR_VISIBLE:return "HMIC_CMD_SET_MASK_SUR_VISIBLE";
            case HMIC_CMD_ENABLE_GESTURE_RECOGNIZER:return "HMIC_CMD_ENABLE_GESTURE_RECOGNIZER";
            case HMIC_CMD_TITLEBAR_DROPDOWN:return "HMIC_CMD_TITLEBAR_DROPDOWN";
            case HMIC_CMD_REMOVE_APP_FROM_ZORDER:return "HMIC_CMD_REMOVE_APP_FROM_ZORDER";
            case HMIC_CMD_ADD_EXTRA_SURFACE:return "HMIC_CMD_ADD_EXTRA_SURFACE";
            case HMIC_CMD_START_SCREEN_MONITOR:return "HMIC_CMD_START_SCREEN_MONITOR";
            case HMIC_CMD_CHECK_SCREEN_STATUS:return "HMIC_CMD_CHECK_SCREEN_STATUS";
            case HMIC_QUERY_TITLEBAR_VISIBILITY_FOR_SCREEN:return "HMIC_QUERY_TITLEBAR_VISIBILITY_FOR_SCREEN";
            case HMIC_QUERY_TITLEBAR_DROPDOWNSTATE:return "HMIC_QUERY_TITLEBAR_DROPDOWNSTATE";
            case HMIC_QUERY_SYNC_GET_CURRENT_SHARE:return "HMIC_QUERY_SYNC_GET_CURRENT_SHARE";
            case HMIC_QUERY_SYNC_SOURCE_SURFACES:return "HMIC_QUERY_SYNC_SOURCE_SURFACES";
            case HMIC_QUERY_SYNC_WORKING_LAYERS:return "HMIC_QUERY_SYNC_WORKING_LAYERS";
            case HMIC_QUERY_GET_AVAILABLE_SCREENS:return "HMIC_QUERY_GET_AVAILABLE_SCREENS";
            case HMIC_QUERY_GET_SCREEN_SIZE:return "HMIC_QUERY_GET_SCREEN_SIZE";
            case HMIC_REQUEST_ALL_SCREEN_SHOT:return "HMIC_REQUEST_ALL_SCREEN_SHOT";
            default: return "error task type or forget to update task type table.";
        }
    }

private:
    AMPID app;
    SurfaceID surfaceID;
};

static std::shared_ptr<HMICTask> GetScreenQueryTask()
{
    std::vector<E_SEAT>* screens = new std::vector<E_SEAT>;
    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetPriority(E_TASK_PRIORITY_HIGH);
    hmicTask->SetType(HMIC_QUERY_GET_AVAILABLE_SCREENS);
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(screens), [](unsigned long long screens){
        delete reinterpret_cast<std::vector<E_SEAT>*>(screens);
    });

    return hmicTask;
}

std::vector<E_SEAT> GetAvailableScreens(bool wait4ever = false);
void GetAvailableScreens(std::function<void(std::vector<E_SEAT>)> replyer);

typedef tuple<bool, std::string> BSTuple;
static void SendEnableTouchForbidder(E_SEAT seat, HMIC_NOTIFY tfType, bool enable, string png = std::string())
{
    auto userData = new BSTuple(enable, png);

    auto hmicTask = std::make_shared<HMICTask>();
    hmicTask->SetAMPID(AMPID_AMGR(seat, E_APPID_TOUCH_FORBIDDER));
    hmicTask->SetUserData(reinterpret_cast<unsigned long long>(userData), [](unsigned long long userData){
        delete reinterpret_cast<BSTuple*>(userData);
    });
    hmicTask->SetType(tfType);

    TaskDispatcher::GetInstance()->Dispatch(hmicTask);
}

static void SendEnableTouchScreen(E_SEAT seat, bool enable, string png = std::string())
{
    SendEnableTouchForbidder(seat, HMIC_CMD_ENABLE_TOUCH_SCREEN, enable, png);
}

static void SendEnableChildSafetyLock(E_SEAT seat, bool enable, string png = std::string())
{
    SendEnableTouchForbidder(seat, HMIC_CMD_ENABLE_CHILD_SAFETY_LOCK, enable, png);
}

#ifdef _DEBUG_MEMORY_
    #undef new
#endif

#endif // HMICTASK_H
