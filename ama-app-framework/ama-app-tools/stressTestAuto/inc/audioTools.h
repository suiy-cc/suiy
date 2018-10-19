/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef AUDIOTOOLS_H
#define AUDIOTOOLS_H

#include <mutex>
#include <vector>
#include <map>
#include <condition_variable>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>

#include <AppSDK.h>
#include <log.h>

extern "C"
{
#include <ah/simple.h>
};

#define BUF_SIZE         1024

extern LogContext TEST_AUDIO;

#define DEMO_PLAYFILE   "/etc/ama.d/rec_RSST_nenRadarHigh.wav"


using std::string;
using std::shared_ptr;
using std::vector;
using std::map;
using std::condition_variable;

class WAVPlayer
{
public:
    WAVPlayer(string streamName);
    ~WAVPlayer();
    void startPlay();
    void stopPlay();
    void pause();

    void start();
private:
    void run();
   //  void start();
    void stop();
    void playOnce();

    void setPlayFilePath(string filePath);
    bool isSuccessToCreateAHHandler(void);
private:
    bool mIsRunning;
    bool mIsSetToStop;
    bool mIsPlaying;
    bool mIsInterruptPlay;

    string mStreamName;
    string mPlayFilePath;
    std::thread mThisThread;
    std::mutex mMutexOfAHHandler;
    ah_simple_t *mpAHHandler;

    std::mutex mMutexOfCondition;
    std::condition_variable mConditionVariable;
};


class AppStream
: public AppAudioStream
{
public:
    AppStream(ama_streamType_t streamType);
    virtual ~AppStream();
    void initialize(void);
private:
    // void OnAudioFocusChangedForDefDev(ama_focusSta_t newFocusSta);
    void OnAudioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta);
    WAVPlayer *mPlayer;
};


#endif //AUDIOTOOLS_H
