/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file AppPictureDisplay.cpp
/// @brief contains APIs for picture-display
///
/// Created by zs on 2016/9/18.
/// this file contains APIs for picture-display
///

#include "AppPictureDisplay.h"

#include <thread>
#include <unistd.h>
#include <wait.h>

int StartPictureDisplayDemo(unsigned int surfaceID, const std::string& picturePath)
{
    int processID = fork();
    if(processID == 0)
    {
        execlp("/usr/local/bin/ama-app-picture-display/ama-app-picture-display-demo"
               , "ama-app-picture-display-demo"
               , std::to_string(surfaceID).c_str()
               , picturePath.c_str()
               , nullptr);
        exit(errno);
    }
    else
    {
        std::thread t([=](){
            int status = 0;
            waitpid(processID, &status, 0);
            StartPictureDisplayDemo(surfaceID, picturePath);
            printf("---123--- pid = %d status = %d\n", processID, status);
        });

        t.detach();
        return processID;
    }
}