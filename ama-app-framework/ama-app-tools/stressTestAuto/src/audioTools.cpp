/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "audioTools.h"
#include "tools.h"
#include <ama_types.h>

using namespace std;

struct wav_struct
{
    uint32_t file_size;        //文件大小
    uint16_t channel;            //通道数
    uint32_t frequency;        //采样频率
    uint32_t Bps;                //Byte率
    uint16_t sample_num_bit;    //一个样本的位数
    uint32_t data_size;        //数据大小
};

AppStream::AppStream(ama_streamType_t streamType)
:AppAudioStream(streamType)
,mPlayer(NULL)
{
    logVerbose(TEST_AUDIO,"AppStream::AppStream()-->>in");

    logVerbose(TEST_AUDIO,"AppStream::AppStream()-->>out");
}

AppStream::~AppStream()
{
    logVerbose(TEST_AUDIO,"AppStream::AppStream()-->>in");

    logVerbose(TEST_AUDIO,"AppStream::AppStream()-->>out");
}

void AppStream::initialize(void)
{
    logVerbose(TEST_AUDIO,"AppStream::initialize()-->>IN");

    logInfo(TEST_AUDIO,"streamID:",this->streamIDStr());

    if(NULL == mPlayer){
        mPlayer = new WAVPlayer(this->streamIDStr());
        mPlayer->start();
    }

    logVerbose(TEST_AUDIO,"AppStream::initialize()-->>OUT");
}

void AppStream::OnAudioFocusChanged(ama_audioDeviceType_t audioDevice,ama_focusSta_t newFocusSta)
{
    logVerbose(TEST_AUDIO,"AppStream::OnAudioFocusChangedForDefDev()-->>in");

    switch(audioDevice){
    case E_IVI_SPEAKER:
    case E_RSE_L_HEADSET:
    case E_RSE_R_HEADSET:
            if(E_FOUCS_GAIN==newFocusSta){
                if(mPlayer){
                    mPlayer->startPlay();
                }
            }else if(E_FOUCS_LOSE==newFocusSta){
                if(mPlayer){
                    mPlayer->stopPlay();
                }
            }else if(E_FOUCS_LOSE_TRANSIENT==newFocusSta){
                if(mPlayer){
                    mPlayer->pause();
                }
            }
            break;
    default:
        logError(TEST_AUDIO,"audioDevice is not defined");
        break;
    }

    logVerbose(TEST_AUDIO,"AppStream::OnAudioFocusChangedForDefDev()-->>out");
}

WAVPlayer::WAVPlayer(string streamName)
:mStreamName(streamName)
,mIsRunning(false)
,mIsSetToStop(false)
,mIsPlaying(false)
,mIsInterruptPlay(false)
,mPlayFilePath("")
,mpAHHandler(NULL)
{
    logVerbose(TEST_AUDIO,"WAVPlayer::WAVPlayer()-->IN");

    // start();

    setPlayFilePath(DEMO_PLAYFILE);

    logVerbose(TEST_AUDIO,"WAVPlayer::WAVPlayer()-->OUT");
}

WAVPlayer::~WAVPlayer()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::~WAVPlayer()-->IN");

    if(mpAHHandler){
        mMutexOfAHHandler.lock();
        ah_simple_free(mpAHHandler);
        mpAHHandler = nullptr;
        mMutexOfAHHandler.unlock();
    }

    stop();

    logVerbose(TEST_AUDIO,"WAVPlayer::~WAVPlayer()-->OUT");
}

void WAVPlayer::startPlay()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::startPlay()-->IN");

    while(!mIsPlaying){
        mConditionVariable.notify_one();
        usleep(500);
    }
    logDebug(TEST_AUDIO,"mIsPlaying:",mIsPlaying);

    logVerbose(TEST_AUDIO,"WAVPlayer::startPlay()-->OUT");
}

void WAVPlayer::stopPlay()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::stopPlay()-->IN");

    if(mIsPlaying){
        mIsInterruptPlay = true;
        while(mIsPlaying){
            usleep(500);
        }
        logDebug(TEST_AUDIO,"mIsPlaying:",mIsPlaying);
        mIsInterruptPlay = false;
    }

    logVerbose(TEST_AUDIO,"WAVPlayer::stopPlay()-->OUT");
}

void WAVPlayer::pause()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::pause()-->IN");

    stopPlay();

    logVerbose(TEST_AUDIO,"WAVPlayer::pause()-->OUT");
}

void WAVPlayer::run()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::Run()-->IN");

    mIsRunning = true;
    mIsSetToStop = false;
    while(true){
        if(!mIsPlaying){
            std::unique_lock<std::mutex> uLock(mMutexOfCondition);
            logInfo(TEST_AUDIO,"->->wating set to play");
            mConditionVariable.wait(uLock);
            mIsPlaying = true;
            mIsInterruptPlay = false;
            logInfo(TEST_AUDIO,"->->start playing");
        }

        //if set stop flag stop this thread
        if(mIsSetToStop){
            break;
        }

        //play the file always
        playOnce();

        if(mIsInterruptPlay){
            mIsPlaying = false;
        }
    }
    mIsRunning = false;

    logVerbose(TEST_AUDIO,"WAVPlayer::Run()-->OUT");
}

