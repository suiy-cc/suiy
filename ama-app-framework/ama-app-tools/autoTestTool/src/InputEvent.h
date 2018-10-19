/*
* Copyright (C) 2016-2017 Neusoft, Inc.
* All Rights Reserved.
*
* The following programs are the sole property of Neusoft Inc.,
* and contain its proprietary and confidential information.
*/

#ifndef INPUTEVENT_H_
#define INPUTEVENT_H_

#include <linux/input.h>

#define TOUCH_DEVICE     ("/dev/input/event2")

void showTouchEventDevice(void);
void startPrintInputEvent(void);
void stopPrintInputEvent(void);
void printEvent(input_event event);

#endif//INPUTEVENT_H_
