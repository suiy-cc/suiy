/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppIME.h"
#include <unistd.h>
#include <string>
#include <v0_1/org/neusoft/AppMgrProxy.hpp>
#include <CommonAPI/CommonAPI.hpp>
#include <v0_1/org/neusoft/ImeIfProxy.hpp>
#include "AppManager.h"
#include "log.h"
#include "CAPINotifyEnum.h"
#include <LogContext.h>

using namespace std;
using v0_1::org::neusoft::ImeIf;

extern std::shared_ptr<v0_1::org::neusoft::AppMgrProxy<>> appManager;
extern unsigned int GetAMPID();
std::shared_ptr<v0_1::org::neusoft::ImeIfProxy<>> gImeIfProxyPtr;

static uint32_t sImeFocusAmpid = 0;
static ImeInputCallback sImeInputCallbackFunc = NULL;
static ImeInputCallback sImeInputBuffCallbackFunc = NULL;
static ImeClosedCallback sImeClosedCallbackFunc = NULL;

static bool sImeVisible = false;
static bool sIsImeInit = false;


static void imeFocusAmpidCallback(uint32_t ampid)
{
    unsigned int selfAmpid = GetAMPID();
    int selfSeat = GET_SEAT(selfAmpid);
    int seat = GET_SEAT(ampid);
    logDebug(SDK_AMGR, "IME Self  Seat : ", selfSeat);
    logDebug(SDK_AMGR, "IME App   Seat : ", seat);
    logDebug(SDK_AMGR, "IME App   Seat : ", seat == selfSeat);
    logDebug(SDK_AMGR, "Focus AMPID : ", ampid);
    if (selfSeat == seat) {
        logInfo(SDK_AMGR, "IME Set Focus Ampid, ", ampid);
        sImeFocusAmpid = ampid;
    }
}

static inline bool imeIsSelfFocus()
{
    unsigned int selfAmpid = GetAMPID();
    return selfAmpid == sImeFocusAmpid;
}

static void imeClosedEventCallback(uint32_t ampid)
{
    logDebug(SDK_AMGR, "imeClosedEventCallback IN");
    uint32_t selfAmpid = GetAMPID();
    logDebug(SDK_AMGR, "IME Self  AMPID : ", selfAmpid);
    logDebug(SDK_AMGR, "IME focus AMPID : ", sImeFocusAmpid);
    if (selfAmpid == sImeFocusAmpid) {
            sImeVisible = false;
            if (sImeClosedCallbackFunc) {
                sImeClosedCallbackFunc();
                logInfo(SDK_AMGR, "IME closed");
            }
    }
    logDebug(SDK_AMGR, "imeClosedEventCallback OUT");
}

static void imeReceiveMsgCallback(ImeIf::ImeMsg imsg, uint32_t msgType)
{
    logDebug(SDK_AMGR, "IME Message : ", imsg.getMessage());
    uint32_t selfAmpid = GetAMPID();
    logDebug(SDK_AMGR, "IME Self  AMPID : ", selfAmpid);
    logDebug(SDK_AMGR, "IME focus AMPID : ", sImeFocusAmpid);
    if (selfAmpid == sImeFocusAmpid) {
            if (sImeInputCallbackFunc) {
                sImeInputCallbackFunc(imsg.getMessage(), (EMsgType)msgType);
                logInfo(SDK_AMGR, "Selected IME Message : ", imsg.getMessage());
                logInfo(SDK_AMGR, "Selected IME Type : ", msgType);
            }
    }
}

// static void imeReceiveBuffMsgCallback(ImeIf::ImeMsg imsg)
// {
//     logInfo(DT_APP_MGR, "IME Buff Message : ", imsg.getMessage());
//     uint32_t selfAmpid = GetAMPID();
//     if (selfAmpid == sImeFocusAmpid) {
//         if (sImeInputBuffCallbackFunc) {
//             sImeInputCallbackFunc(imsg.getMessage());
//             logInfo(DT_APP_MGR, "Selected IME Buff Message : ", imsg.getMessage());
//         }
//     }
// }

void imeInit()
{
    logVerbose(SDK_AMGR, "IME Init IN");

    if(!sIsImeInit){
        CommonAPI::Runtime::setProperty("LibraryBase", "ImeIf");
        unsigned int ampid = GetAMPID();
        int seat = GET_SEAT(ampid);
        std::string domain;
        std::string instance;
        std::string connection;
        if(seat == E_SEAT_IVI){
            domain = "local";
            instance = "com.hs7.ime.scr1";
            connection = "";
        }else if(seat == E_SEAT_RSE1){
            domain = "local";
            instance = "com.hs7.ime.scr2";
            std::string connection = "";
        }else if(seat == E_SEAT_RSE2){
            domain = "local";
            instance = "com.hs7.ime.scr3";
            connection = "";
        }else{
            logWarn(SDK_AMGR,"service can not use IME!");
            return;
        }
        shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
        gImeIfProxyPtr = runtime->buildProxy<::v0_1::org::neusoft::ImeIfProxy>(domain,instance, connection);
        logDebug(SDK_AMGR, "IME Init Tag1");

        int tryCnt = 0;
        while (!gImeIfProxyPtr->isAvailable()){
            usleep(100);
            tryCnt++;
            if(tryCnt>10){
                logWarn(SDK_AMGR,"IME commonAPI connection failed first time!!!");
                return;
            }
        }
        logDebug(SDK_AMGR, "IME Init Tag2");
        // broadcast CS --focus_ampid--> APP
        AppManager::GetInstance()->SetIMEFocusChangedCB(imeFocusAmpidCallback);
        // broadcast CS --focus_ampid--> APP
        AppManager::GetInstance()->SetIMEClosedCB(imeClosedEventCallback);
        // broadcast IME --iuput_messge--> APP
        gImeIfProxyPtr->getInputNotifyEvent().subscribe(imeReceiveMsgCallback);
        //gImeIfProxyPtr->getInputBuffNotifyEvent().subscribe(imeReceiveBuffMsgCallback);
        sIsImeInit = true;
    }else{
        logDebug(SDK_AMGR,"IME already init");
    }

    logVerbose(SDK_AMGR, "IME Init OUT");
}

