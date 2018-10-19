/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "RadarSoundThread.h"
#include "TaskDispatcher.h"
#include "ISCtrl.h"
#include "logDef.h"

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

RadarSoundThread::RadarSoundThread()
:mIsRunning(false)
,mIsSetToStop(false)
,mIsPlaying(false)
,mIsInterruptPlay(false)
,mPlayFilePath("")
,mpAHHandler(nullptr)
,mCurType(E_NO_TONE)
,mCurChannel(E_NO_CHANNEL)
,mFileDataSize(0)
{
    logVerbose(IS,"RadarSoundThread::RadarSoundThread()-->IN");

    mpFLAudioStream = std::make_shared<ISAudioStream>(E_STREAM_FL_RADAR,E_IVI_SPEAKER);
    mpFRAudioStream = std::make_shared<ISAudioStream>(E_STREAM_FR_RADAR,E_IVI_SPEAKER);
    mpRLAudioStream = std::make_shared<ISAudioStream>(E_STREAM_RL_RADAR,E_IVI_SPEAKER);
    mpRRAudioStream = std::make_shared<ISAudioStream>(E_STREAM_RR_RADAR,E_IVI_SPEAKER);

    logVerbose(IS,"RadarSoundThread::RadarSoundThread()-->OUT");
}

RadarSoundThread::~RadarSoundThread()
{
    logVerbose(IS,"RadarSoundThread::~RadarSoundThread()-->IN");

    if(mpAHHandler){
        mMutexOfAHHandler.lock();
        ah_simple_free(mpAHHandler);
        mpAHHandler = nullptr;
        mMutexOfAHHandler.unlock();
    }

    logVerbose(IS,"RadarSoundThread::~RadarSoundThread()-->OUT");
}

void RadarSoundThread::startPlay()
{
    logVerbose(IS,"RadarSoundThread::startPlay()-->IN");

    while(!mIsPlaying){
        mConditionVariable.notify_one();
        usleep(500);
    }
    logDebug(IS,"mIsPlaying:",mIsPlaying);


    logVerbose(IS,"RadarSoundThread::startPlay()-->OUT");
}

void RadarSoundThread::stopPlay()
{
    logVerbose(IS,"RadarSoundThread::stopPlay()-->IN");

    if(mIsPlaying){
        mIsInterruptPlay = true;
        while(mIsPlaying){
            usleep(500);
        }
        logDebug(IS,"mIsPlaying:",mIsPlaying);
        mIsInterruptPlay = false;
    }

    logVerbose(IS,"RadarSoundThread::stopPlay()-->OUT");
}

