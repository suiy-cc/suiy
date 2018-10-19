/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include "LastUserCtx.h"
#include "UnitManager.h"
#include "PLCTask.h"
#include "TaskDispatcher.h"
#include "ID.h"

#include <persistence_client_library.h>
#include <persistence_client_library_error_def.h>
#include <persistence_client_library_file.h>
#include <persistence_client_library_key.h>

LastUserCtx::LastUserCtx()
{
}
LastUserCtx::~LastUserCtx()
{
   // pclDeinitLibrary();
}
void LastUserCtx::init()
{
    logVerbose(PLC, "LastUserCtx init()--->IN");
    logVerbose(PLC, "LastUserCtx init()--->OUT");
}

void LastUserCtx::recoverForeground()
{
    logVerbose(PLC, "LastUserCtx recoverForeground()  --->IN");



    std::string iviFG=readLUC(lucKeyArr[LUC_IVI_FOREGROUND]);
    logVerbose(PLC, "LastUserCtx recoverForeground() get from pcl amapid: ",iviFG);
   // std::string srcLeftFG=readLUC(lucKeyArr[LUC_SRCL_FOREGROUND]);
    //std::string srcRightFG=readLUC(lucKeyArr[LUC_SRCR_FOREGROUND]);
    int iviPID= 0;
    //int srclPID=0;
    //int scrrPID=0;

    try {
         iviPID= atol(iviFG.c_str());
        // srclPID= atol(srcLeftFG.c_str());
        // scrrPID= atol(srcRightFG.c_str());
      }
    catch (std::string & e) {
            logError(PLC, "LastUserCtx recoverForeground :ERROR: ", e);
        }
    if(iviPID!=0||iviFG!="")
    {
        auto pTask = std::make_shared<PLCTask>();
        pTask->SetType(PLC_CMD_START);
        pTask->SetAMPID(iviPID);
         pTask->SetUserData(1);
        TaskDispatcher::GetInstance()->Dispatch(pTask);
    }
    else
    {
		auto pTask = std::make_shared<PLCTask>();
		pTask->SetType(PLC_CMD_START);
		pTask->SetAMPID(AMPID_APP(E_APPID_RADIO,E_SEAT_IVI));
		pTask->SetUserData(1);
		TaskDispatcher::GetInstance()->Dispatch(pTask);
    }

    //UnitManager::GetInstance()->startUnit(srclPID,nullptr);
    //UnitManager::GetInstance()->startUnit(scrrPID,nullptr);
    logVerbose(PLC, "LastUserCtx recoverForeground()  --->OUT");
}

void LastUserCtx::recoverBackground()
{

}

void LastUserCtx::recoverAudio( )
{

}

void LastUserCtx::setAudio(){}
void LastUserCtx::setForeground(AMPID ampid)
{
    if (INVALID_AMPID!=ampid)
        logVerbose(PLC, "LastUserCtx setForeground()   ampid: ",ampid,"--->IN");
    else
        logError(PLC, "LastUserCtx writeLUC()   get invalid ampid ",ampid);
    //check if it is a audio source if not set 0
    //todo
    char tmpChar[32]={0};
    sprintf(tmpChar,"%d",ampid);
    std::string value(tmpChar);
    E_SEAT seatid = GET_SEAT(ampid);

    switch (seatid)
    {
    case E_SEAT_TEST:
        writeLUC(lucKeyArr[LUC_IVI_FOREGROUND],value);
        break;
    case E_SEAT_IVI:
        writeLUC(lucKeyArr[LUC_IVI_FOREGROUND],value);
        break;
    case E_SEAT_RSE1:
        writeLUC(lucKeyArr[LUC_SRCL_FOREGROUND],value);
        break;
    case E_SEAT_RSE2:
        writeLUC(lucKeyArr[LUC_SRCR_FOREGROUND],value);
        break;
    default:
        logError(PLC, "LastUserCtx writeLUC()   get invalid seatid ",seatid);
        break;
    }
    logVerbose(PLC, "LastUserCtx setForeground()   ampid: ",ampid,"--->OUT");
}
void LastUserCtx::setBackground(AMPID ampid)
{

}
void LastUserCtx::removeBackground(AMPID ampid)
{

}
std::string LastUserCtx::readLUC(std::string key)
{
    logVerbose(PLC, "LastUserCtx readLUC() key:",key,"--->IN");

    int ret=0;
    unsigned char buffer[65] = { 0 };
    ret = pclKeyReadData(PCL_LDBID_LOCAL, key.c_str(),  1, 0,buffer, 64);
    logVerbose(PLC, "LastUserCtx readLUC luc retValue : ",ret);
    if (ret>0)
    {
        std::string retValue((char*)buffer);
        logVerbose(PLC, "LastUserCtx readLUC luc data :",key,": ",retValue);
        logVerbose(PLC, "LastUserCtx readLUC() key:",key,"--->OUT");
        return retValue;
    }
    else
    {
        logError(PLC, "LastUserCtx::readLUC() read data failed with errCode:",ret);
        logVerbose(PLC, "LastUserCtx readLUC() key:",key,"--->OUT");
        return "";
    }

}
int LastUserCtx::writeLUC(std::string key ,std::string value)
{
    logVerbose(PLC, "LastUserCtx writeLUC()   key ",key," value ",value,"--->IN");
    int ret=0;
    ret=pclKeyWriteData(PCL_LDBID_LOCAL, key.c_str(),  1, 0, (unsigned char*)value.c_str(),value.length());
    if (ret>0)
    {
        logVerbose(PLC, "LastUserCtx write luc data :",key,": ",value);
    }
    else
    {
        logError(PLC, "LastUserCtx::writeLUC() write luc data failed with errCode:",ret);
    }
    logVerbose(PLC, "LastUserCtx writeLUC()   key ",key," value ",value,"--->OUT");
    readLUC(  key);
    return ret;
}
LastUserCtx *LastUserCtx::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static LastUserCtx m_Instance;
    return &m_Instance;
}