//this function has diferent try time with imeInit()
void imeInitAgain(){
    logVerbose(SDK_AMGR, "IME Init IN");
    //CommonAPI::Runtime::setProperty("LogContext", "APLC");
    CommonAPI::Runtime::setProperty("LibraryBase", "ImeIf");
    unsigned int ampid = GetAMPID();
    int seat = GET_SEAT(ampid);
    std::string domain;
    std::string instance;
    std::string connection;
    if(seat == E_SEAT_IVI){
        domain = "local";
        instance = "com.hs7.ime.scr1";
        connection = "";
    }else if(seat == E_SEAT_RSE1){
        domain = "local";
        instance = "com.hs7.ime.scr2";
        std::string connection = "";
    }else if(seat == E_SEAT_RSE2){
        domain = "local";
        instance = "com.hs7.ime.scr3";
        connection = "";
    }else{
        logError(SDK_AMGR,"service can not use IME!");\
        return;
    }
    shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    gImeIfProxyPtr = runtime->buildProxy<::v0_1::org::neusoft::ImeIfProxy>(domain,instance, connection);

    int tryCnt = 0;
    while (!gImeIfProxyPtr->isAvailable()){
        usleep(100);
        tryCnt++;
        if(tryCnt>3000){
            logError(SDK_AMGR,"IME commonAPI connection failed!!!");
            return;
        }
    }
    logInfoF(SDK_AMGR,"IME CommonAPI service Available! spend %d us \n",(tryCnt*100));
    // broadcast CS --focus_ampid--> APP
    AppManager::GetInstance()->SetIMEFocusChangedCB(imeFocusAmpidCallback);
    // broadcast CS --focus_ampid--> APP
    AppManager::GetInstance()->SetIMEClosedCB(imeClosedEventCallback);
    // broadcast IME --iuput_messge--> APP
    gImeIfProxyPtr->getInputNotifyEvent().subscribe(imeReceiveMsgCallback);
    //gImeIfProxyPtr->getInputBuffNotifyEvent().subscribe(imeReceiveBuffMsgCallback);
    sIsImeInit = true;
    logVerbose(SDK_AMGR, "IME Init OUT");
}

void imeOpenIME(IMEContext icxt)
{
    if (sImeVisible)
        return ;

    if (!sIsImeInit) {
        imeInitAgain();
    }

    logDebug(SDK_AMGR, "IME OPEN IN");
    CommonAPI::CallStatus callStatus;
    uint32_t ampid = GetAMPID();
    logDebug(SDK_AMGR, "IME OPEN AMPID : ", ampid);
    gImeIfProxyPtr->sendIMEContext((uint32_t)icxt.KeyboardType, (uint32_t)icxt.LangType, icxt.BuffString, callStatus);
    logDebug(SDK_AMGR, "IME OPEN Send CTX");
    appManager->openIME(ampid, icxt.KeyboardType, icxt.LangType, icxt.BuffString, callStatus);
    sImeVisible = true;
    logDebug(SDK_AMGR, "IME OPEN OUT");
}

void imeRegisterInputEvent(ImeInputCallback icb)
{
    sImeInputCallbackFunc = icb;
}

// void imeRegisterInputBuffEvent(ImeInputCallback icb)
// {
//     sImeInputBuffCallbackFunc = icb;
// }

void imeCloseIME()
{
    if (!sImeVisible)
        return ;

    if (!sIsImeInit) {
        imeInitAgain();
    }

    logDebug(SDK_AMGR, "IME Close IN");
    CommonAPI::CallStatus callStatus;
    uint32_t ampid = GetAMPID();
    logInfo(SDK_AMGR, "IME Close AMPID :", ampid);
    appManager->closeIME(ampid, callStatus);
    sImeVisible = false;
    logDebug(SDK_AMGR, "IME Close OUT");
}

uint32_t imeGetFocusAmpid()
{
    return sImeFocusAmpid;
}

void imeRegisterClosedEvent(ImeClosedCallback ccb)
{
    sImeClosedCallbackFunc = ccb;
}

void imeSendEvent(int eventType, std::string param)
{
    if (!sIsImeInit) {
        imeInitAgain();
    }

    if (imeIsSelfFocus()) {
        logInfoF(SDK_AMGR, "IME Send Event ( %d, %s)", eventType, param.c_str());
        CommonAPI::CallStatus callStatus;
        gImeIfProxyPtr->sendEvent(eventType, param, callStatus);
    }
}