void WAVPlayer::start()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::start()-->IN");
    if(!mIsRunning){
        mThisThread = std::thread(std::bind(&WAVPlayer::run, this));
    }else{
        logError(TEST_AUDIO,"can't start because not stop thread");
    }
    logVerbose(TEST_AUDIO,"WAVPlayer::start()-->OUT");
}

void WAVPlayer::stop()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::stop()-->IN");

    while(mIsRunning){
        mIsSetToStop = true;
        // wake up fadein thread to let it finish.
        mConditionVariable.notify_one();
        usleep(500);
    }
    mThisThread.join();

    logVerbose(TEST_AUDIO,"WAVPlayer::stop()-->OUT");
}

void WAVPlayer::setPlayFilePath(string filePath)
{
    logVerbose(TEST_AUDIO,"WAVPlayer::setPlayFilePath()-->IN");

    mPlayFilePath = filePath;

    if(mpAHHandler){
        mMutexOfAHHandler.lock();
        ah_simple_free(mpAHHandler);
        mpAHHandler = nullptr;
        mMutexOfAHHandler.unlock();
    }

    isSuccessToCreateAHHandler();

    logVerbose(TEST_AUDIO,"WAVPlayer::setPlayFilePath()-->OUT");
}

bool WAVPlayer::isSuccessToCreateAHHandler(void)
 {
     logVerbose(TEST_AUDIO,"PlaySoundThread::isSuccessToCreateAHHandler()-->IN");

     if(mPlayFilePath!=""){
         fstream fs;
         wav_struct wav;

         fs.open(mPlayFilePath,ios::binary|ios::in);

         fs.seekg(0,ios::end);
         wav.file_size=fs.tellg();
         fs.seekg(0x16);
         fs.read((char*)&wav.channel,sizeof(wav.channel));
         fs.seekg(0x18);
         fs.read((char*)&wav.frequency,sizeof(wav.frequency));
         fs.seekg(0x1c);
         fs.read((char*)&wav.Bps,sizeof(wav.Bps));
         fs.seekg(0x22);
         fs.read((char*)&wav.sample_num_bit,sizeof(wav.sample_num_bit));
         fs.seekg(0x28);
         fs.read((char*)&wav.data_size,sizeof(wav.data_size));

         fs.close();

         logInfo(TEST_AUDIO,"wav.file_size:",wav.file_size," wav.channel:",wav.channel," wav.frequency:",wav.frequency);
         logInfo(TEST_AUDIO,"wav.Bps:",wav.Bps," wav.sample_num_bit:",wav.sample_num_bit," wav.data_size:",wav.data_size);

         static ah_sample_spec ss;
         switch(wav.sample_num_bit){
             case 8:ss.format = AH_SAMPLE_U8;break;
             case 16:ss.format = AH_SAMPLE_S16LE;break;
             case 24:ss.format = AH_SAMPLE_S24LE;break;
             case 32:ss.format = AH_SAMPLE_S32LE;break;
             default:break;
         }
         ss.rate = wav.frequency;
         ss.channels = wav.channel;

         mMutexOfAHHandler.lock();
         mpAHHandler = ah_simple_new("radarStream", AH_STREAM_PLAYBACK, &ss);
         mMutexOfAHHandler.unlock();
         if (!mpAHHandler) {
             logWarn(TEST_AUDIO,"ah_simple_new() failed error");
             return false;
         }
     }else{
         return false;
     }

     logVerbose(TEST_AUDIO,"PlaySoundThread::isSuccessToCreateAHHandler()-->OUT");

     return true;
 }

void WAVPlayer::playOnce()
{
    logVerbose(TEST_AUDIO,"WAVPlayer::playOnce()-->IN");

    int fd;

    /* replace STDIN with the specified file if needed */
    logInfo(TEST_AUDIO,"mPlayFilePath:",mPlayFilePath);
    if (mPlayFilePath != "") {
        if ((fd = open(mPlayFilePath.c_str(), O_RDONLY)) < 0) {
            logError(TEST_AUDIO,"open play file failed");
            return;
        }
        lseek(fd,44,SEEK_SET);
    }else{
        logError(TEST_AUDIO,"havn't set playFile");
        return;
    }

    if(!mpAHHandler){
        logInfo(TEST_AUDIO,"mpAHHandler==NULL,try to create mpAHHandler again!");
        if(!isSuccessToCreateAHHandler()){
            logError(TEST_AUDIO,"create mpAHHandler failed when we play");
        }
    }

    logInfo(TEST_AUDIO,"play sound start");
    if(mpAHHandler){
        for (;;) {
            uint8_t buf[BUF_SIZE];
            ssize_t r;
            int ret=0;

            /* Read 1K data ... */
            if ((r = read(fd, buf, sizeof(buf))) <= 0) {
                if (r==0)break;/* EOF */
                logError(TEST_AUDIO,"read playFile failed");
                close(fd);
                return;
           }
           /* ... and play it */
           mMutexOfAHHandler.lock();
           ret = ah_simple_write(mpAHHandler, buf, (size_t) r);
           mMutexOfAHHandler.unlock();
           if (ret < 0) {
               logError(TEST_AUDIO,"play playFile in paluseAudio failed");
               close(fd);
               return;
           }

           if(mIsInterruptPlay){
               close(fd);
               return;
           }
        }
    }else{
        logError(TEST_AUDIO,"mpAHHandler == NULL");
        return;
    }

    close(fd);

    logVerbose(TEST_AUDIO,"WAVPlayer::playOnce()-->OUT");
}
