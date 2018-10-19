/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <map>

#include "AppList.h"
#include "SMTask.h"
#include "cs_task.h"
#include "SMCtrl.h"
#include "AppState.h"
#include "ProcessInfo.h"
#include "wd.h"
#include "AMGRLogContext.h"
#include "AudioTask.h"
#include "TaskDispatcher.h"
#include "LCDManager.h"
#include "SMList.h"
#include "rpc_proxy.h"
#include "Capi.h"
#include "ReadXmlConfig.h"
#include "DataBase.h"

using namespace std;
using std::make_shared;

extern RpcProxy rp;
SMValueObserver sSMKeyValuePairObserver;

static EOLInfo sm_eolGet()
{
	FUNCTION_SCOPE_LOG_C(SM);

    return rp.getEOLInfo();
}

void sm_eolInit(void)
{
	logVerbose(SM,"SMCtrl.cpp::sm_eolInit-->>IN");
	EOLInfo ei = sm_eolGet();
	#ifdef CODE_FOR_EV
	logInfoF(SM,"SMCtrl.cpp::sm_eolInit CODE_FOR_EV \
		FCW:%d,AEB:%d,BSD:%d,LDW:%d,LKA:%d,DSM:%d,LDWS:%d,TRM:%d,HBA:%d,AP:%d,TSR:%d,LANG:%d,ALC:%d,ALCD:%d,SCM:%d,DSCU:%d,HUD:%d,DVR:%d,EFI:%d,MSMTD:%d,BMFS:%d,ALL:%d,CURISE:%d,TDMC:%d",\
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.BSD,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,(uint8_t)ei.LDWS,\
		(uint8_t)ei.TRM,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TSR,(uint8_t)ei.LANG,(uint8_t)ei.ALC,(uint8_t)ei.ALCD,\
		(uint8_t)ei.SCM,(uint8_t)ei.DSCU,(uint8_t)ei.HUD,(uint8_t)ei.DVR,(uint8_t)ei.EFI,(uint8_t)ei.MSMTD,(uint8_t)ei.BMFS,(uint8_t)ei.ALL,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);
	// BroadcastAMGRSetNotify(E_MODULE_EV_FCW,to_string(ei.FCW));
	// BroadcastAMGRSetNotify(E_MODULE_EV_AEB,to_string(ei.AEB));
	// BroadcastAMGRSetNotify(E_MODULE_EV_BSD,to_string(ei.BSD));
	// BroadcastAMGRSetNotify(E_MODULE_EV_LDW,to_string(ei.LDW));
	// BroadcastAMGRSetNotify(E_MODULE_EV_LKA,to_string(ei.LKA));
	// BroadcastAMGRSetNotify(E_MODULE_EV_DSM,to_string(ei.DSM));
	// BroadcastAMGRSetNotify(E_MODULE_EV_LDWS,to_string(ei.LDWS));
	// BroadcastAMGRSetNotify(E_MODULE_EV_TRM,to_string(ei.TRM));
	// BroadcastAMGRSetNotify(E_MODULE_EV_HBA,to_string(ei.HBA));
	// BroadcastAMGRSetNotify(E_MODULE_EV_AP,to_string(ei.AP));
	// BroadcastAMGRSetNotify(E_MODULE_EV_TSR,to_string(ei.TSR));
	// BroadcastAMGRSetNotify(E_MODULE_EV_LANG,to_string(ei.LANG));
	// BroadcastAMGRSetNotify(E_MODULE_EV_ALC,to_string(ei.ALC));
	// BroadcastAMGRSetNotify(E_MODULE_EV_ALCD,to_string(ei.ALCD));
	// BroadcastAMGRSetNotify(E_MODULE_EV_SCM,to_string(ei.SCM));
	// BroadcastAMGRSetNotify(E_MODULE_EV_DSCU,to_string(ei.DSCU));
	// BroadcastAMGRSetNotify(E_MODULE_EV_HUD,to_string(ei.HUD));
	// BroadcastAMGRSetNotify(E_MODULE_EV_DVR,to_string(ei.DVR));
	// BroadcastAMGRSetNotify(E_MODULE_EV_EFI,to_string(ei.EFI));
	// BroadcastAMGRSetNotify(E_MODULE_EV_MSMTD,to_string(ei.MSMTD));
	// BroadcastAMGRSetNotify(E_MODULE_EV_BMFS,to_string(ei.BMFS));
	// BroadcastAMGRSetNotify(E_MODULE_EV_ALL,to_string(ei.ALL));
	// BroadcastAMGRSetNotify(E_MODULE_EV_CURISE,to_string(ei.CURISE));
	// BroadcastAMGRSetNotify(E_MODULE_EV_TDMC,to_string(ei.TDMC));
	#else
	logInfoF(SM,"SMCtrl.cpp::sm_eolInit CODE_FOR_HS7 \
		fcw:%d aeb:%d mpi:%d ldw:%d lka:%d dsm:%d ri_avs:%d rpe:%d hba:%d ap:%d tbox:%d lang:%d wifi:%d brow:%d \
		DMSCfg:%d DVR:%d AVS:%d CURISE:%d TDMC:%d",\
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.MPI,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,\
		(uint8_t)ei.RI_AVS,(uint8_t)ei.RPE,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TBOX,(uint8_t)ei.LANG,\
		(uint8_t)ei.WIFI,(uint8_t)ei.BROW,(uint8_t)ei.DMSCfg,(uint8_t)ei.DVR,(uint8_t)ei.AVS,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);
	// BroadcastAMGRSetNotify(E_MODULE_FCW,to_string(ei.FCW));
	// BroadcastAMGRSetNotify(E_MODULE_AEB,to_string(ei.AEB));
	// BroadcastAMGRSetNotify(E_MODULE_MoPhCon,to_string(ei.MPI));
	// BroadcastAMGRSetNotify(E_MODULE_LDW,to_string(ei.LDW));
	// BroadcastAMGRSetNotify(E_MODULE_LKA,to_string(ei.LKA));
	// BroadcastAMGRSetNotify(E_MODULE_DSM,to_string(ei.DSM));
	// BroadcastAMGRSetNotify(E_MODULE_RIorAVS,to_string(ei.RI_AVS));
	// BroadcastAMGRSetNotify(E_MODULE_RPE,to_string(ei.RPE));
	// BroadcastAMGRSetNotify(E_MODULE_HBA,to_string(ei.HBA));
	// BroadcastAMGRSetNotify(E_MODULE_AP,to_string(ei.AP));
	// BroadcastAMGRSetNotify(E_MODULE_TBOX,to_string(ei.TBOX));
	// BroadcastAMGRSetNotify(E_MODULE_LANG,to_string(ei.LANG));
	// BroadcastAMGRSetNotify(E_MODULE_WIFI,to_string(ei.WIFI));
	// BroadcastAMGRSetNotify(E_MODULE_BROW,to_string(ei.BROW));
	#endif

	logVerbose(SM,"SMCtrl.cpp::sm_eolInit-->>OUT");

	return;
}

