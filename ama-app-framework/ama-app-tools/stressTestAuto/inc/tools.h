/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef TOOLS_H
#define TOOLS_H

#ifndef __cplusplus//识别C/C++的宏，望文生意
    #define _GNU_SOURCE
#endif

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()

#ifdef __cplusplus
    #include <iostream>
    #include <cstdlib>
    #include <cstring>
    #include <cstdarg>
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdarg.h>
#endif

pid_t getPidOfProc(const char* procName);
bool isProcOfPidExist(pid_t pid);
void delayAfterCallAPI(void);
int getRand(int min,int max);

#endif//TOOLS_H
