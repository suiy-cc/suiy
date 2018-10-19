/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APP_STATE_H
#define APP_STATE_H

#include <memory>
#include <string>

#include "ProcState.h"

using std::shared_ptr;

class Task;
class UIProcess;

class IAppState
: virtual public IProcState
{
public:
    IAppState(E_PROC_STATE state):IProcState(state){};
    virtual bool IsVisibleState()const=0;
    virtual void ShowReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const;
    virtual void HideReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const;
    virtual void RemoveAppFromZOrderReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const;
};


class InitialState
: virtual public IAppState
, virtual public ProcDeadState
{
public:
    InitialState():IProcState(E_PROC_STATE_DEAD), IAppState(E_PROC_STATE_DEAD){};
    virtual bool StartReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override;
    virtual void IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override;
    virtual bool IsVisibleState()const override
    {
        return false;
    }
};

class StartingState
: virtual public IAppState
, virtual public ProcStartingState
{
public:
    StartingState():IProcState(E_PROC_STATE_STARTING), IAppState(E_PROC_STATE_STARTING){};
    virtual bool IsVisibleState()const override
    {
        return false;
    }
};

class ShowState
: virtual public IAppState
, virtual public ProcActiveState
{
public:
    ShowState():IProcState(E_APP_STATE_SHOW), IAppState(E_APP_STATE_SHOW){};
    virtual void StopReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override ;
    virtual void HideReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const override ;
    virtual void IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override ;
    virtual void RemoveAppFromZOrderReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const override ;
    virtual bool IsVisibleState()const override
    {
        return true;
    }
};

class VisibilityChangingState
: virtual public IAppState
, virtual public ProcActiveState
{
public:
    VisibilityChangingState(E_PROC_STATE state):IProcState(state), IAppState(state){};
    virtual bool IsStableState()const override
    {
        return false;
    }
};

class ShowingState
: public VisibilityChangingState
{
public:
    ShowingState():IProcState(E_APP_STATE_SHOWING), IAppState(E_APP_STATE_SHOWING), VisibilityChangingState(E_APP_STATE_SHOWING){};
    virtual bool IsVisibleState()const override
    {
        return false;
    }
};

class HidingState
: public VisibilityChangingState
{
public:
    HidingState():IProcState(E_APP_STATE_HIDING), IAppState(E_APP_STATE_HIDING), VisibilityChangingState(E_APP_STATE_HIDING){};
    virtual bool IsVisibleState()const override
    {
        return true;
    }
};

class HideState
: virtual public IAppState
, virtual public ProcActiveState
{
public:
    HideState():IProcState(E_APP_STATE_HIDE), IAppState(E_APP_STATE_HIDE){};
    virtual bool StartReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override;
    virtual void ShowReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const override;
    virtual void StopReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override;
    virtual void IntentReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override;
    virtual void RemoveAppFromZOrderReq(shared_ptr<UIProcess> proc, shared_ptr<Task> task)const override ;
    virtual bool IsVisibleState()const
    {
        return false;
    }
};

class StoppingState
: virtual public IAppState
, virtual public ProcStoppingState
{
public:
    StoppingState():IProcState(E_PROC_STATE_STOPPING), IAppState(E_PROC_STATE_STOPPING){};
    virtual void KillReq(shared_ptr<Process> proc, shared_ptr<Task> task)const override;
    virtual bool IsVisibleState()const
    {
        return false;
    }
};

class KilledState
: virtual public IAppState
, virtual public ProcDeadState
{
public:
    KilledState():IProcState(E_PROC_STATE_DEAD), IAppState(E_PROC_STATE_DEAD){};
    virtual bool IsVisibleState()const override
    {
        return false;
    }
};

#endif // APP_STATE_H
