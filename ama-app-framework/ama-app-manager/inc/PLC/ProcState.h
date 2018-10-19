/*
 * Copyright (C) 2016-2018 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file ProcState.h
/// @brief contains
/// 
/// Created by zs on 2018/07/13.
///
/// this file contains the definition of class ProcState
/// 

#ifndef PROCSTATE_H
#define PROCSTATE_H

#include <string>

enum E_PROC_STATE{
    E_PROC_STATE_DEAD = 0x10,
    E_PROC_STATE_STARTING = 0x20,

    // active state has a lot of subtype
        E_PROC_STATE_ACTIVE = 0x30,
        E_APP_STATE_SHOW = 0x31,
        E_APP_STATE_HIDING = 0x32,
        E_APP_STATE_SHOWING = 0x33,
        E_APP_STATE_HIDE = 0x34,

    E_PROC_STATE_STOPPING = 0x40,
};

#define IS_ACTIVE_STATE(state) static_cast<bool>((state&0xF0)==E_PROC_STATE_ACTIVE)

class Task;
class Process;

class IProcState
{
public:
    IProcState(E_PROC_STATE procState):state(procState){};

    virtual bool StartReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const;
    virtual void IntentReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const;
    virtual void StopReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const;
    virtual void KillReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const;

    virtual std::string GetStateString()const
    {
        switch(GetState())
        {
            case E_PROC_STATE_DEAD:return "E_PROC_STATE_DEAD";
            case E_PROC_STATE_STARTING:return "E_PROC_STATE_STARTING";
            case E_APP_STATE_SHOW:return "E_APP_STATE_SHOW";
            case E_APP_STATE_HIDING:return "E_APP_STATE_HIDING";
            case E_APP_STATE_SHOWING:return "E_APP_STATE_SHOWING";
            case E_APP_STATE_HIDE:return "E_APP_STATE_HIDE";
            case E_PROC_STATE_STOPPING:return "E_PROC_STATE_STOPPING";
            default: return "ERROR STATE";
        }
    }
    virtual E_PROC_STATE GetState()const{return state;};
    virtual bool IsStableState()const=0;
    void OnEnterStableState(std::shared_ptr<Process> proc);

protected:
    static void _DelayCurrentRequest(std::shared_ptr<Process> proc, std::shared_ptr<Task> task);

private:
    E_PROC_STATE state;
};

#define PRINT_WRONG_STATE_LOG logWarn(PLC,"Wrong state to receive ", __STR_FUNCTION__, ", current state is ", typeid(*this).name())

class ProcDeadState
: virtual public IProcState
{
public:
    ProcDeadState():IProcState(E_PROC_STATE_DEAD){};

    virtual bool StartReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const override;

    virtual bool IsStableState() const override
    {
        return true;
    }
};

class ProcStartingState
: virtual public IProcState
{
public:
    ProcStartingState():IProcState(E_PROC_STATE_STARTING){};
    virtual bool IsStableState() const override
    {
        return false;
    }
};

class ProcActiveState
: virtual public IProcState
{
public:
    ProcActiveState():IProcState(E_PROC_STATE_ACTIVE){};

    virtual void IntentReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const override;
    virtual void StopReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const override;

    virtual bool IsStableState() const override
    {
        return true;
    }
};

class ProcStoppingState
: virtual public IProcState
{
public:
    ProcStoppingState():IProcState(E_PROC_STATE_STOPPING){};

    virtual void KillReq(std::shared_ptr<Process> proc, std::shared_ptr<Task> task)const override;

    virtual bool IsStableState() const override
    {
        return false;
    }
};

#endif // PROCSTATE_H
