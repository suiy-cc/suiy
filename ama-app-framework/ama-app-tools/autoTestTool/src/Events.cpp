/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "Events.h"
#include "InputEvent.h"
#include "DBHandler.h"

#include "AppBroadcast.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <fstream>

namespace ama
{

/******************************************************************************/
/***************************|begin of Events|**********************************/
/******************************************************************************/
Events::Events()
{

}

Events::~Events()
{

}

void Events::saveAs(string filePath)
{
    DBHandler::GetInstance()->saveEventsAs(this,filePath);
}

void Events::loadFrom(string filePath)
{
    DBHandler::GetInstance()->getEventsFrom(this,filePath);
}

void Events::print() throw(ama_exceptionType_e)
{
    if(isEmpty()){
        throw E_NO_RECORD;
    }

    printf("events.size:%d \n",size());
    for(auto it=mEventVector.begin();it!=mEventVector.end();it++){
        (*it)->print();
    }
}

void Events::addEvent(shared_ptr<Event> pEvent)
{
    if((E_FLAG_MARK==pEvent->getEventFlag())&&(dynamic_pointer_cast<MarkEvent>(pEvent)->isBeginMarkEvent())){
        mEventVector.clear();
        mEventVector.push_back(pEvent);
    }else{
        mEventVector.push_back(pEvent);
    }
}
/******************************************************************************/
/*****************************|end of Events|**********************************/
/******************************************************************************/

/******************************************************************************/
/***************************|begin of TouchEvents|*****************************/
/******************************************************************************/
TouchEvents::TouchEvents()
:mIsPlaying(false)
,mIsRecording(false)
,mOperateThread(nullptr)
{

}

TouchEvents::~TouchEvents()
{

}

void TouchEvents::play(std::function<void (void)> playOverCb)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsPlaying){
            throw E_PLAYING;
        }else{
            throw E_RECORDING;
        }
    }else{
        mIsPlaying = true;
        mPlayOverCallback = playOverCb;

        if(mOperateThread){
            mOperateThread = nullptr;
        }

        mOperateThread = std::make_shared<std::thread>(std::bind(&TouchEvents::playRunning,this));
        mOperateThread->detach();
    }
}

void TouchEvents::stopPlay(void)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsPlaying){
            mIsPlaying = false;
        }else{
            throw E_RECORDING;
        }
    }else{
        throw E_NOT_OPERATING;
    }

    if(!mIsPlaying){
        throw E_NOT_PLAYING;
    }else{
        mIsPlaying = false;
        if(mOperateThread->joinable()){
            mOperateThread->join();
        }
        mOperateThread = nullptr;
    }
}

void TouchEvents::record(void)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsPlaying){
            throw E_PLAYING;
        }else{
            throw E_RECORDING;
        }
    }else{
        mIsRecording = true;
    }

    if(mOperateThread){
        mOperateThread->detach();
        mOperateThread = nullptr;
    }
    mOperateThread = std::make_shared<std::thread>(std::bind(&TouchEvents::recordRunning,this));
}

void TouchEvents::stopRecord(void)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsPlaying){
            throw E_PLAYING;
        }else{
            mIsRecording = false;
        }
    }else{
        throw E_NOT_OPERATING;
    }

    mOperateThread->join();
    mOperateThread = nullptr;
    updateRelativeTimeStampForEvents();
}

