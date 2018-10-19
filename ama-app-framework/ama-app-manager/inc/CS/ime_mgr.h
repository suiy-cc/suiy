/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef _IME_MGR_H
#define _IME_MGR_H

#include <ama_types.h>
#include <string>
#include <mutex>
#include <vector>

#define E_IME_APP_SHOW  (1)
#define E_IME_APP_HIDE  (0)

enum EKeyboardType
{
	E_KEYBOARD_TYPE_NINE,
	E_KEYBOARD_TYPE_FULL,
};

enum ELangType
{
	E_LANG_TYPE_CN,
	E_LANG_TYPE_EN,
};

typedef struct _IMEContext
{
	EKeyboardType KeyboardType;
	ELangType     LangType;
	std::string   ParamString;
} IMEContext;

typedef IMEContext ImeArgument;

typedef struct _Link
{
	AMPID Ampid;
	int   Show;
	IMEContext Ctx;
} Link;

class ImeMgr
{
public:
	ImeMgr();
	void createLink(AMPID ampid, uint32_t keyboardType, uint32_t langType, std::string paramStr);
    void closeIME(AMPID ampid);
	void schedIME(uint32_t ampid);
	void updateAppShowStatus(AMPID ampid);
	uint32_t getFocusAmpid(uint32_t appAmpid);
	
private:
	void activateIME(uint32_t ampid);	// resume IME prev status
	static void updateAppsFocusAmpid(AMPID ampid);
	void setFocusAmpid(uint32_t ampid);
	
	std::mutex mtx_;
	std::vector<Link> links_;
	uint32_t focusAmpid1_ = 0;
	uint32_t focusAmpid2_ = 0;
	uint32_t focusAmpid3_ = 0;
};

extern ImeMgr gImeMgr;
#endif