void sm_eolNotify(void)
{
	logVerboseF(SM, "%s::%s-->IN",basename(__FILE__),__FUNCTION__);
	EOLInfo ei = sm_eolGet();
	#ifdef CODE_FOR_EV
	logInfoF(SM,"%s::%s CODE_FOR_EV \
		FCW:%d,AEB:%d,BSD:%d,LDW:%d,LKA:%d,DSM:%d,LDWS:%d,TRM:%d,HBA:%d,AP:%d,TSR:%d,LANG:%d,ALC:%d,ALCD:%d,SCM:%d,DSCU:%d,HUD:%d,DVR:%d,EFI:%d,MSMTD:%d,BMFS:%d,ALL:%d,CURISE:%d,TDMC:%d",\
		basename(__FILE__),__FUNCTION__, \
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.BSD,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,(uint8_t)ei.LDWS,\
		(uint8_t)ei.TRM,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TSR,(uint8_t)ei.LANG,(uint8_t)ei.ALC,(uint8_t)ei.ALCD,\
		(uint8_t)ei.SCM,(uint8_t)ei.DSCU,(uint8_t)ei.HUD,(uint8_t)ei.DVR,(uint8_t)ei.EFI,(uint8_t)ei.MSMTD,(uint8_t)ei.BMFS,(uint8_t)ei.ALL,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);
	BroadcastAMGRSetNotify(E_MODULE_EV_FCW,to_string(ei.FCW));
	BroadcastAMGRSetNotify(E_MODULE_EV_AEB,to_string(ei.AEB));
	BroadcastAMGRSetNotify(E_MODULE_EV_BSD,to_string(ei.BSD));
	BroadcastAMGRSetNotify(E_MODULE_EV_LDW,to_string(ei.LDW));
	BroadcastAMGRSetNotify(E_MODULE_EV_LKA,to_string(ei.LKA));
	BroadcastAMGRSetNotify(E_MODULE_EV_DSM,to_string(ei.DSM));
	BroadcastAMGRSetNotify(E_MODULE_EV_LDWS,to_string(ei.LDWS));
	BroadcastAMGRSetNotify(E_MODULE_EV_TRM,to_string(ei.TRM));
	BroadcastAMGRSetNotify(E_MODULE_EV_HBA,to_string(ei.HBA));
	BroadcastAMGRSetNotify(E_MODULE_EV_AP,to_string(ei.AP));
	BroadcastAMGRSetNotify(E_MODULE_EV_TSR,to_string(ei.TSR));
	BroadcastAMGRSetNotify(E_MODULE_EV_LANG,to_string(ei.LANG));
	BroadcastAMGRSetNotify(E_MODULE_EV_ALC,to_string(ei.ALC));
	BroadcastAMGRSetNotify(E_MODULE_EV_ALCD,to_string(ei.ALCD));
	BroadcastAMGRSetNotify(E_MODULE_EV_SCM,to_string(ei.SCM));
	BroadcastAMGRSetNotify(E_MODULE_EV_DSCU,to_string(ei.DSCU));
	BroadcastAMGRSetNotify(E_MODULE_EV_HUD,to_string(ei.HUD));
	BroadcastAMGRSetNotify(E_MODULE_EV_DVR,to_string(ei.DVR));
	BroadcastAMGRSetNotify(E_MODULE_EV_EFI,to_string(ei.EFI));
	BroadcastAMGRSetNotify(E_MODULE_EV_MSMTD,to_string(ei.MSMTD));
	BroadcastAMGRSetNotify(E_MODULE_EV_BMFS,to_string(ei.BMFS));
	BroadcastAMGRSetNotify(E_MODULE_EV_ALL,to_string(ei.ALL));
	BroadcastAMGRSetNotify(E_MODULE_EV_CURISE,to_string(ei.CURISE));
	BroadcastAMGRSetNotify(E_MODULE_EV_TDMC,to_string(ei.TDMC));
	#else
	logInfoF(SM,"%s::%s CODE_FOR_HS7 \
		fcw:%d aeb:%d mpi:%d ldw:%d lka:%d dsm:%d ri_avs:%d rpe:%d hba:%d ap:%d tbox:%d lang:%d wifi:%d brow:%d \
		DMSCfg:%d DVR:%d AVS:%d CURISE:%d TDMC:%d",\
		basename(__FILE__),__FUNCTION__, \
		(uint8_t)ei.FCW,(uint8_t)ei.AEB,(uint8_t)ei.MPI,(uint8_t)ei.LDW,(uint8_t)ei.LKA,(uint8_t)ei.DSM,\
		(uint8_t)ei.RI_AVS,(uint8_t)ei.RPE,(uint8_t)ei.HBA,(uint8_t)ei.AP,(uint8_t)ei.TBOX,(uint8_t)ei.LANG,\
		(uint8_t)ei.WIFI,(uint8_t)ei.BROW,(uint8_t)ei.DMSCfg,(uint8_t)ei.DVR,(uint8_t)ei.AVS,(uint8_t)ei.CURISE,(uint8_t)ei.TDMC);
	BroadcastAMGRSetNotify(E_MODULE_FCW,to_string(ei.FCW));
	BroadcastAMGRSetNotify(E_MODULE_AEB,to_string(ei.AEB));
	BroadcastAMGRSetNotify(E_MODULE_MoPhCon,to_string(ei.MPI));
	BroadcastAMGRSetNotify(E_MODULE_LDW,to_string(ei.LDW));
	BroadcastAMGRSetNotify(E_MODULE_LKA,to_string(ei.LKA));
	BroadcastAMGRSetNotify(E_MODULE_DSM,to_string(ei.DSM));
	BroadcastAMGRSetNotify(E_MODULE_RIorAVS,to_string(ei.RI_AVS));
	BroadcastAMGRSetNotify(E_MODULE_RPE,to_string(ei.RPE));
	BroadcastAMGRSetNotify(E_MODULE_HBA,to_string(ei.HBA));
	BroadcastAMGRSetNotify(E_MODULE_AP,to_string(ei.AP));
	BroadcastAMGRSetNotify(E_MODULE_TBOX,to_string(ei.TBOX));
	BroadcastAMGRSetNotify(E_MODULE_LANG,to_string(ei.LANG));
	BroadcastAMGRSetNotify(E_MODULE_WIFI,to_string(ei.WIFI));
	BroadcastAMGRSetNotify(E_MODULE_BROW,to_string(ei.BROW));
	BroadcastAMGRSetNotify(E_MODULE_DMSCfg,to_string(ei.DMSCfg));
	BroadcastAMGRSetNotify(E_MODULE_DVR,to_string(ei.DVR));
	BroadcastAMGRSetNotify(E_MODULE_AVS,to_string(ei.AVS));
	BroadcastAMGRSetNotify(E_MODULE_CURISE,to_string(ei.CURISE));
	BroadcastAMGRSetNotify(E_MODULE_TDMC,to_string(ei.TDMC));
	#endif

	logVerboseF(SM, "%s::%s-->OUT",basename(__FILE__),__FUNCTION__);

	return;
}


