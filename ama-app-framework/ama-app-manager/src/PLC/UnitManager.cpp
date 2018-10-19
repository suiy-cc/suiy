/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <COMMON/UniqueID.h>
#include "Dependency.h"
#include "UnitManager.h"
#include "PLCTask.h"
#include "ProcessInfo.h"
#include "LastUserCtx.h"
#include "NotifyWaiter.h"
#include "TaskDispatcher.h"
#include "AudioTask.h"
#include "ID.h"
#include "AppAwakeMgr.h"
#include "ReadXmlConfig.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

static void pllc_startCb(bool result, uint32_t pid, void *userData);
static void pllc_stopCb(bool result, void *userData);
static void pllc_unitChangeCb(const char *unitName , bool isRuning);

UnitManager::UnitManager()
: isInitialized(false)
{
    //we should created all unitfiles when unitManager initialized.
    //mybe not nessary let's find
    //    if(llcErr == LLC_RESULT_SUCCESS)
    //    {
    //        logInfo(PLC,"UnitManager call LLC_Init  SUCCESS!");

    //        for (int i = 0; unitFullPath[i]!=NULL ;i++)
    //        {
    //            llcErr = LLC_CreateUnit(unitFullPath[i]);
    //            if(llcErr != LLC_RESULT_SUCCESS)
    //            {
    //                logError(PLC,"LLC_CreateUnit ",unitFullPath[i] ," failed.  error code : ", llcErr);
    //            }
    //        }
    //        logInfo(PLC,"UnitManager create all existing unitfile finish!");
    //    }
    //    else
    //    {
    //       logError(PLC,"libLifeCycle initial failed. all applications would not working!");
    //    }
    if(!isInitialized)
        init();
}

UnitManager::~UnitManager()
{
    if(isInitialized)
        destroy();
}

void UnitManager::init()
{
    DomainVerboseLog chatter(PLC, "UnitManager::init()");

    if(isInitialized)
    {
        logWarn(PLC, "UnitManager::init(): already initialized.");
        return;
    }

    string succeed = "succeeded";
    string fail = "failed";

    // initialize LLC connection
    logInfo(PLC, LOG_HEAD, "initializing LLC...");
    LLC_Result llcErr = LLC_Init();
    string outcome = (llcErr==LLC_RESULT_SUCCESS?succeed:fail);
    logInfo(PLC, LOG_HEAD, "LLC's initialization is ", outcome);
    if(llcErr!=LLC_RESULT_SUCCESS)
        logError(PLC, "UnitManager::init(): init failed. error code: ", llcErr);
    isInitialized = true;

    // listen to unit status' modification
    llcErr = LLC_ListenUnitStatus(pllc_unitChangeCb);
    outcome = (llcErr==LLC_RESULT_SUCCESS?succeed:fail);
    logInfo(PLC,"UnitManager::init(): LLC_ListenUnitStatus() ", outcome);
}

void UnitManager::destroy()
{
    LLC_Result result = LLC_Destroy();
    isInitialized = false;
}

bool UnitManager::StartUnit(string unitName, const char **argv, void *userData)
{
    string::size_type index = unitName.find(".service");
    if(string::npos == index){
        logWarn(PLC,"can't find .service string in unitName,so unitName is illegal");
        return false;
    }

    LLC_Result result = LLC_StartUnit(unitName.c_str(), argv, pllc_startCb, userData);
    if (result!=LLC_RESULT_SUCCESS)
    {
        logError(PLC,"libLifeCycle LLC_StartUnit failed. error code : ", result);
    }

    return result==LLC_RESULT_SUCCESS;
}

std::string UnitManager::StartUnit(unsigned int ampid, const char **argv)
{
    DomainVerboseLog chatter(PLC, formatText("UnitManager::startUnit(%#x)", ampid));

    std::string unitName = GetUnitName(ampid);

    if(!unitName.empty())
    {
        logInfo(PLC, "UnitManager::startUnit(): boot command: ", unitName);
        unsigned int *userData = new unsigned int;
        *userData = ampid;
        if(!StartUnit(unitName, argv, userData))
            unitName.clear();

        std::string keyApp;
        GetConfigFromString("KeyApp", keyApp);
        if(ampid==AppList::GetInstance()->GetAMPID(keyApp))
            logInfo(PROF, "UnitManager::startUnit(): start app: ", ToHEX(ampid), " is sent to systemd.");
    }
    else
        logError(PLC, "UnitManager::startUnit(): failed getting UnitName!");

    return unitName;
}

