/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef LCD_H
#define LCD_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include "log.h"
#include "ama_enum.h"
#include "ScreenTemperature.h"
extern LOG_DECLARE_CONTEXT(LCDM);
typedef enum
{
    NORMAL_STATE,
    T90_T95_SATATE,
    T95_T100_SATATE,
    T100_MORE_STATE

}TEMPERATURE_STATE;

typedef enum
{
    MODE_DAY,
    MODE_NIGHT,
    MODE_UNKNOWN

}LCD_MODE;

static const char *nightMode [11] = {"20","25","30","35","40","45","50","55"," 65","75","80"};
static const char *dayMode   [11] = {"50","55","60","65","70","75","80","85"," 90","95","100"};

class LCD
{
public:
    LCD();
    LCD(E_SEAT seatid);
    ~LCD();

    TEMPERATURE_STATE GetState() const;
    void SetState(const TEMPERATURE_STATE &state);

    LCD_MODE GetMode() const;
    void SetMode(const LCD_MODE &mode);

    int GetDegree() const;
    void SetDegree(int degree);


    E_SEAT GetSeat() const;
    void SetSeat(const E_SEAT &seat);

    char *GetTemperaturePath() ;
    void SetTemperaturePath(const char* temperaturePath);

    char *GetInputDevPath() ;
    void SetInputDevPath(const char* inputDevPath);

    char *GetBlPowerPath() ;
    void SetBlPowerPath(const char* blPowerPath);

    char *GetBlBrightnessPath() ;
    void SetBlBrightnessPath(const char* blBrightnessPath);

    void Write2File();

    int GetLastDegree() const;
    void SetLastDegree(int lastDegree);

    LCD_MODE GetLastMode() const;
    void SetLastMode(const LCD_MODE &lastMode);

    bool GetIsDisable() const;
    void SetIsDisable(bool isDisable);

    ScreenTemperature *GetScreenTemperatureDev();
    int GetTemperature();

    int GetBrightness() ;
    void SetBrightness(int brightness);

    int GetSMBrightness() ;
    void SetSMBrightness(int SMBrightness);

    int GetLCDTemperature() ;
    void SetLCDTemperature(int temperature);

    bool GetIsRse();
    void SetIsRse(bool isRse);


    bool GetBlPowerState() const;
    void SetBlPowerState(bool blPower);


protected:
    bool m_blPowerState;
    bool m_isRSE;
    E_SEAT m_seat;
private:

    int m_temperature=-100;
    int m_brightness=0;//CURRENT BRIGHTNESS
    int m_SMBrightness=0;//SM WANT'S TO SET BRIGHTNESS
    int m_degree=0;
    int m_lastDegree=0;
    char m_temperaturePath[128]={0};
    char m_inputDevPath[128]={0};
    char m_blPowerPath[128]={0};
    char m_blBrightnessPath[128]={0};
    ScreenTemperature *m_ScreenTemperatureDev=NULL;

    bool m_isDisable=false;

    LCD_MODE m_mode=MODE_DAY;
    LCD_MODE m_lastMode=MODE_DAY;
    TEMPERATURE_STATE m_state=NORMAL_STATE;


};

#endif // LCD_H