bool TouchEvents::isGetNextActionSuccessed(struct input_event *pEvents,int &eventCnt,int &playingIndex,uint32_t &recordingTime) throw(ama_exceptionType_e)
{
    if(isEmpty()){
        throw E_NO_RECORD;
        return false;
    }

    eventCnt = 0;

    if(E_FLAG_MARK==mEventVector.at(playingIndex)->getEventFlag()){
        if(dynamic_pointer_cast<MarkEvent>(mEventVector.at(playingIndex))->isBeginMarkEvent()){
            //TODO:nothing
            playingIndex++;
        }else{
            playingIndex = 0;
            return false;
        }
    }

    while(E_FLAG_TOUCH==mEventVector.at(playingIndex)->getEventFlag()){
        if(dynamic_pointer_cast<TouchEvent>(mEventVector.at(playingIndex))->isSyncOfTouchType()){
            recordingTime =  dynamic_pointer_cast<TouchEvent>(mEventVector.at(playingIndex))->getTimeStamp();
            *pEvents = dynamic_pointer_cast<TouchEvent>(mEventVector.at(playingIndex))->getTouchEvent();
            pEvents++;
            playingIndex++;
            eventCnt++;
            return true;
        }else{
            *pEvents = dynamic_pointer_cast<TouchEvent>(mEventVector.at(playingIndex))->getTouchEvent();
            pEvents++;
            playingIndex++;
            eventCnt++;
        }
    }

    return false;
}

void TouchEvents::playRunning(void)
{
    int           fd = 0;

    if ((fd = open(TOUCH_DEVICE, O_RDWR, 0)) >= 0) {
        // printf("%s: open, fd = %d\n", TOUCH_DEVICE, fd);
        uint32_t playStartTime = EventTimeStamp::GetStamp_s();
        uint32_t now = EventTimeStamp::GetStamp_ms();
        uint32_t playingTime = now - playStartTime;
        uint32_t recordingTime;
        struct input_event eventAry[10];
        int eventCnt = 0;
        int playingIndex = 0;

        while(mIsPlaying){
            if(0==eventCnt){
                //read events of one action
                // printf("read a action event==>>\n");
                try{
                    if(!isGetNextActionSuccessed(eventAry,eventCnt,playingIndex,recordingTime)){
                        mIsPlaying = false;
                        mPlayOverCallback();
                        printf("play all touch events  \n");
                        return;
                    }
                }catch(ama_exceptionType_e e){
                    // printf("read stop event==>>\n");
                    printException(e);
                    mIsPlaying = false;
                    return;
                }catch(...){
                    mIsPlaying = false;
                    return;
                }
            }else{
                //check time
                now = EventTimeStamp::GetStamp_ms();
                playingTime = now - playStartTime;

                //play
                if(playingTime >= recordingTime){
                    // printf("write a action event==>>\n");
                    struct timeval time;
                    gettimeofday(&time,0);
                    for(int i=0;i<eventCnt;i++){
                        eventAry[i].time = time;
                        // printEvent(eventAry[i]);
                        if(write(fd,&eventAry[i],sizeof(input_event))<0){
                            printf("write input event error! \n");
                        }
                    }
                    eventCnt = 0;
                }else{
                    usleep(10);
                }
            }
        }
        close(fd);
    }else{
        printf("open touch device:%s failed! \n", TOUCH_DEVICE);
    }
}

void TouchEvents::recordRunning(void)
{
    int           fd = 0;
    struct input_event event;

    if ((fd = open(TOUCH_DEVICE, O_RDWR | O_NONBLOCK, 0)) >= 0) {
        printf("%s: open, fd = %d\n", TOUCH_DEVICE, fd);

        shared_ptr<MarkEvent> pBeginEvent = std::make_shared<MarkEvent>(E_MARK_BEGIN);
        uint32_t startTime = pBeginEvent->getTimeStamp();
        addEvent(pBeginEvent);
        while(mIsRecording){
            if(0 < read(fd, &event, sizeof(event))){
                if(!mIsRecording){
                    break;
                }
                addEvent(std::make_shared<TouchEvent>(event));
                // TouchEvent(event).print();
                if(!mIsRecording){
                    break;
                }
            }else{
                usleep(10);
            }
        }
        addEvent(std::make_shared<MarkEvent>(E_MARK_END));
        close(fd);
    }else{
        printf("open touch device:%s failed! \n", TOUCH_DEVICE);
    }
}

void TouchEvents::updateRelativeTimeStampForEvents(void)
{
    uint32_t startTime = mEventVector.at(0)->getTimeStamp();
    for(auto it=mEventVector.begin();it!=mEventVector.end();it++){
        (*it)->updateTimeStampAsRelative(startTime);
    }
}

