/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
/// @file FileSystem.h
/// @brief contains
///
/// Created by zs on 2017/11/09.
///

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <mutex>

class Inotify
{
public:
    static Inotify* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static Inotify instance;
        return &instance;
    }
    virtual ~Inotify();

    bool waitAndCheckIsFileCreated(const std::string& folder, const std::string& fileName, unsigned int timeout);
    bool waitAndCheckIsFileWrited(const std::string& filePath, unsigned int timeout);
private:
    Inotify();

    int mInotifyFd;
    int mEpollFd;
};

#endif // FILESYSTEM_H
