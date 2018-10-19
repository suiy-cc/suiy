/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <stdlib.h>

#include "LCD.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"
#include "BootListener.h"

extern LOG_DECLARE_CONTEXT(LCDM);
LCD::LCD(){}
LCD::LCD(E_SEAT seatid)
{
    m_blPowerState=false;
    m_temperature=-100;
    m_brightness=0;
    m_SMBrightness=0;
    m_degree=5;
    m_lastDegree=5;
    m_seat=seatid;
    m_isRSE=false;
    LCD_MODE m_mode=MODE_DAY;
    TEMPERATURE_STATE m_state=NORMAL_STATE;
    m_isDisable=false;
}


LCD::~LCD()
{
    if (NULL!=m_ScreenTemperatureDev)
        delete(m_ScreenTemperatureDev);
}
TEMPERATURE_STATE LCD::GetState() const
{
    return m_state;
}

void LCD::SetState(const TEMPERATURE_STATE &state)
{
    m_state = state;
}

LCD_MODE LCD::GetMode() const
{
    return m_mode;
}

void LCD::SetMode(const LCD_MODE &mode)
{
    m_mode = mode;
}

int LCD::GetDegree() const
{
    return m_degree;
}

void LCD::SetDegree(int degree)
{
    m_degree = degree;
}

E_SEAT LCD::GetSeat() const
{
    return m_seat;
}

void LCD::SetSeat(const E_SEAT &seat)
{
    m_seat = seat;
}

char *LCD::GetBlPowerPath()
{
    return m_blPowerPath;
}

void LCD::SetBlPowerPath(const char* blPowerPath)
{
    // if strlen(nullprt) cause a coredump
    if (NULL==blPowerPath)
    {
        return;
    }
    memcpy(m_blPowerPath,blPowerPath,strlen(blPowerPath));
}

char *LCD::GetBlBrightnessPath()
{
    return m_blBrightnessPath;
}

void LCD::SetBlBrightnessPath(const char* blBrightnessPath)
{
    if (NULL==blBrightnessPath)
    {
        return;
    }
    memcpy(m_blBrightnessPath,blBrightnessPath,strlen(blBrightnessPath));
}
char *LCD::GetTemperaturePath()
{
    return m_temperaturePath;
}
void LCD::SetTemperaturePath(const char* temperaturePath)
{
    if (NULL==temperaturePath)
    {
        return;
    }
    memcpy(m_temperaturePath,temperaturePath,strlen(temperaturePath));
}
char *LCD::GetInputDevPath()
{
    logInfo(LCDM,"LCD GetInputDevPath:",std::string(m_inputDevPath));
    return m_inputDevPath;
}
void LCD::SetInputDevPath(const char* inputDevPath)
{

    if (NULL==inputDevPath)
    {

            logErrorF(LCDM,"LCD SetInputDevPath:seat %d get null path!!!",(int)GetSeat());
            return;

    }
    memcpy(m_inputDevPath,inputDevPath,strlen(inputDevPath));
 //   logInfo(LCDM,"LCD SetInputDevPath:",std::string(inputDevPath));
}

void LCD::Write2File()
{
    logVerbose(LCDM,"LCD::Write2File()--> IN");
    char value[4] = {0};
    int i=GetDegree();
    int len=0;
    if (GetMode()==MODE_NIGHT){
        memcpy(value,nightMode[i],strlen(nightMode[GetDegree()]));
        len=strlen(nightMode[GetDegree()]);
    }
    else{
        memcpy(value,dayMode[i],strlen(dayMode[GetDegree()]));
        len=strlen(dayMode[GetDegree()]);
    }

    std::string strValue(value);
#ifdef CODE_FOR_GAIA
    BootListener::GetInstance()->setLCDBrightness(m_seat, stol(strValue));  

#else
    int file=open(GetBlBrightnessPath() , O_WRONLY);
    logDebugF(LCDM,"LCD::Write2File open file ok");

    if(file<=0)
    {
        logErrorF(LCDM,"LCD::Write2File open  %s device error!",GetBlBrightnessPath() );
        close(file);
    }
    else
    {
        ssize_t retVal= write(file,strValue.c_str(),strValue.length());
        logDebugF(LCDM,"LCD::Write2File len:%d value:%s return:%d",len,value,retVal);
        close(file);
        logDebugF(LCDM,"LCD::Write2File close file ok");
    }

#endif

    logVerbose(LCDM,"LCD::Write2File()-->OUT");
}

int LCD::GetLastDegree() const
{
    return m_lastDegree;
}

void LCD::SetLastDegree(int lastDegree)
{
    m_lastDegree = lastDegree;
}

LCD_MODE LCD::GetLastMode() const
{
    return m_lastMode;
}

void LCD::SetLastMode(const LCD_MODE &lastMode)
{
    m_lastMode = lastMode;
}

bool LCD::GetIsDisable() const
{
    return m_isDisable;
}

void LCD::SetIsDisable(bool isDisable)
{
    m_isDisable = isDisable;
}



bool LCD::GetBlPowerState() const
{
    return m_blPowerState;
}

void LCD::SetBlPowerState(bool blPower)
{
    m_blPowerState = blPower;
}

ScreenTemperature* LCD::GetScreenTemperatureDev()
{
    if (NULL==m_ScreenTemperatureDev)
    {
        m_ScreenTemperatureDev=new ScreenTemperature(this->GetTemperaturePath());
    }
    //logVerboseF(LCDM,"getScnTemp  path:%s " ,this->GetTemperaturePath());
    return m_ScreenTemperatureDev;
}
int LCD::GetTemperature()
{
    //logVerboseF(LCDM,"LCD::GetTemperature()---> IN ");
    int temperature=0;
    temperature=this->GetScreenTemperatureDev()->GetTemperature();
    //logVerboseF(LCDM,"LCD::GetTemperature()--->OUT %d temperature:%d ",(int)m_seat,temperature);
    return temperature;
}

int LCD::GetBrightness()
{
    return m_brightness;
}

void LCD::SetBrightness(int brightness)
{
    m_brightness = brightness;
}

int LCD::GetSMBrightness()
{
    return m_SMBrightness;
}

void LCD::SetSMBrightness(int SMBrightness)
{
    m_SMBrightness = SMBrightness;
}


int LCD::GetLCDTemperature()
{
    return m_temperature;
}

void LCD::SetLCDTemperature(int temperature)
{
    m_temperature = temperature;
}

bool LCD::GetIsRse()
{
    return m_isRSE;
}
void LCD::SetIsRse(bool isRse)
{
    m_isRSE=isRse;
}
