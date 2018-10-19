/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "RecoverAppMgr.h"
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include "TaskDispatcher.h"
#include "AMGRLogContext.h"

#include <persistence_client_library.h>
#include <persistence_client_library_error_def.h>
#include <persistence_client_library_file.h>
#include <persistence_client_library_key.h>



RecoverAppMgr::RecoverAppMgr()
{

    m_RecoverState=false;
}
RecoverAppMgr::~RecoverAppMgr()
{

}

bool RecoverAppMgr::getRecoverState()
{
    return m_RecoverState;
}
bool RecoverAppMgr::init()
{
    std::string ShutDownFlag=FLAG_RECOVER;
    ShutDownFlag = readShutdownFlag(SHUTDOWN_FLAG_KEY);
    int retVal=0;

    if (!ShutDownFlag.compare(FLAG_RECOVER))
    {//RECOVER sequence
        logInfo(PLC, "RecoverAppMgr sequence:RECOVER");
        writeShutdownFlag(SHUTDOWN_FLAG_KEY,FLAG_RECOVER);
        if (retVal>0){
            m_RecoverState=true;
            logInfo(PLC, "RecoverAppMgr set FLAG_RECOVER");
            //1.shutdown AC
            ShutDownAC();
            //2.get Started App List
            GetStartedAppList();
            //3.shutdown all started apps & it's service
            ShutDownApplication();
            ShutDownAppServer();
            //4.restart all app service
            RestartAppServer();
            //5.restart last app
            RestartApp();
            return true;
        }
        else{
            return false;
        }
    }
    else //if (!ShutDownFlag.compare(FLAG_LUC))
    {//LUC sequence
        logInfo(PLC, "RecoverAppMgr sequence:LUC");
        m_RecoverState=false;
        retVal=writeShutdownFlag(SHUTDOWN_FLAG_KEY,FLAG_RECOVER);
        if (retVal>0){
            logInfo(PLC, "RecoverAppMgr set FLAG_RECOVER");
        }
        else{
            return false;
        }

    }

}


bool RecoverAppMgr::setNormalShutDown()
{
    logVerbose(PLC, "RecoverAppMgr setNormalShutDown()--> IN");
    int retVal=writeShutdownFlag(SHUTDOWN_FLAG_KEY,FLAG_LUC);
    logInfo(PLC, "setNormalShutDown retVal :",retVal);
    logVerbose(PLC, "RecoverAppMgr setNormalShutDown()-->OUT");
    return retVal;
}

//1.shutdown AC
bool RecoverAppMgr::ShutDownAC()
{return true;}
//2.get Started App List
void RecoverAppMgr::GetStartedAppList()
{}
//3.shutdown all started apps & it's service
void RecoverAppMgr::ShutDownApplication()
{}
void RecoverAppMgr::ShutDownAppServer()
{}
//4.restart all app service
void RecoverAppMgr::RestartAppServer()
{}
//5.restart last app
void RecoverAppMgr::RestartApp()
{}

std::string RecoverAppMgr::readShutdownFlag(std::string key)
{
    logVerbose(PLC, "RecoverAppMgr readShutdownFlag() key:",key,"--->IN");

    int ret=0;
    unsigned char buffer[8] = { 0 };
    std::string tmpKey="RECOVER/FLAG";
    ret = pclKeyReadData(PCL_LDBID_LOCAL, tmpKey.c_str(),  1, 0,buffer, 8);
    logInfo(PLC, "RecoverAppMgr readShutdownFlag retValue : ",ret);
    if (ret>0)
    {
        std::string retValue((char*)buffer);
        logInfo(PLC, "RecoverAppMgr readShutdownFlag data :",key,": ",ret);
        logVerbose(PLC, "RecoverAppMgr readShutdownFlag() key:",key,"--->OUT");
        return retValue;
    }
    else
    {
        logError(PLC, "RecoverAppMgr::readShutdownFlag() read data failed with errCode:",ret);
        logVerbose(PLC, "RecoverAppMgr readShutdownFlag() key:",key,"--->OUT");
        return "";
    }

}
int RecoverAppMgr::writeShutdownFlag(std::string key ,std::string value)
{
    logVerbose(PLC, "RecoverAppMgr writeShutdownFlag()   key ",key," value ",value,"--->IN");
    int ret=0;
    ret=pclKeyWriteData(PCL_LDBID_LOCAL, key.c_str(),  1, 0, (unsigned char*)value.c_str(),value.length());
    if (ret>0)
    {
        logInfo(PLC, "RecoverAppMgr write data :",key,": ",value);
    }
    else
    {
        logError(PLC, "RecoverAppMgr::writeShutdownFlag() write data failed with errCode:",ret);
    }
    logVerbose(PLC, "RecoverAppMgr writeShutdownFlag()   key ",key," value ",value,"--->OUT");

    return ret;
}

RecoverAppMgr *RecoverAppMgr::GetInstance()
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static RecoverAppMgr m_Instance;
    return &m_Instance;
}