void UnitManager::StopUnit(AMPID proc)
{
    DomainVerboseLog chatter(PLC, formatText("UnitManager::stopUnit(%#x)", proc));

    auto unitName = GetUnitName(proc);

    if(unitName.empty())
        logError(PLC, "UnitManager::StopUnit(): failed getting UnitName!");
    else
        StopUnit(unitName);
}

typedef std::tuple<unsigned long long, std::string, unsigned int> StopUnitInfo;

void UnitManager::StopUnit(const string &unit)
{
    DomainVerboseLog chatter(PLC, formatText("UnitManager::stopUnit(%s)", unit.c_str()));

    string::size_type index = unit.find(".service");
    if(string::npos == index){
        logWarn(PLC,"can't find .service string in unitName,so unitName is illegal");
        return;
    }

    // register waiter ID
    unsigned long long waiterID = GetUniqueID();
    RegisterWaiterID(waiterID);

    // stop specified unit
    StopUnitInfo* userData = new StopUnitInfo(waiterID, unit, GetAMPID(unit));
    LLC_Result result = LLC_StopUnit(unit.c_str(), pllc_stopCb, userData);
    if(result!=LLC_RESULT_SUCCESS)
    {
        UnregisterWaiterID(waiterID);
        logError(PLC, "LLC_StopUnit() failed. error code: ", result);
        return;
    }

    // wait for stopCB to be called
    Wait(waiterID);

    // un-register waiter ID
    UnregisterWaiterID(waiterID);
}

bool UnitManager::GetUnitState(const std::string& unitName)
{
    DomainVerboseLog chatter(PLC, formatText("UnitManager::GetUnitState(%s)", unitName));

    bool status;

    LLC_Result result=LLC_GetUnitStatus (unitName.c_str(), &status);
    if (result!=LLC_RESULT_SUCCESS)
    {
        logError(PLC,"libLifeCycle getUnitState failed. error code : ", result);
    }

    return status;
}

bool UnitManager::GetUnitState(unsigned int proc)
{
    DomainVerboseLog chatter(PLC, formatText("UnitManager::GetUnitState(%#x)", proc));

    auto unitName = GetUnitName(proc);

    if(unitName.empty())
    {
        logError(PLC, "UnitManager::startUnit(): failed getting UnitName!");
        return false;
    }
    else
        return GetUnitState(unitName);
}

LLC_Result UnitManager::listenUnitState()
{
    LLC_Result result=LLC_ListenUnitStatus (pllc_unitChangeCb);
    if (result!=LLC_RESULT_SUCCESS)
    {
        logError(PLC,"libLifeCycle getUnitState failed. error code : ", result);
    }
    return result;
}

AMPID UnitManager::GetAMPID(const string& unitName)
{
    auto pos = unitName.rfind("AMPID");
    if(pos != std::string::npos)
    {
        auto tailPos = pos+10;
        if(unitName.size() > tailPos)
        {
            std::string ampid = unitName.substr(pos+5, 6);
            try{
                return std::stoul(ampid, 0, 0);
            }
            catch(const std::exception& e)
            {
                return INVALID_AMPID;
            }
        }
    }

    return 0;
}

string UnitManager::GetUnitName(unsigned int proc)
{
    // prepare command line option
    std::string option = "AMPID"+ToHEX(proc);

    // get system-d prefix
    std::string systemDPrefix;
    bool isValid = AppList::GetInstance()->IsValidApp(proc);
    if(isValid)
    {
        logInfo(PLC, "UnitManager::startUnit(): found in AppList");
        std::string systemDFileName = AppList::GetInstance()->GetSystemD(proc);
        unsigned long pos = systemDFileName.rfind('.');
        if(pos==std::string::npos)
        {
            systemDPrefix = systemDFileName;
        }
        else
        {
            systemDPrefix = systemDFileName.substr(0,pos);
        }
    }
    else
    {
        logWarn(PLC, "UnitManager::startUnit(): not found in AppList");
        switch(GET_APPID(proc))
        {
            default:
                logError(PLC, ToHEX(proc), " not defined in UnitManager::startUnit");
                break;
        }
    }

    if(systemDPrefix.empty())
        return "";
    else
        return systemDPrefix + option + ".service";
}

