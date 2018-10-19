/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef _APP_IME_H
#define _APP_IME_H

#include <string>

enum EKeyboardType
{
	E_KEYBOARD_TYPE_NINE,
	E_KEYBOARD_TYPE_FULL,
};

enum ELangType
{
	E_LANG_TYPE_CN,
	E_LANG_TYPE_EN,
	E_LANG_TYPE_HWR,
    E_LANG_TYPE_NUM,
    E_LANG_TYPE_CHAR

};

typedef struct _IMEContext
{
	EKeyboardType KeyboardType;
	ELangType     LangType;
	std::string   BuffString;
} IMEContext;

enum EMsgType
{
	E_MSG_TYPE_NORMAL,
	E_MSG_TYPE_BUFF,
	E_MSG_TYPE_DEL,
    E_MSG_TYPE_CONFIRM,
};

enum EEventType
{
	E_EVT_CLEAR_INPUT,
};

typedef void (*ImeInputCallback)(std::string msg, EMsgType msgType);
typedef void (*ImeClosedCallback)();

void imeInit();
void imeOpenIME(IMEContext);
void imeSendEvent(int eventType, std::string param = "");
void imeRegisterInputEvent(ImeInputCallback icb);
void imeRegisterClosedEvent(ImeClosedCallback ccb);
void imeCloseIME();

#endif