::v0_1::org::neusoft::AppMgr::value_ampid SMV2VA(const SMValue& value)
{
    ::v0_1::org::neusoft::AppMgr::value_ampid value_ampid;
    value_ampid.setValue(value.value);
    value_ampid.setAmpid_req(value.requester);
    value_ampid.setItemid_reply(value.replyerItemID);

    return value_ampid;
}

::v0_1::org::neusoft::AppMgr::key_value SMVO2KV(const SMValueObserver& kvMap)
{
    ::v0_1::org::neusoft::AppMgr::key_value keyValue;
	auto tmp = kvMap.Copy();
    for(auto& pair: tmp)
    {
        ::v0_1::org::neusoft::AppMgr::value_ampid value_ampid = SMV2VA(pair.second);
        keyValue.insert(std::make_pair(pair.first, value_ampid));
    }

    return keyValue;
}

void initOrgState()
{
    appNoteOperate *date_xml=new appNoteOperate();
    std::pair<::v0_1::org::neusoft::AppMgr::key_value::iterator,bool> map_ret;
    v0_1::org::neusoft::AppMgr::value_ampid __value_ampid;
    std::string app_path = "null";
    std::size_t pos = -1;
    AMPID __ampid = 0;
    std::string xml_file = "null";
    std::string app_name = "null";
    for(std::list<std::string>::iterator it=sAppItemPrefixList.begin();it!=sAppItemPrefixList.end();++it)
    {
        app_name = *it;
        logDebug(SM,"initOrgState() get app_name:",app_name);
        xml_file = "/opt/system/apps/"+app_name+"/state_info.xml";
        logDebug(SM,"initOrgState() get xml_file:",xml_file);
        FILE * fp=fopen(xml_file.c_str(),"r");
        if(NULL==fp){
            logWarn(SM,"initOrgState() xml file::",xml_file,"is not exist!");
            continue;
        }else{
            logDebug(SM,"initOrgState() get state from xml file",xml_file);
            pos = app_name.find("com.hs7.");
            if(std::string::npos != pos){
                app_name.erase(pos, 8);
            }else{
                logError(SM,"initOrgState() app_name",app_name,"can't find 'com.hs7.' from app_name",app_name);
                continue;
            }
            std::list<appNoteStruct> *date_list=new std::list<appNoteStruct>;
            date_xml->ReadXmlFile(xml_file,date_list);
            std::list<appNoteStruct>::iterator iter;
            for(iter = date_list->begin(); iter != date_list->end(); iter++)
            {
                struct appNoteStruct s_XML=*iter;
                __value_ampid.setValue(s_XML.value3);
                __value_ampid.setAmpid_req(0);
                __value_ampid.setItemid_reply(s_XML.value1);

                s_XML.value2=app_name+"_"+s_XML.value2;
                sSMKeyValuePairObserver.AddKey(s_XML.value2, __value_ampid);
            }
            delete(date_list);
            date_list=NULL;
            delete(date_xml);
            date_xml=NULL;
            fclose(fp);
            fp = NULL;
        }
    }
    ::v0_1::org::neusoft::AppMgr::key_value key_value = SMVO2KV(sSMKeyValuePairObserver);
    // update_orgDate(key_value);
    for (::v0_1::org::neusoft::AppMgr::key_value::iterator it=key_value.begin(); it!=key_value.end(); ++it)
    {
        logInfoF(SM,"initOrgState() key:%s value:%s itemid_reply:%s ampid_req:0x%x\n",it->first.c_str(),it->second.getValue().c_str(),it->second.getItemid_reply().c_str(),it->second.getAmpid_req());
    }
    logVerbose(SM,"initOrgState()-->>OUT");
}

