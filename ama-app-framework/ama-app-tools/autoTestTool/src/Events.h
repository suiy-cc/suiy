/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef EVENTS_H_
#define EVENTS_H_

#include "exceptionDef.h"
#include "Event.h"

#include <vector>
#include <string>
#include <memory>
#include <thread>

#include <time.h>
#include <linux/input.h>

using namespace std;

namespace ama
{

class DBHandler;
class TestCase;

class Events
{
public:
    Events();
    virtual ~Events();

    friend class DBHandler;
    friend class TestCase;

    void saveAs(string filePath);
    void loadFrom(string filePath);

    void print() throw(ama_exceptionType_e);
    void addEvent(shared_ptr<Event> pEvent);
protected:
    // void addEvent(shared_ptr<Event> pEvent);
    int size(void){
        return mEventVector.size();
    }
    bool isEmpty(void){
        return (mEventVector.size()<2);
    }
    void clear(void){
        mEventVector.clear();
    }
protected:
    std::vector<shared_ptr<Event>> mEventVector;
};//end Events



class TouchEvents : public Events
{
public:
    TouchEvents();
    virtual ~TouchEvents();

    friend class DBHandler;

    void play(std::function<void (void)> playOverCb)throw(ama_exceptionType_e);
    void stopPlay(void)throw(ama_exceptionType_e);
    void record(void)throw(ama_exceptionType_e);
    void stopRecord(void)throw(ama_exceptionType_e);
    void updateRelativeTimeStampForEvents(void);
private:
    bool isGetNextActionSuccessed(struct input_event *pEvents,int &eventCnt,int &playingIndex,uint32_t &recordingTime) throw(ama_exceptionType_e);
    bool isOperating(){
        return (mIsPlaying||mIsRecording);
    }
    void playRunning(void);
    void recordRunning(void);
private:
    bool mIsPlaying;
    bool mIsRecording;
    std::shared_ptr<std::thread> mOperateThread;
    std::function<void (void)> mPlayOverCallback;
};//end TouchEvents

class TestEvents : public Events
{
public:
    TestEvents();
    virtual ~TestEvents();

    friend class DBHandler;
    friend class TestCase;

    void play(void)throw(ama_exceptionType_e);
    void stopPlay(void)throw(ama_exceptionType_e);
    void record(string filter="")throw(ama_exceptionType_e);
    void stopRecord(void)throw(ama_exceptionType_e);
    void handleNewTestEvent(shared_ptr<Event> pEvent);
    void updateRelativeTimeStampForEvents(void);
    void generateReport(TestEvents& receiveTestEvents,string reportFilePath="");
    bool isAbleFindResultEvent(int &startIndex,
                            shared_ptr<TestEvent> &pExpectEvent,shared_ptr<TestEvent> &pResultEvent);
    void printWithEx(string ex);
private:
    bool isOperating(){
        return (mIsPlaying||mIsRecording);
    }
    void printExpectationRunning(void);
private:
    bool mIsPlaying;
    bool mIsRecording;
    string mFilter;
    std::shared_ptr<std::thread> mOperateThread;
};//end TestEvents

}//end namespace ama

#endif //EVENTS_H_