void RadarSoundThread::run()
{
    logVerbose(IS,"RadarSoundThread::Run()-->IN");

    try
    {
        mIsRunning = true;
        mIsSetToStop = false;
        while(true){

            if(!mIsPlaying){
                std::unique_lock<std::mutex> uLock(mMutexOfCondition);
                logDebug(IS,"->->wating set to play");
                mConditionVariable.wait(uLock);
                mIsPlaying = true;
                mIsInterruptPlay = false;
                logDebug(IS,"->->start playing");
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
    }
    catch(std::exception& e)
    {
        logError(IS, LOG_HEAD, "exception is caught: ", (&e)->what());
    }
    catch(...)
    {
        logError(IS, LOG_HEAD, "unexpected exception is caught!");
    }

    logVerbose(IS,"RadarSoundThread::Run()-->OUT");
}

void RadarSoundThread::start()
{
    logVerbose(IS,"RadarSoundThread::start()-->IN");
    if(!mIsRunning){
        mThisThread = std::thread(std::bind(&RadarSoundThread::run, this));
    }else{
        logError(IS,"can't start because not stop thread");
    }
    logVerbose(IS,"RadarSoundThread::start()-->OUT");
}

void RadarSoundThread::stop()
{
    logVerbose(IS,"RadarSoundThread::stop()-->IN");

    while(mIsRunning){
        mIsSetToStop = true;
        // wake up fadein thread to let it finish.
        mConditionVariable.notify_one();
        usleep(500);
    }

    mThisThread.join();

    logVerbose(IS,"RadarSoundThread::stop()-->OUT");
}

void RadarSoundThread::setPlayFilePath(string filePath)
{
    logVerbose(IS,"RadarSoundThread::setPlayFilePath()-->IN");

    mPlayFilePath = filePath;

    if(mpAHHandler){
        mMutexOfAHHandler.lock();
        ah_simple_free(mpAHHandler);
        mpAHHandler = nullptr;
        mMutexOfAHHandler.unlock();
    }

    isSuccessToCreateAHHandler();

    logVerbose(IS,"RadarSoundThread::setPlayFilePath()-->OUT");
}

bool RadarSoundThread::isSuccessToCreateAHHandler(void)
 {
     logVerbose(IS,"RadarSoundThread::isSuccessToCreateAHHandler()-->IN");

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

         logDebug(IS,"wav.file_size:",wav.file_size," wav.channel:",wav.channel," wav.frequency:",wav.frequency);
         logDebug(IS,"wav.Bps:",wav.Bps," wav.sample_num_bit:",wav.sample_num_bit," wav.data_size:",wav.data_size);

         struct stat buf;
         stat(mPlayFilePath.c_str(),&buf);
         int realFileSize = buf.st_size;
         int realDataSize = realFileSize - 44;
         logDebug(IS,"realFileSize:",realFileSize);
         logDebug(IS,"realDataSize:",realDataSize);

         mFileDataSize = realDataSize;

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
             logWarn(IS,"ah_simple_new() failed error");
             return false;
         }
     }else{
         return false;
     }

     logVerbose(IS,"RadarSoundThread::isSuccessToCreateAHHandler()-->OUT");

     return true;
 }

void RadarSoundThread::playOnce()
{
    logVerbose(IS,"RadarSoundThread::playOnce()-->IN");

    int fd;

    /* replace STDIN with the specified file if needed */
    logInfo(IS,"mPlayFilePath:",mPlayFilePath);
    if (mPlayFilePath != "") {
        if ((fd = open(mPlayFilePath.c_str(), O_RDONLY)) < 0) {
            logError(IS,"open play file failed");
            return;
        }
        lseek(fd,44,SEEK_SET);
    }else{
        logError(IS,"havn't set playFile");
        return;
    }

    if(!mpAHHandler){
        logWarn(IS,"mpAHHandler==nullptr,try to create mpAHHandler again!");
        if(!isSuccessToCreateAHHandler()){
            logError(IS,"create mpAHHandler failed when we play");
        }
    }

    logDebug(IS,"play sound start");
    if(mpAHHandler){
        uint8_t buf[mFileDataSize];
        ssize_t r = 0;
        int ret = 0;

        /* Read all data one time*/
        r = read(fd, buf, sizeof(buf));
        logDebug(IS,"read data size:",r);
        close(fd);
        if( (r<=0) || (r>mFileDataSize)){
            logError(IS,"can't read data from file!");
            return;
        }else if(r<mFileDataSize){
            logWarn(IS,"can't read all data from wav file!");
        }else{
            logDebug(IS,"read all data from wav file!");
        }

        /* ... and play it */
        mMutexOfAHHandler.lock();
        logDebug(IS,"radarThread:->->playOnce()-->before ah_simple_write()");
        ret = ah_simple_write(mpAHHandler, buf, (size_t) r);
        logDebug(IS,"ret:",ret);
        logDebug(IS,"radarThread:->->playOnce()-->after ah_simple_write()");
        mMutexOfAHHandler.unlock();
        if(ret==0){
            logDebug(IS,"write all data successed!");
        }else{
            logError(IS,"play playFile in paluseAudio failed");
            return;
        }
    }else{
        logError(IS,"mpAHHandler == nullptr");
        return;
    }

    logVerbose(IS,"RadarSoundThread::playOnce()-->OUT");
}

