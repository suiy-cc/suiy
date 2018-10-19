/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef INPUTDEV_H
#define INPUTDEV_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "log.h"

extern LOG_DECLARE_CONTEXT(LCDM);

class InputDevice
{
public:
    InputDevice(const char *DevPath);
    //InputDevice();
    ~InputDevice();
    bool checkPress();
private:

    char m_DevPath[64]={0};
    int inputFile=0;//hd or input dev file
    void init();
    void deInit();
};

#endif // INPUTDEV_H
