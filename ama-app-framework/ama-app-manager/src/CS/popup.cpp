/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <algorithm>

#include "popup.h"
#include "log.h"
#include "Capi.h"
#include "PLCTask.h"
#include "ama_types.h"
#include "TaskDispatcher.h"
#include "ID.h"
extern LOG_DECLARE_CONTEXT(CS);

bool operator<(const PopInfo & pi1, const PopInfo & pi2)
{
	return pi1.Priority < pi2.Priority;
}

PopupManager gPopupMgr;

PopupManager::PopupManager() : popupId_(0)
{

}

void PopupManager::putPopInfo(uint32_t popId, const PopInfo & pi)
{
	logDebug(CS, "putPopInfo IN");
	logDebug(CS, "PopID : ", popId);
	logDebug(CS, "PopType : ", pi.Pt);
	logDebug(CS, "PopValue : ", pi.Value);
	logDebug(CS, "PopScreen : ", pi.ScreenIndex);

	if (/*E_SEAT_IVI == GET_SEAT(pi.Ampid)*/pi.ScreenIndex == 0) {
		logDebug(CS, "PopupManager::putPopInfo Pop-ivi");
		iviPopInfoList_.push_back(make_pair(popId, pi));
		for (auto & v : iviPopInfoList_) {
			logDebug(CS, "IVI PopID : ", v.first);
			logDebug(CS, "IVI PopInfo : ", v.second.Value);
		}
	} else if (/*E_SEAT_RSE1 == GET_SEAT(pi.Ampid)*/pi.ScreenIndex == 1) {
		logDebug(CS, "PopupManager::putPopInfo Pop-rse1");
		rse1PopInfoList_.push_back(make_pair(popId, pi));
		for (auto & v : rse1PopInfoList_) {
			logDebug(CS, "RSE1 PopID : ", v.first);
			logDebug(CS, "RSE1 PopInfo : ", v.second.Value);
		}
	} else if (/*E_SEAT_RSE2 == GET_SEAT(pi.Ampid)*/pi.ScreenIndex == 2) {
		logDebug(CS, "PopupManager::putPopInfo Pop-rse2");
		rse2PopInfoList_.push_back(make_pair(popId, pi));
		for (auto & v : rse2PopInfoList_) {
			logDebug(CS, "RSE2 PopID : ", v.first);
			logDebug(CS, "RSE2 PopInfo : ", v.second.Value);
		}
	}

	handlePopupCtrl(pi.ScreenIndex);
	// test end
	logDebug(CS, "putPopInfo OUT");
}

std::vector<PopInfo> PopupManager::getPopInfoList()
{
	// T.B.D
	std::vector<PopInfo> vp(iviPopInfoList_.size());
	for (auto & pi : iviPopInfoList_) {
		vp.push_back(pi.second);
	}
	return vp;
}

void PopupManager::removePopInfo(uint32_t popId)
{
	logDebug(CS, "removePopInfo IN");
	removePopInfoInter(popId, iviPopInfoList_, 0);   // process ivi popup
	removePopInfoInter(popId, rse1PopInfoList_, 1);  // process rse1 popup
	removePopInfoInter(popId, rse2PopInfoList_, 2);  // process rse2 popup
	logDebug(CS, "removePopInfo OUT");
}

bool PopupManager::removePopInfoInter(uint32_t popId, std::vector<std::pair<uint64_t, PopInfo>> &popInfoList, int srcIdx)
{
	auto retval = std::find_if(begin(popInfoList), end(popInfoList), [=](std::pair<uint64_t, PopInfo> & v){
		if (v.first == popId) {
			return true;
		} else {
			return false;
		}
	});
	if (end(popInfoList) != retval) {
		popInfoList.erase(retval);
		handlePopupCtrl(srcIdx);
		for (auto & v : popInfoList) {
			logDebug(CS, "PopID : ", v.first);
			logDebug(CS, "PopInfo : ", v.second.Value);
		}
		return true;
	} else {
		logDebug(CS, "IVI PopupID not found");
		return false;
	}
}

uint32_t PopupManager::getPopupId()
{
	return popupId_++;
}

void PopupManager::handlePopupCtrl(uint32_t scrIdx)
{
	switch (scrIdx) {
		case 0:
		{
			handlePopupIter(iviPopInfoList_, E_SEAT_IVI);
			break;
		}
		case 1:
		{
			handlePopupIter(rse1PopInfoList_, E_SEAT_RSE1);
			break;
		}
		case 2:
		{
			handlePopupIter(rse2PopInfoList_, E_SEAT_RSE2);
			break;
		}
	}

}

void PopupManager::handlePopupIter(std::vector<std::pair<uint64_t, PopInfo>> &pil, E_SEAT seat)
{
	if (!pil.empty()) {
		popupUpdateInfoList(pil);
		// show popup
		logDebug(CS, "Popup SHOW");
		auto plcTask = make_shared<PLCTask>();
		plcTask->SetType(PLC_CMD_SHOW);
		plcTask->SetAMPID(AMPID_SERVICE(seat, E_APPID_POPUP));
		TaskDispatcher::GetInstance()->Dispatch(plcTask);
	} else {
		// hide App
		logDebug(CS, "Popup HIDE");
		auto plcTask = make_shared<PLCTask>();
		plcTask->SetType(PLC_CMD_HIDE);
		plcTask->SetAMPID(AMPID_SERVICE(seat, E_APPID_POPUP));
		TaskDispatcher::GetInstance()->Dispatch(plcTask);
	}
}

void PopupManager::updatePopupList()
{

}

void PopupManager::processPopupRespond(uint32_t popId, uint32_t reason)
{
	// TODO: only remove popinfo
	logDebug(CS, "Process Popup Respond ID: ", popId);
	logDebug(CS, "Process Popup Respond reason:", reason);
	ShowPopupNotify(popId, reason);
	removePopInfo(popId);
}

void PopupManager::processPopupRespond(uint32_t popId, uint32_t reason, std::string message)
{
	// TODO: only remove popinfo
	logDebug(CS, "Process Popup Respond ID: ", popId);
	logDebug(CS, "Process Popup Respond reason:", reason);
	logDebug(CS, "Process Popup Respond message:", message);
	ShowPopupExNotify(popId, reason, message);
	removePopInfo(popId);
}
