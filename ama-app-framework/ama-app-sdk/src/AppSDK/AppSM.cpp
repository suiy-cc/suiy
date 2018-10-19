/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppSM.cpp
/// @brief contains the implementation of State Manager's APIs
///
/// Created by zs on 2016/8/4.
/// this file contains the implementation of State Manager's APIs
///

#include "log.h"
#include "AppSM.h"
#include "AppManager.h"
#include "LogContext.h"

inline std::string c_time(std::string key)
{
    struct tm *p;
    time_t now_time;
    now_time = time(NULL);
    p=localtime(&now_time);
    std::string cenv;

    if(key=="syssetting_SYS_TIME")
    {
        static char c_time[9];
        sprintf(c_time, "%.2d:%.2d:%.2d",p->tm_hour,p->tm_min, p->tm_sec);
        cenv = c_time;
    }
    else// if(key==STATMGR_SYS_DATE)
    {
        static char c_date[11];
        //strftime(str,100,"%x",ptr);
        sprintf(c_date, "%.4d/%.2d/%.2d",1900 + p->tm_year,1+p->tm_mon,p->tm_mday);
        //cenv = to_string(1900+p->tm_year) + "/" + to_string(1+p->tm_mon) + "/" + to_string(p->tm_mday);
        cenv = c_date;
    }

    return cenv;
}

bool SM_isStateExist(std::string state)
{
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_isStateExist-->>IN");
	::v0_1::org::neusoft::AppMgr::key_value::iterator it = app_key_value.find(state);
	if (it != app_key_value.end())
	{
		logInfo(SDK_AMGR,"AppSM.cpp::SM_isStateExist::state::",state,"exist");
		logVerbose(SDK_AMGR,"AppSM.cpp::SM_isStateExist-->>OUT");
		return true;
	}
	else
	{
		logWarn(SDK_AMGR,"AppSM.cpp::SM_isStateExist::state::",state,"is not exist");
		logVerbose(SDK_AMGR,"AppSM.cpp::SM_isStateExist-->>OUT");
		return false;
	}
}

std::string SM_getState(std::string state)
{
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_getState",state.c_str(),"-->>IN");
	if((state=="syssetting_SYS_TIME")||(state=="syssetting_SYS_DATE"))
    {
        return c_time(state);
    }
    else
    {
		::v0_1::org::neusoft::AppMgr::key_value::iterator it = app_key_value.find(state);
		if (it != app_key_value.end())
		{
			logVerbose(SDK_AMGR,"AppSM.cpp::SM_getState",state.c_str()," =",app_key_value[state].getValue().c_str(),"-->>OUT");
			return app_key_value[state].getValue();
		}
		else
		{
			logError(SDK_AMGR,"AppSM.cpp::SM_getState::state::",state,"is not exist-->>OUT");
			return "unknown";
		}
    }
}

void SM_req2SetState(std::string state,std::string value)
{
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_req2SetState::",state.c_str()," =",value.c_str(),"-->>IN");
	::v0_1::org::neusoft::AppMgr::key_value::iterator it = app_key_value.find(state);
	if (it != app_key_value.end())
	{
		AppManager::GetInstance()->statMgr_SetReq(state,value);
	}
	else
	{
		logError(SDK_AMGR,"AppSM.cpp::SM_req2SetState::",state,"is not exist");
	}
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_req2SetState::",state.c_str()," =",value.c_str(),"-->>OUT");
	return;
}

void SM_regReply4Req(SM_handleReqSet callback)
{
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_regReply4Req-->>IN");
		
	if(callback != NULL)
	{
		//setReqCallback = callback;
		AppManager::GetInstance()->statMgr_SMReqSet(callback);
	}
	else
	{
		logError(SDK_AMGR,"AppSM.cpp::SM_regReply4Req::callback is NULL!");
	}
	
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_regReply4Req-->>OUT");
	return;
}

void SM_updateState(uint16_t ampid_req, std::string state,std::string value)
{
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_updateState::ampid_req::",ampid_req,state," =",value,"-->>IN");
	::v0_1::org::neusoft::AppMgr::key_value::iterator it = app_key_value.find(state);
	if (it != app_key_value.end())
	{
		app_key_value[state].setAmpid_req(ampid_req);
		app_key_value[state].setValue(value);
		AppManager::GetInstance()->statMgr_StateNotify(ampid_req,state,value);
		logInfo(SDK_AMGR,"AppSM.cpp::SM_updateState::",state," =",app_key_value[state].getValue());
	}
	else
	{
		logError(SDK_AMGR,"AppSM.cpp::SM_updateState::",state,"is not exist");
	}
	
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_updateState::ampid_req::",ampid_req,state," =",value,"-->>OUT");
	return;
}

void SM_regCb4StateChanged(SM_handleStateChange callback)
{
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_regCb4StateChanged-->>IN");
		
	if(callback != NULL)
	{
		//stateChangeCallback = callback;
		AppManager::GetInstance()->statMgr_SMChangedNotify(callback);
	}
	else
	{
		logError(SDK_AMGR,"AppSM.cpp::SM_regCb4StateChanged::callback is NULL!");
	}
	
	logVerbose(SDK_AMGR,"AppSM.cpp::SM_regCb4StateChanged-->>OUT");
	return;
}


