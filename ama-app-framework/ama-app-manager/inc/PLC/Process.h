/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file Process.h
/// @brief contains class Process
///
/// Created by zs on 2016/10/13.
/// this file contains the definition of class Process
///

#ifndef PROCESS_H
#define PROCESS_H

#include <memory>
#include <map>

#include "ama_types.h"
#include "IntentInfo.h"
#include "ama_audioTypes.h"
#include "ProcState.h"
#include "TASK/Task.h"
#include "COMMON/Broadcaster.h"

using std::shared_ptr;

typedef std::function<void(AMPID app, E_PROC_STATE state)> AppShareStateChangeNotifyCB;

class Process
: public std::enable_shared_from_this<Process>
{
public:
    /// @name constructors & destructors
    /// @{
    Process();
    Process(AMPID app);
    Process(AMPID app, std::shared_ptr<IProcState> procState);
    virtual ~Process(){};
    /// @}

    /// @name operators
    /// @{
    bool operator==(const Process &rhs) const;
    bool operator!=(const Process &rhs) const;
    bool operator<(const Process &rhs) const;
    bool operator>(const Process &rhs) const;
    bool operator<=(const Process &rhs) const;
    bool operator>=(const Process &rhs) const;
    /// @}

    /// @name getters & setters
    /// @{
    AMPID GetAMPID()const;
    virtual void SetAMPID(AMPID ID);
    std::string GetBootCMD();
    void SetBootCMD(std::string cmd);
    bool IsTheSame(shared_ptr<Process> app) const;
    E_APPID GetAppID()const;
    E_SEAT GetSeat()const;
    bool IsAutoStart()const;
    unsigned int GetPID()const;
    void SetPID(unsigned int pid);
    /// @}

    /// @name status
    /// @{
    virtual bool IsStarting()const;
    virtual bool IsStopping()const;
    virtual bool IsOperating()const;
    virtual AppStatus GetStatus()const;
    shared_ptr<const IProcState> GetState()const;
    void SetState(const shared_ptr<IProcState> state);
    /// @}

    /// @name operations
    /// @{
    bool Start();
    void Stop();
    /// @}

    /// @name deferred task queue
    /// @{
    std::list<shared_ptr<Task>> PopAllDeferredTasks();
    void AddDeferredTask(shared_ptr<Task> plcTask);
    int getDeferredTaskCount();
    /// @}

    /// @name notifies
    /// @{
    virtual void InitializeNotify(int startArg, bool isVisible);
    virtual void IntentNotify(const IntentInfo &intent);
    virtual void StopNotify();
    /// @}

    bool isHaveAudioFocus(void);
    void updateFocusStaByAC(ama_streamID_t stream,ama_focusSta_t focusSta);

    bool isAlwaysAliveApp(void);
    void updateAlwaysAliveAppBPM(ama_streamID_t stream,bool status);

    /// @name broadcasters
    /// @{
    /// app state change broadcaster
    static Broadcaster<void(AMPID app, E_PROC_STATE state)> appStateChangeBroadcaster;
    /// @}

private:
    AMPID ampid;

    unsigned int pid;

    // unit name
    std::string bootCMD;

    // process state
    ThreadSafeValueKeeper<shared_ptr<IProcState>> state;

    // deferred task queue
    ThreadSafeValueKeeper<std::list<shared_ptr<Task>>> deferredTasks;

    // focus management
    bool isHaveFocus;
    bool isAliveApp;
    std::map<ama_streamID_t,ama_focusSta_t> mStreamFocusMap;
};

/// @name notifies
/// @{
void SendCrushNotify(shared_ptr<Process> process);
void SendCrushNotify(AMPID proc);
void ShutdownNotify(unsigned int type);
/// @}

#endif // ACTIVEPROCESS_H