void RadarSoundThread::playRadarSound(ama_radarSoundType_t soundType,ama_radarChannel_t highestLevelChannel)
{
    logVerbose(IS,"RadarSoundThread::playRadarSound()-->IN soundType:",ToHEX(soundType)," highestLevelChannel:",ToHEX(highestLevelChannel));

    if((E_NO_TONE==soundType)||(E_NORMAL_TONE==soundType)){
        mCurType = soundType;
        stopPlay();
        closeAllCurRadarChannel();
    }else{
        if(soundType!=mCurType){
            mCurType = soundType;
            switch(soundType){
            case E_CONTINUOUS_TONE:
                stopPlay();
                refreshRadarChannel(highestLevelChannel);
                setPlayFilePath(CONTINUOUS_TONE_PLAYFILE);
                // wake thread to play file
                startPlay();
                break;
            case E_HIGH_FREQ_TONE:
                stopPlay();
                refreshRadarChannel(highestLevelChannel);
                setPlayFilePath(HIGH_FREQ_TONE_PLAYFILE);
                // wake thread to play file
                startPlay();
                break;
            case E_LOW_FREQ_TONE:
                stopPlay();
                refreshRadarChannel(highestLevelChannel);
                setPlayFilePath(LOW_FREQ_TONE_PLAYFILE);
                // wake thread to play file
                startPlay();
                break;
            default:
                logError(IS,"Error sound Type!");
                break;
            }
        }else{//(mCurType==soundType)
            refreshRadarChannel(highestLevelChannel);
        }
    }

    logVerbose(IS,"RadarSoundThread::playRadarSound()-->OUT");
}

void RadarSoundThread::closeAllCurRadarChannel(void)
{
    logVerbose(IS,"RadarSoundThread::closeAllCurRadarChannel()-->IN");

    if(mCurChannel&E_FL_CHANNEL){
        mpFLAudioStream->abandonAudioFocusForDefDev();
    }
    if(mCurChannel&E_FR_CHANNEL){
        mpFRAudioStream->abandonAudioFocusForDefDev();
    }
    if(mCurChannel&E_RL_CHANNEL){
        mpRLAudioStream->abandonAudioFocusForDefDev();
    }
    if(mCurChannel&E_RR_CHANNEL){
        mpRRAudioStream->abandonAudioFocusForDefDev();
    }
    mCurChannel = E_NO_CHANNEL;

    logVerbose(IS,"RadarSoundThread::closeAllCurRadarChannel()-->OUT");
}

void RadarSoundThread::refreshRadarChannel(ama_radarChannel_t newChannel)
{
    logVerbose(IS,"RadarSoundThread::refreshRadarChannel()-->IN");

    if((mCurChannel&E_FL_CHANNEL)!=(newChannel&E_FL_CHANNEL)){
        if(newChannel&E_FL_CHANNEL){
            mpFLAudioStream->requestAudioFocusForDefDev();
        }else{
            mpFLAudioStream->abandonAudioFocusForDefDev();
        }
    }
    if((mCurChannel&E_FR_CHANNEL)!=(newChannel&E_FR_CHANNEL)){
        if(newChannel&E_FR_CHANNEL){
            mpFRAudioStream->requestAudioFocusForDefDev();
        }else{
            mpFRAudioStream->abandonAudioFocusForDefDev();
        }
    }
    if((mCurChannel&E_RL_CHANNEL)!=(newChannel&E_RL_CHANNEL)){
        if(newChannel&E_RL_CHANNEL){
            mpRLAudioStream->requestAudioFocusForDefDev();
        }else{
            mpRLAudioStream->abandonAudioFocusForDefDev();
        }
    }
    if((mCurChannel&E_RR_CHANNEL)!=(newChannel&E_RR_CHANNEL)){
        if(newChannel&E_RR_CHANNEL){
            mpRRAudioStream->requestAudioFocusForDefDev();
        }else{
            mpRRAudioStream->abandonAudioFocusForDefDev();
        }
    }
    mCurChannel = newChannel;

    logVerbose(IS,"RadarSoundThread::refreshRadarChannel()-->OUT");
}
