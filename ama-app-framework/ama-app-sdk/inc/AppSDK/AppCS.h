/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _APP_CS_H
#define _APP_CS_H

#include <vector>
#include <string>
#include <stdint.h>
#include <ama_types.h>

enum PopupType
{
	E_POPUP_TYPE_VOL,
	E_POPUP_TYPE_USB,
    E_POPUP_TYPE_BT,
	E_POPUP_TYPE_MSG,
};

typedef uint32_t PopupID;

typedef void (*PopupCallbackFun)(PopupID id, uint32_t reason);
typedef void (*PopupCallbackFunEx)(PopupID id, uint32_t reason,const std::string& message);

typedef struct _PopupInfo
{
	PopupType Pt;
	std::string Value;
	int  Prior = 0;
	uint32_t ScreenIndex;
} PopupInfo;


void psInitPopup();
PopupID psRequestPopup(PopupInfo pi);
void psRegisterClickEvent(PopupID id, PopupCallbackFun pcf);
void psRegisterExClickEvent(PopupID id, PopupCallbackFunEx pcf);
void psShowPopup(PopupID id);
void psClosePopup(PopupID id);

/// @name last source
/// @{
class IntentInformation{
public:
    IntentInformation()
    : isQuiet(false)
    {
        ;
    };
    // move constructor
    IntentInformation(IntentInformation&& info)
    {
        *this = std::move(info);
    };
    // move assign operator
    IntentInformation& operator=(IntentInformation&& info)
    {
        itemID = std::move(info.itemID);
        action = std::move(info.action);
        data = std::move(info.data);
        isQuiet = info.isQuiet;

        return *this;
    };

public:
    std::string itemID;
    std::string action;
    std::string data;
    bool isQuiet;
};

// set instant muti-lmedia source
void SetCurrentSource(const std::vector<IntentInformation>& lastSourceSet);
// get SetCurrentSource()'s input or the last source if current source is empty
std::vector<IntentInformation> GetCurrentSource();
// return the actual last source of current power cycle
std::vector<IntentInformation> GetLastSource();
/// @}

#endif
