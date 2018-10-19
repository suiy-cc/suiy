/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "ime_mgr.h"
#include <algorithm>
#include <PLC/ProcessInfo.h>
#include <v0_1/org/neusoft/ImeIfProxy.hpp>
#include "Capi.h"
#include "log.h"
#include "PLCTask.h"
#include "TaskDispatcher.h"

using v0_1::org::neusoft::ImeIf;

extern LOG_DECLARE_CONTEXT(CS);

ImeMgr gImeMgr;
std::shared_ptr<v0_1::org::neusoft::ImeIfProxy<>> gImeIfProxyPtr;

ImeMgr::ImeMgr()
{
}

void ImeMgr::createLink(AMPID ampid, uint32_t keyboardType, uint32_t langType, std::string paramStr)
{
	logInfo(CS, "IME createLink IN");
	Link lnk;
	std::lock_guard<std::mutex> lck(mtx_);	// guard links_
	// Fill <Link>.Ctx Memeber
	lnk.Ampid = ampid;
	IMEContext ctx;
	ctx.KeyboardType = (EKeyboardType)keyboardType;
	ctx.LangType     = (ELangType)langType;
	ctx.ParamString  = paramStr;
	lnk.Ctx = ctx;
	// focusAmpid_ = ampid;
	setFocusAmpid(ampid);
	links_.push_back(lnk);
	// resched IME
	schedIME(ampid);
	logInfo(CS, "IME createLink OUT");
}

void ImeMgr::schedIME(uint32_t ampid)
{
	// Resume IME using context
	activateIME(ampid);
	updateAppShowStatus(ampid);
}

void ImeMgr::updateAppShowStatus(AMPID ampid)
{
	//focusAmpid_ = ampid;
	setFocusAmpid(ampid);
	//updateAppsFocusAmpid(focusAmpid_);
	updateAppsFocusAmpid(getFocusAmpid(ampid));
}
// CS --> IME
void ImeMgr::activateIME(uint32_t ampid)
{
	// launch IME
	logInfo(CS, "IME SHOW AMPID : ", ampid);
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appObj = procInfo->FindApp(getFocusAmpid(ampid));
    procInfo->Unlock();
    appObj->StartIME();
}
// CS --> Apps (broadcast)
extern std::shared_ptr<NeusoftAppMgrStubImpl> AppMgr_Service;
void ImeMgr::updateAppsFocusAmpid(AMPID ampid)
{
	logInfo(CS, "IME UPDATE APPs AMPID : ", ampid);
	IMEFocusChangedNotify(ampid);
}

void ImeMgr::closeIME(AMPID ampid)
{
	logInfo(CS, "IME Close AMPID : ", getFocusAmpid(ampid));
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto appObj = procInfo->FindApp(getFocusAmpid(ampid));
    procInfo->Unlock();
	if (NULL == appObj)	// No Focus App
		return ;
    appObj->StopIME();

	IMEClosedNotify(ampid);
	// delete IME record
	for (auto it = links_.begin(); it != links_.end(); ) {
		if (it->Ampid == ampid) {	// ime-ampid can not find
			it = links_.erase(it);
		} else {
			++it;
		}
	}
	logInfo(CS, "IME Close OUT");
}

void ImeMgr::setFocusAmpid(uint32_t ampid)
{
    int seat = (ampid & 0xF000) >> 12;
	switch (seat)
	{
		case 1:
			focusAmpid1_ = ampid;
			break;
		case 2:
			focusAmpid2_ = ampid;
			break;
		case 3:
			focusAmpid3_ = ampid;
			break;
	}
}

uint32_t ImeMgr::getFocusAmpid(uint32_t appAmpid)
{
	int seat = (appAmpid & 0xF000) >> 12;
	switch (seat)
	{
		case 1:
			return focusAmpid1_;
		case 2:
			return focusAmpid2_;
		case 3:
			return focusAmpid3_;
	}
}
