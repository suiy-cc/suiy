/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef BPMTASK_H
#define BPMTASK_H

#include <cassert>
#include <string>

#include "Task.h"
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "log.h"

enum BPM_CMD{
  BPM_CMD_NEVER_STOP,   // add to alwaysON list
  BPM_CMD_CAN_BE_STOP,  // remove from alwaysON list
  BPM_CMD_MAX
};

class BPMTask: public Task
{
public:
    BPMTask(){}
    ~BPMTask(){}
    virtual string getTaskTypeString()
    {
        switch(GetType()){
        case BPM_CMD_NEVER_STOP:     return "BPM_CMD_NEVER_STOP";
        case BPM_CMD_CAN_BE_STOP:    return "BPM_CMD_CAN_BE_STOP";
        default: return "BPMTask:ERROR TASK TYPE";
        }
    }



    void setSeatID(E_SEAT seatid)
    {
        m_seatid=seatid;
    }
    E_SEAT getSeatID(void)
    {
        return m_seatid;
    }

    AMPID getApmid() const
    {
        return m_apmid;
    }

    void setApmid(const AMPID &apmid)
    {
        m_apmid = apmid;
    }


private :
    E_SEAT m_seatid;
    AMPID  m_apmid;

};




#endif // BPMTASK_H

