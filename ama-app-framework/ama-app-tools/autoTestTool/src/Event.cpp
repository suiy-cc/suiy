/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "Event.h"

#include <stdint.h>
#include <stdio.h>

namespace ama
{

uint32_t EventTimeStamp::GetStamp_s()
{
    timespec ts = {0,0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint32_t stamp_sec = ts.tv_sec;
    uint32_t stamp_nsec = ts.tv_nsec;
    return (stamp_sec<<10);
}

uint32_t EventTimeStamp::GetStamp_ms()
{
    timespec ts = {0,0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint32_t stamp_sec = ts.tv_sec;
    uint32_t stamp_nsec = ts.tv_nsec;
    return ((stamp_sec<<10)|(stamp_nsec>>20));
}

Event::Event()
:mEventFlag(E_FLAG_NULL)
,mTimeStamp(0)
{

}

Event::~Event()
{

}

void Event::print(void)
{
    printf("eventFlag:%s;",getEventFlagStr().c_str());
    printf("timeStamp:%d \n",mTimeStamp);
}

uint32_t Event::getTimeStamp(void)
{
    return mTimeStamp;
}

void Event::updateTimeStamp_s(void)
{
    mTimeStamp = EventTimeStamp::GetStamp_s();
}

void Event::updateTimeStamp_ms(void)
{
    mTimeStamp = EventTimeStamp::GetStamp_ms();
}

void Event::updateTimeStampAsRelative(uint32_t startTime)
{
    mTimeStamp = mTimeStamp - startTime;
}

ama_EventFlag_e Event::getEventFlag(void)
{
    return mEventFlag;
}

void Event::setEventFlag(ama_EventFlag_e eventFlag)
{
    mEventFlag = eventFlag;
}

std::string Event::getEventFlagStr(void)
{
    switch(mEventFlag){
    case  E_FLAG_NULL:  return string("E_FLAG_NULL");
    case  E_FLAG_MARK:  return string("E_FLAG_MARK");
    case  E_FLAG_TOUCH:  return string("E_FLAG_TOUCH");
    case  E_FLAG_TEST:  return string("E_FLAG_TEST");
    default:    return string("NotDefinedFlag");
    }
}

/******************************************************************************/
/***************************|begin of MarkEvent|*******************************/
/******************************************************************************/
MarkEvent::MarkEvent()
{
    setEventFlag(E_FLAG_MARK);
    updateTimeStamp_s();
}

MarkEvent::MarkEvent(ama_MarkType_e maskType)
:mMaskType(maskType)
{
    setEventFlag(E_FLAG_MARK);
    updateTimeStamp_s();
}

MarkEvent::~MarkEvent()
{

}

void MarkEvent::print(void)
{
    printf("eventFlag:%s;",getEventFlagStr().c_str());
    printf("maskType:%d;",mMaskType);
    printf("timeStamp:%d \n",getTimeStamp());
}

bool MarkEvent::isBeginMarkEvent(void)
{
    return (E_MARK_BEGIN==mMaskType);
}

bool MarkEvent::isEndMarkEvent(void)
{
    return (E_MARK_END==mMaskType);
}
/******************************************************************************/
/*****************************|end of MarkEvent|*******************************/
/******************************************************************************/



/******************************************************************************/
/***************************|begin of TouchEvent|******************************/
/******************************************************************************/
TouchEvent::TouchEvent()
{
    setEventFlag(E_FLAG_TOUCH);
    //set all 0 in stuct input_event
    mTouchEvent.time.tv_sec = 0;
    mTouchEvent.time.tv_usec = 0;
    mTouchEvent.type = EV_MAX;
    mTouchEvent.code = 0;
    mTouchEvent.value = 0;
}

TouchEvent::TouchEvent(input_event touchEvent)
:mTouchEvent(touchEvent)
{
    setEventFlag(E_FLAG_TOUCH);
    //we need update sync event time only
    if(EV_SYN==mTouchEvent.type){
        updateTimeStamp_ms();
    }
    mTouchEvent.time.tv_sec = 0;
    mTouchEvent.time.tv_usec = 0;
}

TouchEvent::~TouchEvent()
{

}

void TouchEvent::print(void)
{
    string tmp;

    printf("eventFlag:%s;",getEventFlagStr().c_str());
    printf("timeStamp:%d;",getTimeStamp());
    printf("type 0x%04x; code 0x%04x; value 0x%08x;",
           mTouchEvent.type, mTouchEvent.code, mTouchEvent.value);

    switch (mTouchEvent.type) {
    case EV_SYN:
        printf("SYNC mTouchEvent");
        break;
    case EV_KEY:
        if (mTouchEvent.code > BTN_MISC) {
            printf("Button %d %s",
                   mTouchEvent.code & 0xff,
                   mTouchEvent.value ? "press" : "release");
        } else {
            printf("Key %d (0x%x) %s",
                   mTouchEvent.code & 0xff,
                   mTouchEvent.code & 0xff,
                   mTouchEvent.value ? "press" : "release");
        }
        break;
    case EV_REL:
        switch (mTouchEvent.code) {
        case REL_X:      tmp = "X";       break;
        case REL_Y:      tmp = "Y";       break;
        case REL_HWHEEL: tmp = "HWHEEL";  break;
        case REL_DIAL:   tmp = "DIAL";    break;
        case REL_WHEEL:  tmp = "WHEEL";   break;
        case REL_MISC:   tmp = "MISC";    break;
        default:         tmp = "UNKNOWN"; break;
        }
        printf("Relative %s %d", tmp.c_str(), mTouchEvent.value);
        break;
    case EV_ABS:
        switch (mTouchEvent.code) {
        case ABS_X:        tmp = "X";        break;
        case ABS_Y:        tmp = "Y";        break;
        case ABS_Z:        tmp = "Z";        break;
        case ABS_RX:       tmp = "RX";       break;
        case ABS_RY:       tmp = "RY";       break;
        case ABS_RZ:       tmp = "RZ";       break;
        case ABS_THROTTLE: tmp = "THROTTLE"; break;
        case ABS_RUDDER:   tmp = "RUDDER";   break;
        case ABS_WHEEL:    tmp = "WHEEL";    break;
        case ABS_GAS:      tmp = "GAS";      break;
        case ABS_BRAKE:    tmp = "BRAKE";    break;
        case ABS_HAT0X:    tmp = "HAT0X";    break;
        case ABS_HAT0Y:    tmp = "HAT0Y";    break;
        case ABS_HAT1X:    tmp = "HAT1X";    break;
        case ABS_HAT1Y:    tmp = "HAT1Y";    break;
        case ABS_HAT2X:    tmp = "HAT2X";    break;
        case ABS_HAT2Y:    tmp = "HAT2Y";    break;
        case ABS_HAT3X:    tmp = "HAT3X";    break;
        case ABS_HAT3Y:    tmp = "HAT3Y";    break;
        case ABS_PRESSURE: tmp = "PRESSURE"; break;
        case ABS_DISTANCE: tmp = "DISTANCE"; break;
        case ABS_TILT_X:   tmp = "TILT_X";   break;
        case ABS_TILT_Y:   tmp = "TILT_Y";   break;
        case ABS_MISC:     tmp = "MISC";     break;
        case ABS_MT_POSITION_X: tmp = "Center X Touch pos";break;
        case ABS_MT_POSITION_Y: tmp = "Center Y Touch pos";break;
        case ABS_MT_TRACKING_ID: tmp = "Unique ID of initiated contact";break;
        default:           tmp = "UNKNOWN";  break;
        }
        printf("Absolute %s %d", tmp.c_str(), mTouchEvent.value);
        break;
    case EV_MSC: printf("Misc"); break;
    case EV_LED: printf("Led");  break;
    case EV_SND: printf("Snd");  break;
    case EV_REP: printf("Rep");  break;
    case EV_FF:  printf("FF");   break;
        break;
    }
    printf("\n");
}

input_event TouchEvent::getTouchEvent(void)
{
    return mTouchEvent;
}

int TouchEvent::getTouchType(void)
{
    return mTouchEvent.type;
}

bool TouchEvent::isSyncOfTouchType(void)
{
    return (EV_SYN==mTouchEvent.type);
}
/******************************************************************************/
/*****************************|end of TouchEvent|******************************/
/******************************************************************************/

/******************************************************************************/
/***************************|begin of TestEvent|*******************************/
/******************************************************************************/

TestEvent::TestEvent()
:mFunDescriptionStr("")
,mDataStr("")
{
    setEventFlag(E_FLAG_TEST);
}

TestEvent::TestEvent(string funDescriptionStr,string dataStr)
:mFunDescriptionStr(funDescriptionStr)
,mDataStr(dataStr)
{
    setEventFlag(E_FLAG_TEST);
    updateTimeStamp_ms();
}
TestEvent::~TestEvent()
{

}

void TestEvent::print(void)
{
    printf("eventFlag:%s;",getEventFlagStr().c_str());
    printf("timeStamp:%d;",getTimeStamp());
    printf("funDescriptionStr:%s; dataStr:%s;\n",mFunDescriptionStr.c_str(),mDataStr.c_str());
}

bool TestEvent::isValidByFilter(string filter)
{
    if(""==filter){
        return true;
    }else{
        if(mFunDescriptionStr.find(filter)!=std::string::npos){
            return true;
        }else{
            return false;
        }
    }
}

/******************************************************************************/
/*****************************|end of TestEvent|*******************************/
/******************************************************************************/

}//end namespace ama