void SMInit(void)
{
	logVerbose(SM,"SMCtrl.cpp::SMInit-->>IN");

	sm_eolInit();

    initOrgState();

	logVerbose(SM,"SMCtrl.cpp::SMInit-->>OUT");
}

void SMdeInit(void)
{

}


SMController::SMController()
{
}

SMController::~SMController()
{
}

void SMController::doTask(shared_ptr<Task> task)
{
    assert(task);
    auto smTask = dynamic_pointer_cast<SMTask>(task);

    if(!smTask)
    {
        logWarn(SM, LOG_HEAD, "task object is null!");
        return;
    }

    if (smTask->IsWatchDotTask()) {
      wdSetState(E_TASK_ID_SM, WATCHDOG_SUCCESS);
      return ;
    }

    DomainVerboseLog chatter(SM, formatText(__STR_FUNCTION__+"(%s)", smTask->GetTaskTypeString().c_str()));

    E_SM_TASK taskType = static_cast<E_SM_TASK>(smTask->GetType());
    logInfo(SM, LOG_HEAD, "task type: ", smTask->GetTaskTypeString());
    switch(taskType)
    {
        case E_SM_TASK_INIT:
            SMInit();
            break;
        case E_SM_TASK_QUERY_SNAPSHOT_SM:
            OnQuerySMSnapshot(smTask);
            break;
        case E_SM_TASK_CMD_BROADCAST_SET_REQUEST:
            OnBroadcastSetRequest(smTask);
            break;
        case E_SM_TASK_CMD_BROADCAST_SET_REPLY:
            OnBroadcastSetReply(smTask);
            break;
        case E_SM_TASK_REQUEST_BROADCAST:
            OnRequestBroadcast(smTask);
            break;
        default:
            logWarn(SM, LOG_HEAD, "error task type or ");
    }
}

