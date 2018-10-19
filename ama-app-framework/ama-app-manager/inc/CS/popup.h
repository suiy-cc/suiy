/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _POPUP_CONTROLLER_H
#define _POPUP_CONTROLLER_H

#include <vector>
#include <memory>
#include <string>
#include "ama_types.h"

using namespace std;

enum PopupState
{
	E_POPUP_STATE_SHOW,
	E_POPUP_STATE_HIDE,
};

enum PopupType
{
	E_POPUP_TYPE_VOL,
	E_POPUP_TYPE_USB,
	E_POPUP_TYPE_MSG,
};

enum ButtonType
{
	E_BUTTON_TYPE_ONE,
	E_BUTTON_TYPE_TWO,
};

enum PopPrority
{
	E_POP_PRORITY_HIGH   = 5,
	E_POP_PRORITY_NORMAL = 4,
	E_POP_PRORITY_LOW    = 3,
};

struct PopInfo
{
	PopInfo() = default;
    PopInfo(PopupType pt, std::string value, uint32_t ampid, uint32_t srcIdx) : Pt(pt), Value(value), Ampid(ampid), ScreenIndex(srcIdx)
	{

	}
	PopupType Pt;
	ButtonType Bt;
	int Priority;
	std::string Value;
	uint32_t Ampid;
	uint32_t ScreenIndex;
};

bool operator<(const PopInfo & pi1, const PopInfo & pi2);

//typedef void (*PopupCallbackFun)(PopupID popId);

class PopupManager
{
public:
	PopupManager();
	//void requestPopup()
	void putPopInfo(uint32_t popId, const PopInfo & pi);
    std::vector<PopInfo> getPopInfoList();
	void removePopInfo(uint32_t popId);
	uint32_t getPopupId();
	void handlePopupCtrl(uint32_t scrIdx);
	void processPopupRespond(uint32_t popId, uint32_t reason);
	void processPopupRespond(uint32_t popId, uint32_t reason, std::string message);

private:
	void updatePopupList();
	bool removePopInfoInter(uint32_t popId, std::vector<std::pair<uint64_t, PopInfo>> &pil, int srcIdx);
	void handlePopupIter(std::vector<std::pair<uint64_t, PopInfo>> &pil, E_SEAT seat);

	uint32_t popupId_;
	std::vector<std::pair<uint64_t, PopInfo>> iviPopInfoList_;
	std::vector<std::pair<uint64_t, PopInfo>> rse1PopInfoList_;
	std::vector<std::pair<uint64_t, PopInfo>> rse2PopInfoList_;
};

#endif