UnitManager* UnitManager::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static UnitManager m_Instance;
    return &m_Instance;
}


///////////////////////callback function///////////////////////////

static void  pllc_startCb(bool success, uint32_t pid, void *userData)
{
    AMPID ampid = INVALID_AMPID;
    if(userData)
        ampid = *static_cast<unsigned int*>(userData);

    DomainVerboseLog chatter(PLC, formatText("pllc_startCb(%s, %d, %#x)", success?"true":"false", pid, ampid));

    if(ampid!=INVALID_AMPID)
    {
        if (success)
        {
            logInfo(PLC, "start app ", ToHEX(ampid), " succeeded!");
            auto plcTask = std::make_shared<PLCTask>();
            plcTask->SetAMPID(ampid);
            plcTask->SetType(PLC_NOTIFY_START_OK);
            TaskDispatcher::GetInstance()->Dispatch(plcTask);
        }
        else
        {
            logError(PLC, "start app ", ToHEX(ampid), " failed!");
            auto plcTask = std::make_shared<PLCTask>();
            plcTask->SetAMPID(ampid);
            plcTask->SetType(PLC_NOTIFY_START_NG);
            TaskDispatcher::GetInstance()->Dispatch(plcTask);
        }
        delete static_cast<unsigned int*>(userData);
    }
    else
    {
        logInfo(PLC, "pllc_startCb(): unrecognised unit(pid=", pid, ") is started.");
    }
}

//typedef void(* LLC_StopCb)(_Bool result, void *userData)
static void pllc_stopCb(bool result, void *userData)
{
    DomainVerboseLog chatter(PLC, "pllc_stopCb()");

    StopUnitInfo stopUnitInfo;
    if(userData)
        stopUnitInfo = *static_cast<StopUnitInfo*>(userData);
    else
        return;

    delete static_cast<StopUnitInfo*>(userData);

    // get ampid
    auto app = std::get<2>(stopUnitInfo);

    // get unit name
    auto unitName = std::get<1>(stopUnitInfo);

    // get waiterID
    auto waiterID = std::get<0>(stopUnitInfo);

    // notify StopUnit that specified unit is stopped
    Notify(waiterID);
    logInfo(PLC, "pllc_stopCb(): waiter ID = ", ToHEX(waiterID));

    // send app stop notify
    auto plcTask = std::make_shared<PLCTask>();
    plcTask->SetAMPID(app);
    plcTask->SetUserData(reinterpret_cast<unsigned long long>(new std::string(unitName))
                         , [](unsigned long long userData){delete reinterpret_cast<std::string*>(userData);});
    if(result)
    {
        logInfo(PLC, "succeeded in stopping app ", unitName);
        plcTask->SetType(PLC_NOTIFY_STOP_OK);
    }
    else
    {
        logError(PLC, "failed in stopping app ", unitName);
        plcTask->SetType(PLC_NOTIFY_STOP_NG);
    }
    TaskDispatcher::GetInstance()->Dispatch(plcTask);

    auto audioTask = std::make_shared<AudioTask>();
    audioTask->SetType(CLEAR_APP_AUDIO);
    audioTask->setAMPID(app);
    TaskDispatcher::GetInstance()->Dispatch(audioTask);

    AppAwakeMgr::GetInstance()->clearAwakeStateWhenAppCrash(app);
}

static void pllc_unitChangeCb(const char *unitName , bool isRunning)
{
    // make sure unit name is valid
    if(!unitName)
        return;

    DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%s)", unitName));


    logInfo(PLC, "pllc_unitChangeCb(): unit \"", std::string(unitName), "\" change its status to ", isRunning);
    //unit " power_manager.service " change its status to false
    //unit " nodestatemanager-daemon.service " change its status to false
    UnitStateChange(unitName, isRunning);

    UnitManager::GetInstance()->unitStateChangeBroadcaster.NotifyAll([&unitName, isRunning](std::function<void(const std::string&, bool)> callback){
        DomainVerboseLog chatter(PLC, formatText(__STR_FUNCTION__+"(%s)", unitName));

        callback(unitName, isRunning);
    });
}
