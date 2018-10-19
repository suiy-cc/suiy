/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

 #ifndef RADARSOUNDTHREAD_H_
 #define RADARSOUNDTHREAD_H_

 #include <mutex>
 #include <vector>
 #include <map>
 #include <condition_variable>
 #include <thread>
 #include <stdio.h>
 #include <unistd.h>

 #include "ama_audioTypes.h"
 #include "ama_types.h"
 #include "SoundDef.h"
 #include "ISAudioStream.h"

 extern "C"
 {
 #include <ah/simple.h>
 };

 #define BUF_SIZE         1024

 #define FL_CONNECTID       (CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(0x10ff,E_STREAM_FL_RADAR,E_IVI_SPEAKER))
 #define FR_CONNECTID       (CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(0x10ff,E_STREAM_FR_RADAR,E_IVI_SPEAKER))
 #define RL_CONNECTID       (CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(0x10ff,E_STREAM_RL_RADAR,E_IVI_SPEAKER))
 #define RR_CONNECTID       (CREATE_CONNECTID_BY_AMPID_STREAMTYPE_DEVICETYPE(0x10ff,E_STREAM_RR_RADAR,E_IVI_SPEAKER))

 using std::string;
 using std::shared_ptr;
 using std::vector;
 using std::map;
 using std::condition_variable;

 class RadarSoundThread
 {
 public:
     RadarSoundThread();
     ~RadarSoundThread();
     void start();
     void stop();

     void playRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel);
 private:
     void run();
     void playOnce();
     void startPlay();
     void stopPlay();

     void closeAllCurRadarChannel(void);
     void refreshRadarChannel(ama_radarChannel_t newChannel);

     void setPlayFilePath(string filePath);

     bool isSuccessToCreateAHHandler(void);
 private:
     bool mIsRunning;
     bool mIsSetToStop;
     bool mIsPlaying;
     bool mIsInterruptPlay;

     shared_ptr<ISAudioStream> mpFLAudioStream;
     shared_ptr<ISAudioStream> mpFRAudioStream;
     shared_ptr<ISAudioStream> mpRLAudioStream;
     shared_ptr<ISAudioStream> mpRRAudioStream;

     string mPlayFilePath;
     int mFileDataSize;
     std::thread mThisThread;
     std::mutex mMutexOfAHHandler;
     ah_simple_t *mpAHHandler;

     std::mutex mMutexOfCondition;
     std::condition_variable mConditionVariable;

     ama_radarSoundType_t mCurType;
     ama_radarChannel_t mCurChannel;
 };

 #endif //RADARSOUNDTHREAD_H_
