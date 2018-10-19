/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PMCTASK_H
#define PMCTASK_H

#include <cassert>
#include <string>

#include "Task.h"
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "log.h"

#include "ama_enum.h"

enum PMCTaskType{
    HANDLE_MODULE_INIT = 0x0,
    HANDLE_PM_SIGNAL,
    HANDLE_LCD_SIGNAL,
    HANDLE_APP_REQ_SET_SCREENON,
    HANDLE_BOOT_SIGNAL,
    HANDLE_APP_AWAKE_SIGNAL,
};

class PMCTask: public Task
{
public:
    PMCTask(){}
    ~PMCTask(){}
    //type = powerMode
    virtual string getTaskTypeString()
    {
        switch(GetType()){
        case HANDLE_MODULE_INIT:    return "HANDLE_MODULE_INIT";
        case HANDLE_PM_SIGNAL:    return "HANDLE_PM_SIGNAL";
        case HANDLE_LCD_SIGNAL:    return "HANDLE_LCD_SIGNAL";
        case HANDLE_APP_REQ_SET_SCREENON:    return "HANDLE_APP_REQ_SET_SCREENON";
        case HANDLE_BOOT_SIGNAL:    return "HANDLE_BOOT_SIGNAL";
        case HANDLE_APP_AWAKE_SIGNAL:    return "HANDLE_APP_AWAKE_SIGNAL";
        default: return "ERROR TASK TYPE";
        }
    }

    void setPowerSignal(ama_PowerSignal_e powerSignal)
    {
        mPowerSignal=powerSignal;
    }
    ama_PowerSignal_e getPowerSignal(void)
    {
        return mPowerSignal;
    }

    void setSeatID(E_SEAT seatid)
    {
        m_seatid=seatid;
    }
    E_SEAT getSeatID(void)
    {
        return m_seatid;
    }
private :
    E_SEAT m_seatid;
    ama_PowerSignal_e mPowerSignal;
};

void sendInitTaskToPMC(void);

#endif // PMCTASK_H
