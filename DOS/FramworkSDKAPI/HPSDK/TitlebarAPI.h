/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file TitlebarAPI.h
/// @brief contains title bar API
///
/// Created by zs on 2016/8/29.
/// this file contains the declaration of title bar API
///

#ifndef TITLEBARAPI_H
#define TITLEBARAPI_H

typedef void(*TitlebarVisibilitySwitchNotifyCB)(bool isVisible, int style);
void ListenToTitlebarVisibilitySwitchNotify(TitlebarVisibilitySwitchNotifyCB f);

typedef void(*TitlebarDropDownNotifyCB)(bool isDropDown, int style);
void ListenToTitlebarDropDownNotify(TitlebarDropDownNotifyCB f);

void NotifyTitlebarDropDown(bool isDropDown);

typedef void(*TitleBarScreenShotCompleteCB) (unsigned int seatID,char * picPath);
void ListenTitleBarScreenShotCompleteCB(TitleBarScreenShotCompleteCB f);

#endif // TITLEBARAPI_H
