/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "AppCS.h"
#include "AppManager.h"
#include <v0_1/org/neusoft/AppMgrProxy.hpp>
#include <stdint.h>
#include <CommonAPI/CommonAPI.hpp>
#include "AppPLC.h"
#include "log.h"
#include <nlohmann/json.hpp>

extern std::shared_ptr<v0_1::org::neusoft::AppMgrProxy<>> appManager;

static std::vector<std::pair<PopupID, PopupCallbackFun>> gRegisterInfo;
static std::vector<std::pair<PopupID, PopupCallbackFunEx>> gRegisterExInfo;


extern LogContext SDK_AMGR;

static void unregisterClickEvent(PopupID id);
static void unregisterClickExEvent(PopupID id);

static void psNotifyFunc(const uint32_t & id, const uint32_t & reason)
{
	logDebug(SDK_AMGR, "POPSDK psNotifyFunc IN id: %d reason %d", id, reason);
    logInfo(SDK_AMGR, "POPSDK RegInfo Size : ", gRegisterInfo.size());
	for (auto & ri : gRegisterInfo) {
		if (ri.first == id) {
			ri.second(id, reason);
			logInfoF(SDK_AMGR, "POPSDK CB Enter %d", id);
			unregisterClickEvent(id);
		}
		logInfoF(SDK_AMGR, "POPSDK RI : %d %d", ri.first, ri.second);
	}
	logDebug(SDK_AMGR, "POPSDK psNotifyFunc OUT");
}

static void psNotifyExFunc(const uint32_t & id, const uint32_t & reason, const std::string& message)
{
	logDebug(SDK_AMGR, "POPSDK psNotifyExFunc IN id: %d reason %d", id, reason);
    logInfo(SDK_AMGR, "POPSDK RegInfo Size : ", gRegisterExInfo.size());
	for (auto & ri : gRegisterExInfo) {
		if (ri.first == id) {
			ri.second(id, reason, message);
			logInfoF(SDK_AMGR, "POPSDK CB Enter %d", id);
			unregisterClickExEvent(id);
		}
		logInfoF(SDK_AMGR, "POPSDK RI : %d %d", ri.first, ri.second);
	}
	logDebug(SDK_AMGR, "POPSDK psNotifyExFunc OUT");
}

void psInitPopup()
{
	static bool isInitialized = false;
	if (!isInitialized) {
		logDebug(SDK_AMGR, "POPSDK INIT IN");
		appManager->getNotifyPopupEvent().subscribe(psNotifyFunc);
		appManager->getNotifyPopupExEvent().subscribe(psNotifyExFunc);
		logDebug(SDK_AMGR, "POPSDK INIT OUT");
		isInitialized = true;
	}
}

PopupID psRequestPopup(PopupInfo pi)
{
	logInfo(SDK_AMGR, "POPSDK REQ IN");
	logDebug(SDK_AMGR, "POPSDK PopType : ", pi.Pt);
	logDebug(SDK_AMGR, "POPSDK PopValue : ", pi.Value);
	logDebug(SDK_AMGR, "POPSDK PopPrior : ", pi.Prior);
	logDebug(SDK_AMGR, "POPSDK PopScreen : ", pi.ScreenIndex);
	logDebug(SDK_AMGR, "POPSDK PopupInfo size : ", sizeof(PopupInfo));
	CommonAPI::CallStatus callStatus;
	PopupID id;
	uint32_t ampid = GetAMPID();
 	using json = nlohmann::json;
	json j;
	j["prior"] = pi.Prior;
	j["Msg"]   = pi.Value;
	std::string value = j.dump();
	appManager->sendPopupInfo(ampid, pi.Pt, /*pi.Value*/value, pi.ScreenIndex, callStatus, id);
	logDebug(SDK_AMGR, "POPSDK REQ OUT");
	return id;
}

void psRegisterClickEvent(PopupID id, PopupCallbackFun pcf)
{
	gRegisterInfo.push_back(std::make_pair(id, pcf));
	logInfoF(SDK_AMGR, " PopID : %d,  PCF %d, RI %d", id, pcf, gRegisterInfo.size());
}

void psRegisterExClickEvent(PopupID id, PopupCallbackFunEx pcf)
{
	gRegisterExInfo.push_back(std::make_pair(id, pcf));
	logInfoF(SDK_AMGR, " PopID : %d,  PCF %d, RI %d", id, pcf, gRegisterExInfo.size());
}

void psShowPopup(PopupID id)
{
	CommonAPI::CallStatus callStatus;
	bool retval;
	appManager->showPopup(id, callStatus, retval);
}

void psClosePopup(PopupID id)
{
	CommonAPI::CallStatus callStatus;
	bool retval;
	appManager->closePopup(id, callStatus, retval);
	unregisterClickEvent(id);
}

void SetCurrentSource(const std::vector<IntentInformation>& lastSourceSet)
{
	AppManager::GetInstance()->SetCurrentSource(lastSourceSet);
}

std::vector<IntentInformation> GetCurrentSource()
{
	return AppManager::GetInstance()->GetCurrentSource();
}

std::vector<IntentInformation> GetLastSource()
{
    return AppManager::GetInstance()->GetLastSource();
}

static void unregisterClickEvent(PopupID id)
{
	logDebug(SDK_AMGR, "unregisterClickEvent IN");
	for (auto it = gRegisterInfo.begin(); it != gRegisterInfo.end(); ) {
		if (it->first == id) {
			it = gRegisterInfo.erase(it);
			logInfoF(SDK_AMGR, "unregisterClickEvent ID : %d", id);
		} else {
			++it;
		}
	} // for
	logDebug(SDK_AMGR, "unregisterClickEvent OUT");
}


static void unregisterClickExEvent(PopupID id)
{
	logDebug(SDK_AMGR, "unregisterClickEvent IN");
	for (auto it = gRegisterExInfo.begin(); it != gRegisterExInfo.end(); ) {
		if (it->first == id) {
			it = gRegisterExInfo.erase(it);
			logInfoF(SDK_AMGR, "unregisterClickEvent ID : %d", id);
		} else {
			++it;
		}
	} // for
	logDebug(SDK_AMGR, "unregisterClickEvent OUT");
}