/******************************************************************************/
/*****************************|end of TouchEvents|*****************************/
/******************************************************************************/

/******************************************************************************/
/***************************|begin of TestEvents|******************************/
/******************************************************************************/
std::function<void (shared_ptr<Event>)> sHandleNewTestEventFun;

static void handleBroadcast(std::string event,std::string data){
    sHandleNewTestEventFun(std::make_shared<TestEvent>(event,data));
}


TestEvents::TestEvents()
:mIsPlaying(false)
,mIsRecording(false)
,mOperateThread(nullptr)
{

}

TestEvents::~TestEvents()
{

}

void TestEvents::play(void) throw(ama_exceptionType_e)
{
    if(isEmpty()){
        throw E_NO_EVENTS;
    }

    if(isOperating()){
        if(mIsPlaying){
            throw E_PLAYING;
        }else{
            throw E_RECORDING;
        }
    }else{
        mIsPlaying = true;

        if(mOperateThread){
            mOperateThread = nullptr;
        }

        mOperateThread = std::make_shared<std::thread>(std::bind(&TestEvents::printExpectationRunning,this));
        mOperateThread->detach();
    }
}

void TestEvents::stopPlay(void)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsPlaying){
            mIsPlaying = false;

            mOperateThread->join();
            mOperateThread = nullptr;
        }else{
            throw E_PLAYING;
        }
    }else{
        throw E_NOT_OPERATING;
    }
}

void TestEvents::record(string filter)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsRecording){
            throw E_PLAYING;
        }else{
            throw E_RECORDING;
        }
    }else{
        addEvent(std::make_shared<MarkEvent>(E_MARK_BEGIN));
        printf("receive<=");mEventVector.at(0)->print();
        mFilter = filter;
        mIsRecording = true;
        sHandleNewTestEventFun = std::bind(&TestEvents::handleNewTestEvent,this,std::placeholders::_1);
        listenBroadcast(handleBroadcast);
    }
}

void TestEvents::stopRecord(void)throw(ama_exceptionType_e)
{
    if(isOperating()){
        if(mIsRecording){
            mIsRecording = false;
            addEvent(std::make_shared<MarkEvent>(E_MARK_END));
            updateRelativeTimeStampForEvents();
        }else{
            throw E_PLAYING;
        }
    }else{
        throw E_NOT_OPERATING;
    }
}

void TestEvents::handleNewTestEvent(shared_ptr<Event> pEvent)
{
    if(isOperating()){
        if(dynamic_pointer_cast<TestEvent>(pEvent)->isValidByFilter("autoTest")){
            if(mIsPlaying){
                //do nothing
            }else if(mIsRecording){
                if(dynamic_pointer_cast<TestEvent>(pEvent)->isValidByFilter(mFilter)){
                    addEvent(pEvent);
                    printf("receive<=");pEvent->print();
                }
            }
        }
    }
}

void TestEvents::printExpectationRunning(void)
{
    uint32_t playStartTime = EventTimeStamp::GetStamp_s();
    uint32_t now = EventTimeStamp::GetStamp_ms();
    uint32_t playingTime = now - playStartTime;
    uint32_t recordingTime = 0;
    shared_ptr<TestEvent> pTestEvent;
    int index =1;

    while(mIsPlaying){
        if(0==recordingTime){
            if(E_FLAG_MARK==mEventVector.at(index)->getEventFlag()){
                if(dynamic_pointer_cast<MarkEvent>(mEventVector.at(index))->isEndMarkEvent()){
                    printf("print expection over!\n");
                    mIsPlaying = false;
                }
            }else if(E_FLAG_TEST==mEventVector.at(index)->getEventFlag()){
                pTestEvent = dynamic_pointer_cast<TestEvent>(mEventVector.at(index));
                recordingTime = pTestEvent->getTimeStamp();
                index++;
            }
        }else{
            //check time
            now = EventTimeStamp::GetStamp_ms();
            playingTime = now - playStartTime;

            //play
            if(playingTime >= recordingTime){
                printf("expect<=");pTestEvent->print();
                recordingTime = 0;
            }else{
                usleep(10);
            }
        }
    }
}

