/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef PROTASK_H
#define PROTASK_H

#include <cassert>
#include <string>
#include <vector>

#include "Task.h"
#include "TaskDispatcher.h"

enum ProjectTaskType
{
    E_RPO_TASK_INIT = 0,
    E_RPO_TASK_CLUSTER_CONNECTED,
    E_RPO_TASK_IVI_POWER_CHANGED,
    E_PRO_TASK_CHANGE_OF_POWER,
    E_PRO_TASK_AUDIOFOCUS_CHANGED,
    E_PRO_TASK_APPSTATE_CHANGED,
    E_PRO_TASK_REQ_START_FACTORYAPP,
    E_PRO_TASK_WELINK_IOS_DEVICE_ATTACH_STA_CHANGED,
    E_PRO_TASK_WELINK_SELECTED_STA_CHANGED
};

class ProjectTask
        : public Task
{
public:
    ProjectTask(){}
    ~ProjectTask(){}

    void setInt32Arg(uint32_t arg){
        mInt32Vecter.push_back(arg);
    }
    uint32_t getInt32Arg(int i){
        return mInt32Vecter.at(i);
    }

    void setStringArg(string arg){
        mStringVector.push_back(arg);
    }
    std::string getStringArg(int i){
        return mStringVector.at(i);
    }

    void setBoolArg(bool arg){
        mBoolVector.push_back(arg);
    }
    bool getBoolArg(int i){
        return mBoolVector.at(i);
    }

    string GetTaskTypeString()
    {
        switch(GetType()){
        case E_RPO_TASK_INIT:return "E_RPO_TASK_INIT";
        case E_RPO_TASK_CLUSTER_CONNECTED:return "E_RPO_TASK_CLUSTER_CONNECTED";
        case E_RPO_TASK_IVI_POWER_CHANGED:return "E_RPO_TASK_IVI_POWER_CHANGED";
        case E_PRO_TASK_CHANGE_OF_POWER:return "E_PRO_TASK_CHANGE_OF_POWER";
        case E_PRO_TASK_AUDIOFOCUS_CHANGED:return "E_PRO_TASK_AUDIOFOCUS_CHANGED";
        case E_PRO_TASK_APPSTATE_CHANGED:return "E_PRO_TASK_APPSTATE_CHANGED";
        case E_PRO_TASK_REQ_START_FACTORYAPP:return "E_PRO_TASK_REQ_START_FACTORYAPP";
        case E_PRO_TASK_WELINK_IOS_DEVICE_ATTACH_STA_CHANGED:return "E_PRO_TASK_WELINK_IOS_DEVICE_ATTACH_STA_CHANGED";
        case E_PRO_TASK_WELINK_SELECTED_STA_CHANGED:return "E_PRO_TASK_WELINK_SELECTED_STA_CHANGED";
        default: return "ERROR TASK TYPE";
        }
    }

private:
    std::vector<uint32_t> mInt32Vecter;
    std::vector<std::string> mStringVector;
    std::vector<bool> mBoolVector;
};

#endif//PROTASK_H
