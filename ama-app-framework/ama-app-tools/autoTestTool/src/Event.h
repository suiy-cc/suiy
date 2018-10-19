/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef EVENT_H_
#define EVENT_H_

#include "exceptionDef.h"

#include <vector>
#include <string>
#include <memory>

#include <time.h>
#include <linux/input.h>

using namespace std;

namespace ama
{

class DBHandler;

class EventTimeStamp
{
public:
    static uint32_t GetStamp_ms();
    static uint32_t GetStamp_s();
};

enum ama_EventFlag_e{
    E_FLAG_NULL = 0x00,
    E_FLAG_MARK = 0x01,
    E_FLAG_TOUCH = 0x02,
    E_FLAG_TEST = 0x03,
};

enum ama_MarkType_e{
    E_MARK_BEGIN = 0x00,
    E_MARK_END = 0x01,
};

class Event
{
public:
    Event();
    ~Event();

    friend class DBHandler;

    virtual void print(void);

    uint32_t getTimeStamp(void);
    void updateTimeStamp_s(void);
    void updateTimeStamp_ms(void);
    void updateTimeStampAsRelative(uint32_t startTime);

    ama_EventFlag_e getEventFlag(void);
    void setEventFlag(ama_EventFlag_e eventFlag);

    std::string getEventFlagStr(void);
private:
    ama_EventFlag_e mEventFlag;
    int32_t mTimeStamp;
};//end Event

class MarkEvent : public Event
{
public:
    MarkEvent();
    MarkEvent(ama_MarkType_e maskType);
    ~MarkEvent();

    friend class DBHandler;

    void print(void);

    bool isBeginMarkEvent(void);
    bool isEndMarkEvent(void);
private:
    //mTimeStamp is absolute time (from system startup)
    ama_MarkType_e mMaskType;
};//end MarkEvent

class TouchEvent : public Event
{
public:
    TouchEvent();
    TouchEvent(input_event touchEvent);
    ~TouchEvent();

    friend class DBHandler;

    void print(void);

    input_event getTouchEvent(void);
    int getTouchType(void);
    bool isSyncOfTouchType(void);
private:
    input_event mTouchEvent;
};//end TouchEvent

class TestEvent : public Event
{
public:
    TestEvent();
    TestEvent(string funDescriptionStr,string dataStr);
    ~TestEvent();

    friend class DBHandler;

    void print(void);

    bool isValidByFilter(string filter);
    string getFunDescriptionStr(void){
        return mFunDescriptionStr;
    }
    string getInfoStr(void){
        string info = string("TimeStamp[")+std::to_string(getTimeStamp())+string("];funDescription[")\
        +mFunDescriptionStr+string("];data[")+mDataStr+string("];");
        return info;
    }
private:
    string mFunDescriptionStr;
    string mDataStr;
};//end TestEvent

}//end namespace ama



#endif //EVENT_H_
