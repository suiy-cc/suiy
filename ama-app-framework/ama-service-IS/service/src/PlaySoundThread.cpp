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

#include "PlaySoundThread.h"
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

PlaySoundThread::PlaySoundThread(shared_ptr<ISAudioStream> pAudioStream)
:mIsRunning(false)
,mIsSetToStop(false)
,mIsPlaying(false)
,mIsInterruptPlay(false)
,mPlayFilePath("")
,mConnectID(0x0)
,mpAHHandler(nullptr)
,mFileDataSize(0)
,mpAudioStream(pAudioStream)
,mIsPlayOnce(true)
{
    logVerbose(IS,"PlaySoundThread::PlaySoundThread()-->IN");

    mStreamName = pAudioStream->streamIDStr();
    mConnectID = pAudioStream->getConnectID();

    logVerbose(IS,"PlaySoundThread::PlaySoundThread()-->OUT");
}

PlaySoundThread::~PlaySoundThread()
{
    logVerbose(IS,"PlaySoundThread::~PlaySoundThread()-->IN");

    if(mpAHHandler){
        mMutexOfAHHandler.lock();
        ah_simple_free(mpAHHandler);
        mpAHHandler = nullptr;
        mMutexOfAHHandler.unlock();
    }

    logVerbose(IS,"PlaySoundThread::~PlaySoundThread()-->OUT");
}

void PlaySoundThread::startPlay()
{
    logVerbose(IS,"PlaySoundThread::startPlay()-->IN");

    while(!mIsPlaying){
        mConditionVariable.notify_one();
        usleep(500);
    }
    logDebug(IS,"mIsPlaying:",mIsPlaying);

    logVerbose(IS,"PlaySoundThread::startPlay()-->OUT");
}

void PlaySoundThread::stopPlay()
{
    logVerbose(IS,"PlaySoundThread::stopPlay()-->IN");

    if(mIsPlaying){
        mIsInterruptPlay = true;
        while(mIsPlaying){
            usleep(500);
        }
        logDebug(IS,"mIsPlaying:",mIsPlaying);
        mIsInterruptPlay = false;
    }

    logVerbose(IS,"PlaySoundThread::stopPlay()-->OUT");
}

