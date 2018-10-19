/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef LCDRES_H
#define LCDRES_H
#include "LCD.h"
#include "InputDevice.h"
#include "Configuration.h"

#define LCDM_USLEEP_INTERVAL 20
#define RSE_DETECT_STEP  5
#define TIME_TO_CLOSE_SCREEN 30*50 //30*20*50=3000ms
typedef enum
{
    TURN_ON_SCREEN,
    TURN_OFF_SCREEN,
    KEEP_SCREEN
}REAR_SCREEN_OPERATE;



class LCDRSE : public LCD
{
public:
    LCDRSE();
    LCDRSE(E_SEAT seatid);
    ~LCDRSE();
    InputDevice *GetInputDev() const;
    void SetInputDev(InputDevice *inputDev);
    REAR_SCREEN_OPERATE CheckPress();


    bool GetIsInputReady() const;
    void SetIsInputReady(bool isReady);

    int GetReadyCheckCnt() const;
    void SetReadyCheckCntPlus();

    bool GetTimerCntFlag() const;
    void SetTimerCntFlag(bool timerEnable);

    int GetTimerEnableCount() const;
    void SetTimerEnableCount(int TimerEnableCount);

private :
    char *m_inputPath;
    bool m_timerEnable;
    bool m_isInputReady;
    int m_ReadyCheckCnt;
    int m_TimeToCloseLCD;
    int m_ScreenOnTimeCnt;
    int m_TimerEnableCount;
    InputDevice *m_inputDev;
};

#endif // LCDRES_H
