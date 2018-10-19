/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

 #ifndef PLAYSOUNDTHREAD_H_
 #define PLAYSOUNDTHREAD_H_

 #include <mutex>
 #include <vector>
 #include <map>
 #include <condition_variable>
 #include <thread>
 #include <stdio.h>
 #include <unistd.h>

 #include "ama_audioTypes.h"
 #include "ama_types.h"
 #include "ISAudioStream.h"

 extern "C"
 {
 #include <ah/simple.h>
 };

 #define BUF_SIZE         1024

 using std::string;
 using std::shared_ptr;
 using std::vector;
 using std::map;
 using std::condition_variable;

 class PlaySoundThread
 {
 public:
     PlaySoundThread(shared_ptr<ISAudioStream> pAudioStream);
     ~PlaySoundThread();
     void start();
     void stop();

     void requestToPlay();
     void requestToPlayAlways();
     void requestToStopPlay();

     void OnGainAudioFocus();
     void OnLossAudioFocus();

     void setPlayFilePath(string filePath);
 private:
     void run();
     void playOnce();
     void drain();
     void startPlay();
     void stopPlay();

     bool isSuccessToCreateAHHandler(void);
 private:
     bool mIsRunning;
     bool mIsSetToStop;
     bool mIsPlaying;
     bool mIsInterruptPlay;
     bool mIsPlayOnce;

     shared_ptr<ISAudioStream> mpAudioStream;

     ama_connectID_t mConnectID;
     string mStreamName;
     string mPlayFilePath;
     std::thread mThisThread;
     std::mutex mMutexOfAHHandler;
     ah_simple_t *mpAHHandler;

     std::mutex mMutexOfCondition;
     std::condition_variable mConditionVariable;

     int mFileDataSize;
 };

 #endif //PLAYSOUNDTHREAD_H_
