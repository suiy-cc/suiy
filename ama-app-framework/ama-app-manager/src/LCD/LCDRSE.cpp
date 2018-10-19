/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "LCDRSE.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

LCDRSE::
LCDRSE(){}
LCDRSE::LCDRSE(E_SEAT seatid)
{
    logVerbose(LCDM,"LCDRSE::LCDRSE--> IN!");

    m_inputDev=NULL;
    m_seat=seatid;
    m_isRSE=true;
    m_ReadyCheckCnt=0;
    m_TimerEnableCount=0;
    m_timerEnable=true;
    m_isInputReady=false;
    m_ScreenOnTimeCnt=0;
    m_TimeToCloseLCD=GetRearTimerToCloseLCD()*(1000/RSE_DETECT_STEP/LCDM_USLEEP_INTERVAL);
    logVerbose(LCDM,"LCDRSE::LCDRSE--> OUT!");

}

LCDRSE::~LCDRSE()
{
    logVerbose(LCDM,"LCDRSE::~LCDRSE--> IN!");
    if (NULL!=m_inputDev)
    {
        delete(m_inputDev);
    }
    logVerbose(LCDM,"LCDRSE::~LCDRSE--> OUT!");
}
InputDevice *LCDRSE::GetInputDev() const
{
    return m_inputDev;
}

void LCDRSE::SetInputDev(InputDevice *inputDev)
{
    m_inputDev = inputDev;
}

REAR_SCREEN_OPERATE LCDRSE::CheckPress()
{
   // logVerbose(LCDM,"LCDRSE::CheckPress--> IN!");
    REAR_SCREEN_OPERATE operate=KEEP_SCREEN;
    if (NULL==m_inputDev)
    {
        m_inputDev=new InputDevice(this->GetInputDevPath());
        logDebug(LCDM,"LCDRSE:CheckPress() GetInputDevPath::",std::string(GetInputDevPath()));

      //  logVerbose(LCDM,"LCDRSE::CheckPress--> OUT!  operate: KEEP_SCREEN");
        return KEEP_SCREEN;
    }
    bool retValue =(bool)( GetInputDev()->checkPress());
    if (GetTimerCntFlag())//timer enable
    {
        m_ScreenOnTimeCnt++;
        //
        if (m_ScreenOnTimeCnt>=100000)
            m_ScreenOnTimeCnt=0;


       // logInfoF(LCDM," LCDRSE::CheckPressm_TimeToCloseLCD:%d m_ScreenOnTimeCnt:%d ",m_TimeToCloseLCD,m_ScreenOnTimeCnt);
        if(retValue)//is pressed : screen on
        {
            m_ScreenOnTimeCnt=0;
            operate = TURN_ON_SCREEN;
            logDebug(LCDM,"GetTimerCntFlag:",GetTimerCntFlag(),"  m_ScreenOnTimeCnt:",m_ScreenOnTimeCnt," retValue:",retValue);
        }
       // else if (m_ScreenOnTimeCnt==TIME_TO_CLOSE_SCREEN)//no press && >30s: off
        else if (m_ScreenOnTimeCnt==m_TimeToCloseLCD)//no press && >30s: off
        {
            operate = TURN_OFF_SCREEN;
            logDebug(LCDM,"LCDRSE::CheckPress GetTimerCntFlag:",GetTimerCntFlag(),"  m_ScreenOnTimeCnt:",m_ScreenOnTimeCnt," retValue:",retValue);
        }

        else//no press && !=TIME_TO_CLOSE_SCREEN : keep
        {
            operate = KEEP_SCREEN;
        //    logDebug(LCDM,"LCDRSE::CheckPress no press && !=TIME_TO_CLOSE_SCREEN : keep");

        }
    }
    else
    {
        m_ScreenOnTimeCnt = 0;
    //    logDebug(LCDM,"LCDRSE::CheckPress GetTimerCntFlag:",GetTimerCntFlag(),"  m_ScreenOnTimeCnt:",m_ScreenOnTimeCnt);
        //  operate = retValue ? TURN_ON_SCREEN : KEEP_SCREEN;
        operate = KEEP_SCREEN;
    }
    // logVerbose(LCDM,"LCDRSE::CheckPress--> OUT!  operate:",operate);
    return  operate;
}

bool LCDRSE::GetTimerCntFlag() const
{
    return m_timerEnable;
}

void LCDRSE::SetTimerCntFlag(bool timerEnable)
{
    m_timerEnable = timerEnable;
}

int LCDRSE::GetTimerEnableCount() const
{
    return m_TimerEnableCount;
}

void LCDRSE::SetTimerEnableCount(int TimerEnableCount)
{
    m_TimerEnableCount = TimerEnableCount;
}

    bool LCDRSE::GetIsInputReady() const
{
    return m_isInputReady;
}
void LCDRSE::SetIsInputReady(bool isReady)
{
    m_isInputReady=isReady;
}

int LCDRSE::GetReadyCheckCnt() const
{
    return m_ReadyCheckCnt;
}
void LCDRSE::SetReadyCheckCntPlus()
{
    m_ReadyCheckCnt++;
}