void TestEvents::generateReport(TestEvents& receiveTestEvents,string reportFilePath)
{
    if(reportFilePath==""){
        reportFilePath = "/tmp/tmpAutoTestReport";
    }

    //if file exist,remove it!
    if(0 == (access(reportFilePath.c_str(),F_OK))){
        if(0!=remove(reportFilePath.c_str())){
            printf("remove %s failed! \n",reportFilePath.c_str());
            return;
        }
    }

    //write result into report file
    std::ofstream ofs;
    ofs.open(reportFilePath.c_str());
    if(ofs.is_open())
    {
        shared_ptr<TestEvent> pExpectEvent,pResultEvent;
        int searchIndex = 0;
        for(auto it=mEventVector.begin();it!=mEventVector.end();it++){
            if(E_FLAG_TEST==(*it)->getEventFlag()){
                pExpectEvent = dynamic_pointer_cast<TestEvent>(*it);
                if(receiveTestEvents.isAbleFindResultEvent(searchIndex,pExpectEvent,pResultEvent)){
                    ofs << "expect :"<< pExpectEvent->getInfoStr() << std::endl;
                    ofs << "receive:"<< pResultEvent->getInfoStr() << std::endl;
                    ofs << "result :<OK>" << std::endl;
                }else{
                    ofs << "expect :"<< pExpectEvent->getInfoStr() << std::endl;
                    ofs << "receive:Null"<< std::endl;
                    ofs << "result :<NG>" << std::endl;
                }
            }
        }
        ofs.close();
    }
}

bool TestEvents::isAbleFindResultEvent(int &startIndex,
                        shared_ptr<TestEvent> &pExpectEvent,shared_ptr<TestEvent> &pResultEvent)
{
    if(startIndex>=mEventVector.size()){
        return false;
    }

    int restoreIndex = startIndex;

    uint32_t minTime = pExpectEvent->getTimeStamp()>(1<<10)?(pExpectEvent->getTimeStamp()-(1<<10)):0;
    uint32_t maxTime = pExpectEvent->getTimeStamp()+(1<<10);
    while(startIndex<mEventVector.size()){
        if(E_FLAG_TEST==mEventVector.at(startIndex)->getEventFlag()){
            pResultEvent = dynamic_pointer_cast<TestEvent>(mEventVector.at(startIndex));
            if(pResultEvent->getTimeStamp()<minTime){
                startIndex++;
            }else if(pResultEvent->getTimeStamp()<maxTime){//try to find right result in <minTime,maxTime>
                if(pExpectEvent->getFunDescriptionStr()==pResultEvent->getFunDescriptionStr()){
                    startIndex++;
                    return true;
                }else{
                    startIndex++;
                }
            }else{//can't find right result in <maxTime,00>
                startIndex = restoreIndex;
                return false;
            }
        }else{
            startIndex++;
        }
    }

    startIndex = restoreIndex;
    return false;
}

void TestEvents::updateRelativeTimeStampForEvents(void)
{
    uint32_t startTime = mEventVector.at(0)->getTimeStamp();
    for(auto it=mEventVector.begin();it!=mEventVector.end();it++){
        (*it)->updateTimeStampAsRelative(startTime);
    }
}

void TestEvents::printWithEx(string ex)
{
    shared_ptr<TestEvent> pEvent;
    for(auto it=mEventVector.begin();it!=mEventVector.end();it++){
        if(E_FLAG_TEST==(*it)->getEventFlag()){
            pEvent = dynamic_pointer_cast<TestEvent>(*it);
            std::cout << ex << pEvent->getInfoStr() << std::endl;
        }
    }
}

/******************************************************************************/
/*****************************|end of TestEvents|******************************/
/******************************************************************************/

}//end namespace ama
