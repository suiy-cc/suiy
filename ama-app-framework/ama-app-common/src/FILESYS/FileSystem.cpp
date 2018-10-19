/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FileSystem.cpp
/// @brief contains filesystem functions
///
/// Created by zs on 2017/11/09.
///

#include "FILESYS/FileSystem.h"
#include "COMMON/Timeout.h"

#include <fstream>
#include <thread>
#include "log.h"

bool stdlib_WaitPath(const std::string& path, unsigned int timeout)
{
    Timeout timer;
    timer.SetTimeout(std::chrono::milliseconds(timeout/2));
    timer.StartTiming();
    std::ifstream iFile(path.c_str());
    if(iFile.is_open())
        return true;
    while(!iFile.is_open() && !timer.IsTimeout())
    {
        // sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout/2));

        // check file existance
        iFile.close();
        iFile.clear();
        iFile.open(path.c_str());
        if(iFile.is_open())
            return true;
    }

    return false;
}

bool WaitPath(const std::string& path, unsigned int timeout)
{
    return stdlib_WaitPath(path, timeout);
}

///////////////////////////////////////////////
// linux platform
///////////////////////////////////////////////
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

#define INOTIFY_BUF 1000

Inotify::Inotify()
:mInotifyFd(0)
,mEpollFd(0)
{
    mInotifyFd = inotify_init();
    logDebug("Inotify::Inotify()-->open mInotifyFd=",mInotifyFd);
    mEpollFd = epoll_create(1);
    logDebug("Inotify::Inotify()-->open mEpollFd=",mEpollFd);
}

Inotify::~Inotify()
{
    close(mInotifyFd);
    logDebug("Inotify::~Inotify()-->close mInotifyFd=",mInotifyFd);
    close(mEpollFd);
    logDebug("Inotify::~Inotify()-->close mInotifyFd=",mEpollFd);
}

bool Inotify::waitAndCheckIsFileCreated(const std::string& folder, const std::string& fileName, unsigned int timeout)
{
    int ret = false;
    std::ifstream iFile((folder+'/'+fileName).c_str());
    if(iFile.is_open())
        return true;

    auto watchDescriptor = inotify_add_watch(mInotifyFd, folder.c_str(), IN_CREATE);
    logDebug("Inotify::waitAndCheckIsFileCreated()-->open watchDescriptor=",watchDescriptor);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = watchDescriptor;
    epoll_event eventArray[1];
    epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mInotifyFd, &event);
    unsigned char inotify_buf[INOTIFY_BUF];
    int eventCount = epoll_wait(mEpollFd, eventArray, 1, timeout);
    for(int i= 0; i < eventCount; ++i)
    {
        if(eventArray[i].data.fd==watchDescriptor)
        {
            memset(inotify_buf, 0, INOTIFY_BUF);
            inotify_event* event = NULL;
            int size = read(mInotifyFd, inotify_buf, INOTIFY_BUF);
            event = reinterpret_cast<inotify_event*>(inotify_buf);
            logDebugF("Inotify::event->wd[%d] \n",event->wd);
            logDebugF("Inotify::event->mask[%x] \n",event->mask);
            logDebug("Inotify::event->name:",event->name);
            if(event->wd==watchDescriptor && (event->mask&IN_CREATE) && 0==strncmp(event->name, fileName.c_str(), event->len)){
                epoll_ctl(mEpollFd,EPOLL_CTL_DEL,mInotifyFd,NULL);
                inotify_rm_watch(mInotifyFd, watchDescriptor);
                logDebug("Inotify::waitAndCheckIsFileCreated()-->remove watchDescriptor=",watchDescriptor);
                ret = true;
                break;
            }
        }
    }

    return ret;
}

bool Inotify::waitAndCheckIsFileWrited(const std::string& filePath, unsigned int timeout)
{
    int ret = false;

    auto watchDescriptor = inotify_add_watch(mInotifyFd, filePath.c_str(), IN_MODIFY);
    logDebug("Inotify::waitAndCheckIsFileWrited()-->open watchDescriptor=",watchDescriptor);

    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = watchDescriptor;
    epoll_event eventArray[10];
    epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mInotifyFd, &event);
    unsigned char inotify_buf[INOTIFY_BUF];
    int eventCount = epoll_wait(mEpollFd, eventArray, 10, timeout);
    for(int i= 0; i < eventCount; ++i)
    {
        logDebug("Inotify:: epollEvent[%d].data.fd[%d]  \n",i,eventArray[i].data.fd);
        if(eventArray[i].data.fd==watchDescriptor)
        {
            logDebug("Inotify:: eventArray[i].data.fd==watchDescriptor \n");
            memset(inotify_buf, 0, INOTIFY_BUF);
            inotify_event* event = NULL;
            int size = read(mInotifyFd, inotify_buf, INOTIFY_BUF);
            event = reinterpret_cast<inotify_event*>(inotify_buf);
            logDebugF("Inotify::event->wd[%d] \n",event->wd);
            logDebugF("Inotify::event->mask[%x] \n",event->mask);
            logDebug("Inotify::event->name:",event->name);
            if(event->wd==watchDescriptor && (event->mask&IN_MODIFY)){
                epoll_ctl(mEpollFd,EPOLL_CTL_DEL,mInotifyFd,NULL);
                inotify_rm_watch(mInotifyFd, watchDescriptor);
                logDebug("Inotify::waitAndCheckIsFileWrited()-->remove watchDescriptor=",watchDescriptor);
                ret = true;
                break;
            }
        }
    }

    return ret;
}
