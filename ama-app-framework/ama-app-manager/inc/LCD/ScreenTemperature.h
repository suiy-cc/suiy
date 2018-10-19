/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef SCREENTemperature_H
#define SCREENTemperature_H


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "log.h"
#include <string.h>

#include <errno.h>// 包含errno所需要的头文件

extern LOG_DECLARE_CONTEXT(LCDM);
#define TMPTURE_FILE_BUFFER 16
#define TEMPERATURE_DETECT_STEP 25
class ScreenTemperature
{
public:
    ScreenTemperature(const char* temperatureDevPath);

    ~ScreenTemperature();
    void init();
    void deInit();
    char *GetTemperatureDevPath() ;
    void SetTemperatureDevPath(char *temperatureDevPath);
    int  GetTemperature();
private :
    int temperatureFile=0;
    char m_temperatureDevPath[64]={0};
};

#endif // SCREENTemperature_H
