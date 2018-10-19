/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef LCDMANAGER_H
#define LCDMANAGER_H
#include <mutex>
#include <thread>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <map>
#include <unordered_map>

#include <errno.h>// 包含errno所需要的头文件
#include "LCD.h"
#include "LCDRSE.h"
#include "InputDevice.h"
extern "C"
{
    #include "hwprop-0/libhwprop.h"
};


#include "ama_enum.h"
#include "ama_types.h"


struct LCDM_Property {
    int mapSize;
    std::map<E_SEAT,char*>  temperaturePathMap;
    std::map<E_SEAT,char*>  blBrightnessPathMap;
    std::map<E_SEAT,char*>  blPowerPathMap;
    std::map<E_SEAT,char*>  inputDevicePathMap;
    std::map<E_SEAT,char*>  touchDevicePathMap;
};


//call back function
typedef void(*RearTouchStatusCB) (E_SEAT seatID,ama_PowerSignal_e signal);
void StartListenRearTouchStatusCB(RearTouchStatusCB f);
void StopListenRearTouchStatusCB(RearTouchStatusCB f);

static const hw_property inputDevPathEnumArray [4] =  {HW_PROPERTY_MAX,
                                                 HW__SEAT_IVI__INPUT__DEV_NODE,
                                                 HW__SEAT_RSE_LEFT__INPUT__DEV_NODE,
                                                 HW__SEAT_RSE_RIGHT__INPUT__DEV_NODE};

static const hw_property touchStatusPathEnumArray [4]={HW_PROPERTY_MAX,//no this
                                                 HW__TOUCH_STATUS_IVI_SYSFS_NODE,
                                                 HW__TOUCH_STATUS_RES_LEFT_SYSFS_NODE,
                                                 HW__TOUCH_STATUS_RES_RIGHT_SYSFS_NODE};

static const hw_property blPowerPathEnumArray [4] =   {HW__SEAT_CLUSTER__BACKLIGHT__BL_POWER__SYSFS_NODE,
                                                 HW__SEAT_IVI__BACKLIGHT__BL_POWER__SYSFS_NODE,
                                                 HW__SEAT_RSE_LEFT__BACKLIGHT__BL_POWER__SYSFS_NODE,
                                                 HW__SEAT_RSE_RIGHT__BACKLIGHT__BL_POWER__SYSFS_NODE};

static const hw_property blBrightnessPathEnumArray [4]={HW__SEAT_CLUSTER__BACKLIGHT__BRIGHTNESS__SYSFS_NODE,
                                                  HW__SEAT_IVI__BACKLIGHT__BRIGHTNESS__SYSFS_NODE,
                                                  HW__SEAT_RSE_LEFT__BACKLIGHT__BRIGHTNESS__SYSFS_NODE,
                                                  HW__SEAT_RSE_RIGHT__BACKLIGHT__BRIGHTNESS__SYSFS_NODE};

static const hw_property temperaturePathEnumArray [4] = {HW__CLUSTER__TEMPERATURE__SYSFS_NODE,
                                                   HW__SEAT_IVI__TEMPERATURE__SYSFS_NODE,
                                                   HW__SEAT_RSE_LEFT__TEMPERATURE__SYSFS_NODE,
                                                   HW__SEAT_RSE_RIGHT__TEMPERATURE__SYSFS_NODE};



using std::map;

/*
 * 类的名称：LCD  Manager LCD管理模块
 * 类的功能：用于管理系统中的屏幕 包括屏幕开关、亮度、温度、点击事件等
 * 其他信息：系统中的屏幕个数由frameworkConfig.xml中的配置文件管理
 */

class LCDManager
{
public:
    LCDManager();
    ~LCDManager();
    void Init();

    std::vector<E_SEAT> GetSeatVector();

    void SetIsLCDEnabled(bool isEnable);
    bool GetIsLCDEnabled();

    void CheckTemperature(E_SEAT seatid);
    //LCDManager get screen temperature check if need to close lcd
    int GetTemperature(E_SEAT seatid);
    //LCDManager get normal temperature for engineering.
    std::unordered_map<int32_t, int32_t> GetAllLCDTemperature();

    //old interface
    //for sm
    void SetLCDDegree4SM(E_SEAT seatid,const int degree,const LCD_MODE mode);
    //for sm
    //for pmc
    void SetLCDDegree4FPM(E_SEAT seatid,const int degree,bool isDisable);
    void SaveLastDegree(E_SEAT seatid);
    void SetLowestDegree(E_SEAT seatid);
    void SetNormalDegree(E_SEAT seatid);
    void RecoverLastDegree(E_SEAT seatid);
    //for pmc
    //old interface

    //current interface
    void SetLCDPower(E_SEAT seatid ,bool onoff);
    void SetLCDBrightness(int value,E_SEAT seatid);
    void SetLowestBrightness(E_SEAT seatid);
    void SetNormalBrightness(E_SEAT seatid);
    //current interface


    void CheckInputDev(E_SEAT seatid);
    void SetRearTouchStatusCB(RearTouchStatusCB f);

    void SetRearTimerFlag(bool isEnable,E_SEAT seatid);
    void SetRearTimerFlag(LCDRSE* lcd);
    void SetRearTimerState(bool isEnable,AMPID ampid);

    bool executeLinuxCMD(const char* cmdstring);

    static LCDManager* GetInstance();
   // LCD *LCDObjArray [4];
    char * GetInputPath(E_SEAT seatid);


private ://member
    //FLAGS:
    //flag of if lcd is locked
    bool isLCDEnabled;
    //flag of if has recovered form lock mode
    bool isRecoverdLCDMode;
    //flag of if saved the lcd degree
    bool m_isSaveDegree;

    //MAPS & VECTORS:
    //LCD we managed
    std::map<E_SEAT,LCD*> m_LCDMap;
    //get seats form xmlConfig files
    std::vector<E_SEAT> m_SeatVector;
    //record lcd onoff when system is in awake mode
    std::map<E_SEAT,bool> LCDModeMap;
    //record which app to set lcd timer stop
    std::map<AMPID,bool> AppSetLCDStopMap;
    //record temperature of seat
    std::map<E_SEAT,TEMPERATURE_STATE> m_TemperatureStatesMap;

    //OTHER:
    bool ResetOK;
    int m_SeatNumber;
    std::map<E_SEAT ,int>T90_T95_cnt_Map;
    std::map<E_SEAT ,int>T95_T100_cnt_Map;
    LCDM_Property m_LCDMProperty;
    RearTouchStatusCB m_RearTouchStatusCB;
    std::map<E_SEAT,TEMPERATURE_STATE> TemperatureStatesMap;

private://function

    void RestoreLastLCDMode(E_SEAT seatid ,bool onoff);
    void RecoverLastLCDMode();
    bool CheckRSETouchDriverReady(E_SEAT seatid);

    bool CheckIsInSeatVector(E_SEAT seatid ,std::string funcName);

    //fucntion for init
    void GetScreenNumber();
    void EnableTouchDriver();
    void GetDevicePath();
    void GenerateLCDObject();
    void TimerInverval();
    bool OnTemperatureChange(E_SEAT seat ,int temperature);
    void StartTimer();
    //fucntion for init

};

#endif // LCDMANAGER_H