void PlaySoundThread::run()
{
    logVerbose(IS,"playThread:",ToHEX(mConnectID),"->->PlaySoundThread::Run()-->IN");

    try
    {
        mIsRunning = true;
        mIsSetToStop = false;
        while(true){
            {
                if(!mIsPlaying){
                    std::unique_lock<std::mutex> uLock(mMutexOfCondition);
                    logDebug(IS,"playThread:",ToHEX(mConnectID),"->->wating set to play");
                    mConditionVariable.wait(uLock);
                    mIsPlaying = true;
                    mIsInterruptPlay = false;
                    logDebug(IS,"playThread:",ToHEX(mConnectID),"->->start playing");
                }
                //if set stop flag stop this thread
                if(mIsSetToStop){
                    break;
                }
            }

            playOnce();

            if(mIsPlayOnce){
                drain();
                usleep(5*1000);
                mpAudioStream->abandonAudioFocusForDev(GET_DEVICETYPE_FROM_CONNECTID(mConnectID));
                mIsPlaying = false;
            }else{
                if(mIsInterruptPlay){
                    mIsPlaying = false;
                }
                // usleep(50*1000);
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

    logVerbose(IS,"playThread:",ToHEX(mConnectID),"->->PlaySoundThread::Run()-->OUT");
}

void PlaySoundThread::start()
{
    logVerbose(IS,"PlaySoundThread::start()-->IN");
    if(!mIsRunning){
        mThisThread = std::thread(std::bind(&PlaySoundThread::run, this));
    }else{
        logError(IS,"can't start because not stop thread");
    }
    logVerbose(IS,"PlaySoundThread::start()-->OUT");
}

void PlaySoundThread::stop()
{
    logVerbose(IS,"PlaySoundThread::stop()-->IN");

    while(mIsRunning){
        mIsSetToStop = true;
        // wake up fadein thread to let it finish.
        mConditionVariable.notify_one();
        usleep(500);
    }

    mThisThread.join();

    logVerbose(IS,"PlaySoundThread::stop()-->OUT");
}

void PlaySoundThread::setPlayFilePath(string filePath)
{
    logVerbose(IS,"PlaySoundThread::setPlayFilePath()-->IN");

    mPlayFilePath = filePath;

    if(mpAHHandler){
        mMutexOfAHHandler.lock();
        ah_simple_free(mpAHHandler);
        mpAHHandler = nullptr;
        mMutexOfAHHandler.unlock();
    }

    isSuccessToCreateAHHandler();

    logVerbose(IS,"PlaySoundThread::setPlayFilePath()-->OUT");
}

void PlaySoundThread::drain()
{
    logVerbose(IS,"PlaySoundThread::drain()-->IN");

    if(mpAHHandler){
        mMutexOfAHHandler.lock();

        logDebug(IS,"playThread:",ToHEX(mConnectID),"->->before flush");
        if (ah_simple_flush(mpAHHandler) < 0) {
            logError(IS,"playThread:",ToHEX(mConnectID),"->->ah_simple_flush() failed");
            return;
        }
        logDebug(IS,"playThread:",ToHEX(mConnectID),"->->after flush");

        logDebug(IS,"playThread:",ToHEX(mConnectID),"->->before drain");
        if (ah_simple_drain(mpAHHandler) < 0) {
            logError(IS,"playThread:",ToHEX(mConnectID),"->->ah_simple_drain() failed");
            return;
        }
        logDebug(IS,"playThread:",ToHEX(mConnectID),"->->after drain");

        mMutexOfAHHandler.unlock();
    }

    logVerbose(IS,"PlaySoundThread::drain()-->OUT");
}

bool PlaySoundThread::isSuccessToCreateAHHandler(void)
 {
     logVerbose(IS,"PlaySoundThread::isSuccessToCreateAHHandler()-->IN");

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

         logDebug(IS,"wav fileName:",mPlayFilePath);
         logDebug(IS,"wav.file_size:",wav.file_size," wav.channel:",wav.channel," wav.frequency:",wav.frequency);
         logDebug(IS,"wav.Bps:",wav.Bps," wav.sample_num_bit:",wav.sample_num_bit," wav.data_size:",wav.data_size);

         struct stat buf;
         stat(mPlayFilePath.c_str(),&buf);
         int realFileSize = buf.st_size;
         int realDataSize = realFileSize - 44;
         logDebug(IS,"realFileSize:",realFileSize);
         logDebug(IS,"realDataSize:",realDataSize);


         mFileDataSize = wav.data_size;

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
         mpAHHandler = ah_simple_new(mStreamName.c_str(), AH_STREAM_PLAYBACK, &ss);
         mMutexOfAHHandler.unlock();
         if (!mpAHHandler) {
             logWarn(IS,"ah_simple_new() failed error");
             return false;
         }
     }else{
         return false;
     }

     logVerbose(IS,"PlaySoundThread::isSuccessToCreateAHHandler()-->OUT");

     return true;
 }

void PlaySoundThread::playOnce()
{
    logVerbose(IS,"playThread:",ToHEX(mConnectID),"->->PlaySoundThread::playOnce()-->IN");

    int fd;

    /* replace STDIN with the specified file if needed */
    logInfo(IS,"playThread:",ToHEX(mConnectID),"->->mPlayFilePath:",mPlayFilePath);
    if (mPlayFilePath != "") {
        if ((fd = open(mPlayFilePath.c_str(), O_RDONLY)) < 0) {
            logError(IS,"playThread:",ToHEX(mConnectID),"->->open play file failed");
            return;
        }
        lseek(fd,44,SEEK_SET);
    }else{
        logError(IS,"playThread:",ToHEX(mConnectID),"->->havn't set playFile");
        return;
    }

    if(!mpAHHandler){
        logWarn(IS,"playThread:",ToHEX(mConnectID),"->->mpAHHandler==nullptr,try to create mpAHHandler again!");
        if(!isSuccessToCreateAHHandler()){
            logError(IS,"playThread:",ToHEX(mConnectID),"->->create mpAHHandler failed when we play");
        }
    }

    logDebug(IS,"playThread:",ToHEX(mConnectID),"->->play sound start");
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
        logDebug(IS,"playThread:",ToHEX(mConnectID),"->->playOnce()-->before ah_simple_write()");
        ret = ah_simple_write(mpAHHandler, buf, (size_t) r);
        logDebug(IS,"ret:",ret);
        logDebug(IS,"playThread:",ToHEX(mConnectID),"->->playOnce()-->after ah_simple_write()");
        mMutexOfAHHandler.unlock();
        if(ret==0){
            logDebug(IS,"write all data successed!");
        }else{
            logError(IS,"play playFile in paluseAudio failed");
            return;
        }
    }else{
        logError(IS,"playThread:",ToHEX(mConnectID),"->->mpAHHandler == nullptr");
        return;
    }

    logVerbose(IS,"playThread:",ToHEX(mConnectID),"->->PlaySoundThread::playOnce()-->OUT");
}

void PlaySoundThread::requestToPlay(void)
{
    logVerbose(IS,"PlaySoundThread::requestToPlay()-->IN");
    stopPlay();
    mpAudioStream->requestAudioFocusForDev(GET_DEVICETYPE_FROM_CONNECTID(mConnectID));
    logVerbose(IS,"PlaySoundThread::requestToPlay()-->OUT");
}

void PlaySoundThread::requestToPlayAlways()
{
    logVerbose(IS,"PlaySoundThread::requestToPlayAlways()-->IN");
    stopPlay();
    mIsPlayOnce = false;
    mpAudioStream->requestAudioFocusForDev(GET_DEVICETYPE_FROM_CONNECTID(mConnectID));
    logVerbose(IS,"PlaySoundThread::requestToPlayAlways()-->OUT");
}

void PlaySoundThread::requestToStopPlay()
{
    logVerbose(IS,"PlaySoundThread::requestToStopPlay()-->IN");
    stopPlay();
    mpAudioStream->abandonAudioFocusForDev(GET_DEVICETYPE_FROM_CONNECTID(mConnectID));
    logVerbose(IS,"PlaySoundThread::requestToStopPlay()-->OUT");
}

void PlaySoundThread::OnGainAudioFocus()
{
    logVerbose(IS,"PlaySoundThread::OnGainAudioFocus()-->IN");

    // wake thread to play file
    startPlay();

    logVerbose(IS,"PlaySoundThread::OnGainAudioFocus()-->OUT");
}

void PlaySoundThread::OnLossAudioFocus()
{
    logVerbose(IS,"PlaySoundThread::OnGainAudioFocus()-->IN");

    stopPlay();

    logVerbose(IS,"PlaySoundThread::OnGainAudioFocus()-->OUT");
}