void SMController::OnQuerySMSnapshot(shared_ptr<SMTask> task)
{
    DomainVerboseLog chatter(SM, __STR_FUNCTIONP__);

    auto replyer = reinterpret_cast<std::function<void(::v0_1::org::neusoft::AppMgr::key_value)>*>(task->GetUserData());

    if(replyer){
        ::v0_1::org::neusoft::AppMgr::key_value key_value = SMVO2KV(sSMKeyValuePairObserver);
        logDebug(SM,"sSMKeyValuePairObserver.size:",sSMKeyValuePairObserver.Size());
        logDebug(SM,"key_value.size:",key_value.size());
        (*replyer)(key_value);
    }else{
        logError(SM,"replyer==NULL!");
    }

	sm_eolNotify();
}

void SMController::OnBroadcastSetRequest(shared_ptr<SMTask> task)
{
    DomainVerboseLog chatter(SM, __STR_FUNCTIONP__);

    auto argTuple = reinterpret_cast<std::tuple<unsigned int, std::string, std::string>*>(task->GetUserData());
    auto requester = std::get<0>(*argTuple);
    auto key = std::get<1>(*argTuple);
    auto value = std::get<2>(*argTuple);

    AMPID ampid_reply = AppList::GetInstance()->GetAMPID(sSMKeyValuePairObserver.GetValue(key).replyerItemID);

    BroadcastSMSetRequestNotify(requester,ampid_reply,key,value);
}

void SMController::OnBroadcastSetReply(shared_ptr<SMTask> task)
{
    DomainVerboseLog chatter(SM, __STR_FUNCTIONP__);

    auto argTuple = reinterpret_cast<std::tuple<unsigned int, std::string, std::string>*>(task->GetUserData());
    auto requester = std::get<0>(*argTuple);
    auto key = std::get<1>(*argTuple);
    auto value = std::get<2>(*argTuple);

    auto oldValue = sSMKeyValuePairObserver.GetValue(key);
    oldValue.value = value;
    oldValue.requester = requester;
    sSMKeyValuePairObserver.ModifyValue(key, oldValue);

    AMPID ampid_reply = AppList::GetInstance()->GetAMPID(sSMKeyValuePairObserver.GetValue(key).replyerItemID);

    BroadcastSMSetDoneNotify(requester,ampid_reply,key,value);
}

void SMController::OnRequestBroadcast(shared_ptr<SMTask> task)
{
    DomainVerboseLog chatter(SM, __STR_FUNCTIONP__);

    auto argTuple = reinterpret_cast<std::tuple<std::string, std::string>*>(task->GetUserData());
    auto event = std::get<0>(*argTuple);
    auto data = std::get<1>(*argTuple);

    BroadcastSMSetDoneNotify(BROADCASTFLAG,BROADCASTFLAG,event,data);
}

void sendInitTaskToSM(void)
{
    DomainVerboseLog chatter(SM, __STR_FUNCTIONP__);
    auto pTask = std::make_shared<SMTask>();
    pTask->SetType(E_SM_TASK_INIT);
    TaskDispatcher::GetInstance()->Dispatch(pTask);
}
