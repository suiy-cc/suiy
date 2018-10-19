/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>
#include <LCD/ScreenTemperature.h>
#include <sys/prctl.h>

#include "ReadXmlConfig.h"
#include "LCDManager.h"
#include "PMC/BootListener.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

#define LOWEST_BRIGHTNESS 20
#define LCDM_BUFFER_SIZE 1024

static int rse1SeatCheckCnt=0;
static int rse2SeatCheckCnt=0;

static bool IsListenRearTouch=false;

static int timerCnt=0;
static int inputCnt=0;
static int temperatureCnt=0;

static pthread_t timerThread=0;//for count time
static bool LCDThreadEndFlag=false;

/*   启动监听后屏点击事件
 *   @param  RearTouchStatusCB 后屏点击事件的回调函数
 *   @param
 *   @return
 *   @note
 */
void StartListenRearTouchStatusCB(RearTouchStatusCB f)
{
    logVerbose(LCDM,"LCDManager()-->ListenRearTouchStatusCB  IN ");
    LCDManager::GetInstance()->SetRearTouchStatusCB(f);
    IsListenRearTouch=true;
    inputCnt=0;
    logVerbose(LCDM,"LCDManager()-->ListenRearTouchStatusCB OUT  IsListenRearTouch: ",IsListenRearTouch);
}
/*   停止监听后屏点击事件
 *   @param  RearTouchStatusCB 后屏点击事件的回调函数
 *   @param
 *   @return
 *   @note
 */
void StopListenRearTouchStatusCB(RearTouchStatusCB f)
{
    logVerbose(LCDM,"LCDManager()-->ListenRearTouchStatusCB  IN ");
    // LCDManager::GetInstance()->SetRearTouchStatusCB(NULL);???
    IsListenRearTouch=false;
    logVerbose(LCDM,"LCDManager()-->ListenRearTouchStatusCB OUT ");
}
/*   LCDManager 构造函数
 *   @param
 *   @param
 *   @return
 *   @note
 */
LCDManager::LCDManager()
        :isLCDEnabled(true),
         isRecoverdLCDMode(true),
         m_SeatNumber(0)
{
    RegisterContext(LCDM,"LCDM");
    logVerbose(LCDM,"LCDManager()--> IN");
    Init();
    logVerbose(LCDM,"LCDManager()-->OUT");
}
/*   LCDManager 析构函数
 *   @param
 *   @param
 *   @return
 *   @note
 */
LCDManager::~LCDManager()
{

    logVerbose(LCDM,"~LCDManager()--> IN ");
    if (!AppSetLCDStopMap.empty()){
        AppSetLCDStopMap.clear();
    }
    if (!m_LCDMap.empty()){
        m_LCDMap.clear();
    }

    if (!m_LCDMProperty.inputDevicePathMap.empty()){
        m_LCDMProperty.inputDevicePathMap.clear();
    }
    if (!m_LCDMProperty.temperaturePathMap.empty()){
        m_LCDMProperty.temperaturePathMap.clear();
    }
    if (!m_LCDMProperty.blPowerPathMap.empty()){
        m_LCDMProperty.blPowerPathMap.clear();
    }
    if (!m_LCDMProperty.blBrightnessPathMap.empty()){
        m_LCDMProperty.blBrightnessPathMap.clear();
    }
    if (!m_LCDMProperty.touchDevicePathMap.empty()){
        m_LCDMProperty.touchDevicePathMap.clear();
    }

    LCDThreadEndFlag=true;
    pthread_join(timerThread,NULL);
    logVerbose(LCDM,"~LCDManager()-->OUT ");
}

//for count timer every 20 ms
static void *LCDManagerTimer(void *)
{
    // set thread name
    prctl(PR_SET_NAME, "LCDManagerTimer");

    while(!LCDThreadEndFlag)
    {

        if (TEMPERATURE_DETECT_STEP<=temperatureCnt)//check for every 500ms
        {
            temperatureCnt=0;
            //framework check cluster's temperature only for engineering
            for (auto & seat :  LCDManager::GetInstance()->GetSeatVector() )
            {
                LCDManager::GetInstance()->CheckTemperature(seat);
            }
        }



        //logVerboseF(LCDM,"IsListenRearTouch() %s inputCnt:%d",IsListenRearTouch?"true":"false",inputCnt);
        if ((RSE_DETECT_STEP<=inputCnt) && IsListenRearTouch )//check for every 100ms
        //if ( IsListenRearTouch )
        {
            inputCnt=0;
            //check inputdev 2 (left rear screen)  stop checking after 20s without inputPath.
            for (auto & seat :  LCDManager::GetInstance()->GetSeatVector() )
            {
                if (E_SEAT_RSE2==seat||E_SEAT_RSE1==seat)//only check rse
                    LCDManager::GetInstance()->CheckInputDev(seat);
            }


        }
        inputCnt++;

        std::this_thread::sleep_for(std::chrono::milliseconds(LCDM_USLEEP_INTERVAL));
        temperatureCnt++;
        //timerCnt++;
    }
}
/*   获取LCD是否可以操作
 *   @param
 *   @param
 *   @return true: 有控制权  false: 没有控制权
 *   @note   在开机动画播放时，虽然LCDM已经启动，但是此时还没有
 *           获得LCD的控制权，所以不响应用户操作，待获得控制权后再响应
 */
bool LCDManager::GetIsLCDEnabled()
{
    return isLCDEnabled;
}
/*   设置LCD是否可以操作
 *   @param
 *   @param
 *   @return true: 有focus  false: 没有focus
 *   @note   在开机动画播放时，虽然LCDM已经启动，但是此时还没有
 *           获得LCD的控制权，所以不响应用户操作，待获得控制权后再响应
 */
void LCDManager::SetIsLCDEnabled(bool isEnable)
{
    logVerbose(LCDM,"LCDManager::SetIsLCDEnabled()--> IN ");
    isLCDEnabled=isEnable;
    if ((!isRecoverdLCDMode)&&isEnable)
    {
        RecoverLastLCDMode();
       // StartTimer();
    }
    logVerbose(LCDM,"LCDManager::SetIsLCDEnabled()-->OUT ");
}


void LCDManager::Init()
{
    logVerbose(LCDM,"LCDManager::Init()--> IN ");
    GetScreenNumber();
    EnableTouchDriver();
    GetDevicePath();
    GenerateLCDObject();
    StartTimer();
    logVerbose(LCDM,"LCDManager::Init()-->OUT ");
}
/*   获得系统屏幕数
 *   @param
 *   @param
 *   @return
 *   @note   该功能仅通过配置文件来获取系统中的屏幕数，
 *           有些情况下并不与系统实际的屏幕数相符，
 *           因为该功能描述的是：希望去管理几个屏幕
 */
void LCDManager::GetScreenNumber()
{
    logVerbose(LCDM,"LCDManager::GetScreenNumber()--> IN");
    std::string configKey="ScreenNumber";
    std::vector<std::string> dataVector;
    RXC_Error retVal = GetConfigFromVector(configKey, dataVector);
    if (Error_None!=retVal)
    {
        logDebugF(LCDM,"LCDManager::GetScreenNumber() GetConfigFromVector failed . return :%d",(int)retVal);
        logVerbose(LCDM,"LCDManager::GetScreenNumber()-->OUT");
        return ;
    }

    for (auto &iter : dataVector)
    {
        E_SEAT seat = (E_SEAT)atol(iter.c_str());
        m_SeatVector.push_back(seat);
        //by the way init m_TemperatureStatesMap
        m_TemperatureStatesMap.insert(std::pair<E_SEAT,TEMPERATURE_STATE>(seat,NORMAL_STATE));

        logDebugF(LCDM,"LCDManager::GetScreenNumber() Get SeatID:%d",seat);
    }
    if (m_SeatVector.size()<=0)
    {
        m_SeatVector.push_back(E_SEAT_IVI);
        m_TemperatureStatesMap.insert(std::pair<E_SEAT,TEMPERATURE_STATE>(E_SEAT_IVI,NORMAL_STATE));
        logWarnF(LCDM,"LCDManager::GetScreenNumber() Can Get Valid Seat Insert IVI ONLY!!!");
    }
    logVerbose(LCDM,"LCDManager::GetScreenNumber()-->OUT");
}
/*   使能系统屏幕的touch
 *   @param
 *   @param
 *   @return
 *   @note   该功能原来在iasImage中，由于开机动画的加入touch的使能由appManager来做
 */
void LCDManager::EnableTouchDriver()
{
    logVerbose(LCDM,"LCDManager::enableTouchDriver()--> IN");
    for (auto &iter : m_SeatVector)
    {
        if(iter==E_SEAT_TEST)
            continue;//touchStatusPathEnumArray[0] is a invalid data

        char* tmpChar  =(char *) hw_get_system_property_string(touchStatusPathEnumArray[iter]);
        if (NULL==tmpChar)
        {
            logErrorF(LCDM,"LCDManager::enableTouchDriver() Get invalid devPath name:%d ",touchStatusPathEnumArray[iter]);
            continue;
        }
        m_LCDMProperty.inputDevicePathMap.insert(std::pair<E_SEAT,char*>(iter,tmpChar));
        int driverFile=open(tmpChar, O_RDWR);
        if(driverFile<=0)
        {
            logErrorF(LCDM,"LCDManager::enableTouchDriver() open  %s  error!",tmpChar);
            close(driverFile);
        }
        else
        {
            ssize_t retVal= write(driverFile,"1",1 );
            close(driverFile);
            logDebugF(LCDM,"LCDManager::enableTouchDriver() write %s return :%d",tmpChar,retVal);
        }
    }
    logVerbose(LCDM,"LCDManager::enableTouchDriver()-->OUT");
}

/*   通过libDeviceManager 获得设备路径
 *   @param
 *   @param
 *   @return
 *   @note
 */
void LCDManager::GetDevicePath()
{

    logVerbose(LCDM,"LCDManager::GetDevicePath()--> IN");
    for (auto &seat : m_SeatVector)
    {
        //get blpower path
        char *blPowerPath = (char *) hw_get_system_property_string(blPowerPathEnumArray[seat]);
        if(NULL == blPowerPath)
        {
            logErrorF(LCDM, "LCDManager::GetDevicePath() Get invalid seat :%d blpowerPath name:%d ",(int)seat,blPowerPathEnumArray[seat]);
            continue;
        }
        else
        {
            logDebugF(LCDM,"LCDManager::GetDevicePath() seat:%d blPowerPath  %s",(int)seat , blPowerPath);
            m_LCDMProperty.blPowerPathMap.insert(std::pair<E_SEAT,char*>(seat,blPowerPath));
        }

        //get blbrightness path
        char *blBrightnessPath = (char *) hw_get_system_property_string(blBrightnessPathEnumArray[seat]);
        if(NULL == blBrightnessPath)
        {
            logErrorF(LCDM, "LCDManager::GetDevicePath() Get invalid seat:%d blBrightnessPath name:%d ",(int)seat,blBrightnessPathEnumArray[seat]);
            continue;
        }
        else
        {
            logDebugF(LCDM,"LCDManager::GetDevicePath() seat:%d blBrightnessPath  %s",(int)seat , blBrightnessPath);
            m_LCDMProperty.blBrightnessPathMap.insert(std::pair<E_SEAT,char*>(seat,blBrightnessPath));
        }


        //get temperature path
        char *temperaturePath = (char *) hw_get_system_property_string(temperaturePathEnumArray[seat]);
        if(NULL == temperaturePath)
        {
            logErrorF(LCDM, "LCDManager::GetDevicePath() seat:%d Get invalid temperaturePath name:%d ",(int)seat,temperaturePathEnumArray[seat]);
            continue;
        }
        else
        {
            logDebugF(LCDM,"LCDManager::GetDevicePath() seat:%d temperaturePath  %s" ,(int)seat, temperaturePath);
            m_LCDMProperty.temperaturePathMap.insert(std::pair<E_SEAT,char*>(seat,temperaturePath));
        }

        if(seat==E_SEAT_TEST)
            continue;//inputDevPathEnumArray[0] is a invalid data

        //get input path
//        char *inputDevicePath = (char *)hw_get_system_property_string(inputDevPathEnumArray[seat]);
//        if(NULL == inputDevicePath)
//        {
//            logErrorF(LCDM, "LCDManager::GetDevicePath() Get invalid seat:%d inputDevicePath name:%d ",(int)seat,inputDevPathEnumArray[seat]);
//            continue;
//        }
//        else
//        {
//            logDebugF(LCDM,"LCDManager::GetDevicePath() seat:%d inputDevicePath  %s",(int)seat , inputDevicePath);
//            m_LCDMProperty.inputDevicePathMap.insert(std::pair<E_SEAT,char*>(seat,inputDevicePath));
//        }
    }

    logVerbose(LCDM,"LCDManager::GetDevicePath()-->OUT ");
}
void LCDManager::GenerateLCDObject()
{
    logVerbose(LCDM,"LCDManager::generateLCDObject()--> IN");

    for (auto &seat : m_SeatVector)
    {
        logDebugF(LCDM,"LCDManager::GenerateLCDObject() Generate LCD seat %d" , seat);
        if (E_SEAT_RSE1==seat||E_SEAT_RSE2==seat){
            m_LCDMap.insert(std::pair<E_SEAT,LCD*>(seat,new LCDRSE(seat)));
        }
        else{
            m_LCDMap.insert(std::pair<E_SEAT,LCD*>(seat,new LCD(seat)));
        }
        m_LCDMap[seat]->SetBlBrightnessPath(m_LCDMProperty.blBrightnessPathMap[seat]);
        m_LCDMap[seat]->SetTemperaturePath(m_LCDMProperty.temperaturePathMap[seat]);
        m_LCDMap[seat]->SetBlPowerPath(m_LCDMProperty.blPowerPathMap[seat]);

    }
    logVerbose(LCDM,"LCDManager::generateLCDObject()-->OUT");
}

std::vector<E_SEAT> LCDManager::GetSeatVector()
{
    return m_SeatVector;
}
/*   获得设备路径
 *   @param   E_SEAT 座位号
 *   @param
 *   @return
 *   @note
 */
char * LCDManager::GetInputPath(E_SEAT seatid)
{

    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return NULL;
    }
    if (NULL!=m_LCDMProperty.inputDevicePathMap[seatid] )
        if(0!=strcmp(m_LCDMProperty.inputDevicePathMap[seatid],""))//use strcmp must pointer not null
        {
            //   logWarnF(LCDM,"get from array %s",inputDevPathArray[seatid] );
            return m_LCDMProperty.inputDevicePathMap[seatid];
        }
    m_LCDMProperty.inputDevicePathMap[seatid] =(char *) hw_get_system_property_string(inputDevPathEnumArray[seatid]);
    if (NULL!=m_LCDMProperty.inputDevicePathMap[seatid])
    {
        logWarnF(LCDM,"get from hw_get_system_property_string");
        m_LCDMap[seatid]->SetInputDevPath(m_LCDMProperty.inputDevicePathMap[seatid]);
    }
    else
    {
        if (E_SEAT_RSE1==seatid){
            rse1SeatCheckCnt++;
            if( 200==rse1SeatCheckCnt)
                logErrorF(LCDM,"LCDManager::getInputPath()  get seat %d's input path!! failed!!!!!",seatid);
        }
        if (E_SEAT_RSE2==seatid){
            rse2SeatCheckCnt++;
            if( 200==rse2SeatCheckCnt)
                logErrorF(LCDM,"LCDManager::getInputPath()  get seat %d's input path!! failed!!!!!",seatid);
        }

        logErrorF(LCDM,"LCDManager::getInputPath() can not get seat %d's correct input path!!",seatid);
    }
    return m_LCDMProperty.inputDevicePathMap[seatid];
}
/*   检测后屏touch 如果30秒没有点击事件就关屏，同时通知回调函数
 *   @param   E_SEAT 座位号
 *   @param
 *   @return
 *   @note   首先要判断后屏touch驱动是否加载完成
 */
void LCDManager::CheckInputDev(E_SEAT seatid)
{
   // logVerbose(LCDM," LCDManager::CheckInputDev-->IN seatid:",seatid);
    if (!CheckIsInSeatVector(seatid,""))
    {
        return ;
    }
    auto tmpLCD = m_LCDMap[seatid];
    auto tmpRse = static_cast<LCDRSE*>(tmpLCD);
    if (!tmpRse->GetBlPowerState())
    {
        //logDebugF(LCDM,"CheckInputDev() seat :%d is closed return!",seatid);
        return;
    }

    if (!CheckRSETouchDriverReady(seatid)){
        //logDebugF(LCDM,"CheckInputDev() CheckRSETouchDriverReady return false");
        return;
    }



    REAR_SCREEN_OPERATE retVal=(REAR_SCREEN_OPERATE)(tmpRse->CheckPress());
    if (KEEP_SCREEN!=retVal){
        logDebugF(LCDM," CheckInputDev REAR_SCREEN_OPERATE:%d",retVal);
    }

    //bool isTurnON = true;
    if(TURN_OFF_SCREEN==retVal){
        logDebugF(LCDM," CheckInputDev get seat:%d E_RSE_SCREEN_OFF:",seatid);
        m_RearTouchStatusCB(seatid,E_RSE_SCREEN_OFF);
    }

   // logVerbose(LCDM," LCDManager::CheckInputDev-->OUT retVal:",retVal);
}

/*   检测后幕的touch是否可用
 *   @param   E_SEAT 座位号
 *   @param
 *   @return
 *   @note    使能touch之后需要一定时间才能收到touch的消息，3个屏幕一共需要大概2秒左右
 */
bool LCDManager::CheckRSETouchDriverReady(E_SEAT seatid)
{
    auto tmpLCD = m_LCDMap[seatid];
    auto tmpRse = static_cast<LCDRSE*>(tmpLCD);
    //logVerbose(LCDM,"LCDManager::CheckRSEReady()--> IN");
    //if isReady==ture or have checked more than 100 times, do not check again.
    if (tmpRse->GetIsInputReady()||tmpRse->GetReadyCheckCnt()>=100)
    {
        //logDebugF(LCDM,"LCDManager::CheckRSEReady() first GetIsInputReady:%s GetReadyCheckCnt :%d return"
         //       ,tmpRse->GetIsInputReady()?"true":"false",tmpRse->GetReadyCheckCnt());
       // logVerbose(LCDM,"LCDManager::CheckRSEReady()-->OUT");
        return tmpRse->GetIsInputReady();
    }

    //check if device file existo
    char * tmpChar=(char *) hw_get_system_property_string(inputDevPathEnumArray[seatid]);

    if (NULL!=tmpChar)
    {//file exist put it in to the inputDevicePathMap
        m_LCDMProperty.inputDevicePathMap.insert(std::pair<E_SEAT,char*>(seatid,tmpChar));
        logWarnF(LCDM,"Get seat:%d inputPath:%s",seatid, tmpChar);
        //set file path and flag
        tmpRse->SetInputDevPath(tmpChar);
        tmpRse->SetIsInputReady(true);
    }
    else
    {//if check 100 times and file still not exist set flag:FALSE
        tmpRse->SetReadyCheckCntPlus();//cnt ++
        if (tmpRse->GetReadyCheckCnt()>=100)
            tmpRse->SetIsInputReady(false);

        // logDebugF(LCDM,"LCDManager::CheckRSEReady() second GetIsInputReady:%s GetReadyCheckCnt :%d"
        //        ,tmpRse->GetIsInputReady()?"true":"false",tmpRse->GetReadyCheckCnt());

    }
  //  logVerbose(LCDM,"LCDManager::CheckRSEReady()-->OUT");
    return  tmpRse->GetIsInputReady();


}
void LCDManager::SetRearTouchStatusCB(RearTouchStatusCB f)
{
    logVerbose(LCDM,"LCDManager::SetRearTouchStatusCB -->IN");
    m_RearTouchStatusCB=f;
    logVerbose(LCDM,"LCDManager::SetRearTouchStatusCB -->OUT");
}

void LCDManager::StartTimer()
{
    //创建计时线程
    int ret=0;
    ret=pthread_create(&timerThread,NULL,LCDManagerTimer,NULL);
    if(ret!=0){
        logError (LCDM,"LCDManager::startTimer() Create thread error!");
    }
    else
    {
        logDebug (LCDM,"LCDManager::startTimer() Create thread success!");
    }

}

static int test_temperature=39;
static int flag_temperature=true;
void tstTemperature()
{
    if (flag_temperature)
    {
        if (test_temperature<88)
        {
            test_temperature=test_temperature+10;
        }
        else if (test_temperature>=88&&test_temperature<105)
        {
            test_temperature=test_temperature+1;
        }
        else
            flag_temperature=false;
    }
    else
    {
        if (test_temperature>88)
        {
            test_temperature=test_temperature-1;
        }
        else if (test_temperature>40&&test_temperature<=88)
        {
            test_temperature=test_temperature-5;
        }
        else
            flag_temperature=true;
    }
}

void LCDManager::CheckTemperature(E_SEAT seatid)
{
    //tstTemperature();
    GetTemperature(seatid);
    //为了兼容fpm已有接口
}
int LCDManager::GetTemperature(E_SEAT seatid)
{
    //logVerboseF(LCDM,"LCDManager::GetTemperature SEATID: %d",seatid);
    int iTemperature=0;
    auto Lcd = m_LCDMap[seatid];
    iTemperature=Lcd->GetTemperature();
    //save temperature to CLASS::LCD
    Lcd->SetLCDTemperature(iTemperature);
    //FOR TEST:
    // iTemperature = test_temperature;
    // seatid=E_SEAT_RSE2;
    //logVerboseF(LCDM,"GetTemperature: %d SEATID: %d",iTemperature,seatid);
    if (E_SEAT_TEST==seatid){
        //we don't care about cluster's temerature.
    }
    else{
        OnTemperatureChange(seatid,iTemperature);
    }
    return iTemperature;
}


bool LCDManager::OnTemperatureChange(E_SEAT seat ,int temperature)
{

    if (90>temperature)
    {
        if ( TemperatureStatesMap[seat]!=NORMAL_STATE)
        {
            logVerbose(LCDM,"OnTemperatureChange--> set NORMAL_STATE and recover");
            RecoverLastDegree(seat);
            TemperatureStatesMap[seat]=NORMAL_STATE;
            m_RearTouchStatusCB(seat,E_SCR_TEMP_HIGH_90);
        }
        else  if (!m_isSaveDegree)
        {
            logVerbose(LCDM,"OnTemperatureChange--> set NORMAL_STATE and save");
            SaveLastDegree(seat);
            TemperatureStatesMap[seat]=NORMAL_STATE;
        }
        else
        {
            // logVerboseF(LCDM,"OnTemperatureChange--> temperature:%d do nothing",temperature);
        }

    }
    else if (90<=temperature&&temperature<95)
    {
        switch (TemperatureStatesMap[seat])
        {
            case NORMAL_STATE:
                break;
            case T90_T95_SATATE:
                T95_T100_cnt_Map[seat]=0;
                T90_T95_cnt_Map[seat]++;
                if (T90_T95_cnt_Map[seat]>=5)
                {
                    T90_T95_cnt_Map[seat]=0;
                    TemperatureStatesMap[seat]=T90_T95_SATATE;
                    m_RearTouchStatusCB(seat,E_SCR_TEMP_HIGH_90_95);
                }
                break;
            case T95_T100_SATATE:
                break;
            case T100_MORE_STATE:
                T90_T95_cnt_Map[seat]++;
                if (T90_T95_cnt_Map[seat]>=5)
                {
                    logVerbose(LCDM,"OnTemperatureChange--> set T90_T95_SATATE");
                    T90_T95_cnt_Map[seat]=0;
                    TemperatureStatesMap[seat]=T90_T95_SATATE;
                    m_RearTouchStatusCB(seat,E_SCR_TEMP_HIGH_90_95);
                }
                break;
            default:
                break;
        }
    }
    else if (95<=temperature&&temperature<100)
    {
        switch (TemperatureStatesMap[seat])
        {
            case NORMAL_STATE:
                T90_T95_cnt_Map[seat]=0;
                T95_T100_cnt_Map[seat]++;
                if (T95_T100_cnt_Map[seat]>=5)
                {
                    logVerbose(LCDM,"OnTemperatureChange--> set T95_T100_SATATE");
                    T95_T100_cnt_Map[seat]=0;
                    SaveLastDegree((E_SEAT)(seat-1));//normal -> abnormal  save last degree
                    TemperatureStatesMap[seat]=T95_T100_SATATE;
                    m_RearTouchStatusCB(seat,E_SCR_TEMP_HIGH_95_100);
                }
            case T90_T95_SATATE:
                break;
            case T95_T100_SATATE:
                T90_T95_cnt_Map[seat]=0;
                T95_T100_cnt_Map[seat]++;
                if (T95_T100_cnt_Map[seat]>=5)
                {
                    T90_T95_cnt_Map[seat]=0;
                    TemperatureStatesMap[seat]=T95_T100_SATATE;
                    m_RearTouchStatusCB(seat,E_SCR_TEMP_HIGH_95_100);
                }
                break;
            case T100_MORE_STATE:
                T90_T95_cnt_Map[seat]=0;
                TemperatureStatesMap[seat]=T100_MORE_STATE;
                break;
            default:
                break;
        }
    }
    else if (100<=temperature)// poweroff
    {
        if (T100_MORE_STATE!=TemperatureStatesMap[seat])
        {
            logVerbose(LCDM,"OnTemperatureChange--> set T100_MORE_STATE");
            TemperatureStatesMap[seat]=T100_MORE_STATE;
            m_RearTouchStatusCB(seat,E_SCR_TEMP_HIGH_100);
        }
    }

    // logVerbose(FPM,"signalIVITemperatureChange--> OUT!");
}
bool LCDManager::CheckIsInSeatVector(E_SEAT seatid ,std::string funcName)
{
    for (auto seat : m_SeatVector)
    {
        if (seat==seatid)
            return true;
    }
    if (!funcName.empty())//if do not want this log USE: CheckIsInSeatVector(seatid,"")
        logErrorF(LCDM,"LCDManager::%s() get invalid seatid:%d",funcName.c_str(),seatid);
    logVerboseF(LCDM,"LCDManager::%s()-->OUT ",funcName.c_str());
    return false;
}


//get signal from apps
void LCDManager::SetRearTimerState(bool isEnable,AMPID ampid)
{
    logVerboseF(LCDM,"SetRearTimerState--> IN! isEnable: %d ampid: 0x%04X",isEnable,ampid);
    std::map<AMPID,bool>::iterator it;
    it = AppSetLCDStopMap.find(ampid);
    if(it != AppSetLCDStopMap.end())
    {
        if (AppSetLCDStopMap[ampid]==isEnable){
            logVerboseF(LCDM,"SetRearTimerState--> IN! isEnable: %d ampid: 0x%04X get same request return!",isEnable,ampid);
            logVerbose(LCDM,"SetRearTimerState-->OUT! isEnable: ",isEnable);
            return;
        }
        else
            AppSetLCDStopMap[ampid]=isEnable;
    }
    else
    {
        AppSetLCDStopMap.insert(std::pair<AMPID,bool>(ampid, isEnable));
    }
    E_SEAT seatid=(E_SEAT)GET_SEAT(ampid);
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    auto lcd =m_LCDMap[seatid];
    //LCDRSE * lcdres;
    auto lcdres = static_cast<LCDRSE*>(lcd);
    int cnt=lcdres->GetTimerEnableCount();

    if (!isEnable)
    {
        cnt++;
        lcdres->SetTimerEnableCount(cnt);
    }
    else
    {
        cnt--;
        lcdres->SetTimerEnableCount(cnt);
    }
    logDebug(LCDM,"SetRearTimerState::GetTimerEnableCount: ",cnt);
    SetRearTimerFlag(lcdres);
    logVerbose(LCDM,"SetRearTimerState-->OUT! isEnable: ",isEnable);
}
//set A rear screen timer flag
void LCDManager::SetRearTimerFlag(LCDRSE* lcdres)
{
    logVerbose(LCDM,"SetRearTimerFlag--> IN! isEnable: ");

    logDebug(LCDM,"SetRearTimerFlag::GetTimerEnableCount: ",lcdres->GetTimerEnableCount());
    if(lcdres->GetTimerEnableCount()>0)
    {
        lcdres->SetTimerCntFlag(false);
    }
    else
    {
        lcdres->SetTimerCntFlag(true);
    }
    logVerbose(LCDM,"SetRearTimerFlag-->OUT! isEnable: ");
}

void LCDManager::SetRearTimerFlag(bool isEnable,E_SEAT seatid)
{}
LCDManager *LCDManager::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static LCDManager m_Instance;
    return &m_Instance;
}

void LCDManager::SetLCDDegree4SM(E_SEAT seatid,const int degree,const LCD_MODE mode)
{
    logVerbose(LCDM,"LCDManager::SetLCDDegree4SM()--> IN ");
    logInfo(LCDM,"SetLCDDegree4SM() seat:",seatid,"degree:",degree,"mode:",mode);

    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    if (degree>10||degree<0)
    {
        logError(LCDM,"SetLCDDegree4SM() INVALID degree value :",degree);
        return ;
    }
    auto lcd= m_LCDMap[seatid];
    if (lcd->GetDegree()==-1)//never set a degree , set default
    {
        lcd->SetLastDegree(5);
    }

    if (lcd->GetMode()==MODE_UNKNOWN)//never set a mode , set default
    {
        lcd->SetLastMode(MODE_DAY);
    }

    lcd->SetDegree(degree);
    lcd->SetMode(mode);

    if (!lcd->GetIsDisable())
    {
        lcd->Write2File();
    }

    lcd->SetLastDegree(degree);
    lcd->SetLastMode(mode);

    lcd=NULL;
    logVerbose(LCDM,"LCDManager::SetLCDDegree4SM()-->OUT ");
}
void LCDManager::SetLowestDegree(E_SEAT seatid)
{
    logVerbose(LCDM,"LCDManager::SetLowestDegree()--> IN");
    SaveLastDegree(seatid);///??? if necessary
    SetLCDDegree4FPM( seatid,  0, true);
    logVerbose(LCDM,"LCDManager::SetLowestDegree()-->OUT");
}
void LCDManager::SetNormalDegree(E_SEAT seatid)
{
    logVerbose(LCDM,"LCDManager::SetNormalDegree()--> IN");
    RecoverLastDegree(seatid);
    logVerbose(LCDM,"LCDManager::SetNormalDegree()-->OUT");
}

void LCDManager::SetLCDDegree4FPM(E_SEAT seatid,const int degree,bool isDisable)
{
    logInfo(LCDM,"LCDManager::SetLCDDegree4FPM()--> IN seat:",seatid,"degree:",degree,"isDisable :",isDisable);
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    if (degree>10||degree<0)
    {
        logError(LCDM,"LCDManager::SetLCDDegree() INVALID degree value :",degree);
        return ;
    }

    auto lcd= m_LCDMap[seatid];
    lcd->SetDegree(degree);
    //Lcd->SetBlBrightnessPath(blBrightnessPathArray[seatid]);
    lcd->Write2File();
    lcd->SetIsDisable(isDisable);


    logDebug(LCDM,"LCDManager::SetLCDDegree4FPM()-->OUT ");

}

void LCDManager::RestoreLastLCDMode(E_SEAT seatid ,bool onoff)
{
    logVerbose(LCDM,"LCDManager::RestoreLastLCDMode()--> IN");
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    LCDModeMap[seatid]=onoff;
    isRecoverdLCDMode=false;
    //LCDModeMap.insert(std::pair<E_SEAT,bool>(seatid,onoff));
    logVerbose(LCDM,"LCDManager::RestoreLastLCDMode()-->OUT");
}
void LCDManager::RecoverLastLCDMode()
{
    logVerbose(LCDM,"LCDManager::RecoverLastLCDMode()--> IN");
    for (auto & mode : LCDModeMap)
    {
        SetLCDPower(mode.first,mode.second);
    }
    isRecoverdLCDMode=true;
    logVerbose(LCDM,"LCDManager::RecoverLastLCDMode()-->OUT");
}

void LCDManager::SetLCDPower(E_SEAT seatid ,bool onoff)
{
    logVerbose(LCDM, "LCDManager::SetLCDPower() onoff: ",onoff,"seatid:",seatid,"--->IN");

    if (!GetIsLCDEnabled())
    {
        logError(LCDM,"LCDManager::SetLCDPower() LCD is not enabled!");
        RestoreLastLCDMode( seatid ,  onoff);
        return ;
    }

    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    auto lcd= m_LCDMap[seatid];
    std::string strValue=onoff?"1":"0";
#ifdef CODE_FOR_GAIA
    BootListener::GetInstance()->setLCDPower(seatid, stol(strValue));
    lcd->SetBlPowerState(onoff);
#else
    int file=open(lcd->GetBlPowerPath() , O_WRONLY);
    if(file<=0)
    {
        logErrorF(LCDM,"LCDManager::SetLCDPower() open  %s device error!",lcd->GetBlPowerPath());
        close(file);
    }
    else
    {
        ssize_t retVal= write(file,strValue.c_str(),strValue.length() );
        logDebugF(LCDM,"LCDManager::SetLCDPower() value:%s return:%d" ,strValue.c_str(),retVal);
        close(file);
        logDebugF(LCDM,"LCDManager::SetLCDPower() close file ok");
        lcd->SetBlPowerState(onoff);
    }
#endif
    logVerbose(LCDM, "LCDManager::SetLCDPower() --->OUT");
}

void LCDManager::SaveLastDegree(E_SEAT seatid )
{
    logVerbose(LCDM,"LCDManager::SaveLastDegree()--> IN");
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }

    auto lcd= m_LCDMap[seatid];
    int degree=lcd->GetDegree();
    lcd->SetLastDegree(degree);
    if (lcd->GetMode()==MODE_UNKNOWN)//never set a mode , set default
    {
        lcd->SetLastMode(MODE_DAY);
    }
    else
    {
        lcd->SetLastMode(lcd->GetMode());
    }
    m_isSaveDegree=true;
    logInfo(LCDM,"LCDManager::SaveLastDegree() seat:",seatid,"lastDegree:",degree);
    logVerbose(LCDM,"LCDManager::SaveLastDegree()-->OUT");
}

void LCDManager::RecoverLastDegree(E_SEAT seatid)
{
    logVerboseF(LCDM,"LCDManager::RecoverLastDegree()--> IN seatid:%d",seatid);
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    auto lcd= m_LCDMap[seatid];
    int lastDegree = lcd->GetLastDegree();
    int currDegree = lcd->GetDegree();
   // if (currDegree!=lastDegree)
    {
        SetLCDDegree4FPM(seatid,lastDegree,false);
    }

    m_isSaveDegree=false;
    logInfo(LCDM,"LCDManager::RecoverLastDegree() seat:",seatid,"lastDegree:",lastDegree);
    logVerbose(LCDM,"LCDManager::RecoverLastDegree()-->OUT");

}

//new brightness function
void LCDManager::SetLowestBrightness(E_SEAT seatid)
{
    logVerbose(LCDM,"LCDManager::RecoverBrightness()--> IN");
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    SetLCDBrightness(LOWEST_BRIGHTNESS,seatid);
    logVerbose(LCDM,"LCDManager::RecoverBrightness()-->OUT");
}
void LCDManager::SetNormalBrightness(E_SEAT seatid)
{
    logVerbose(LCDM,"LCDManager::RecoverBrightness()--> IN");
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    auto lcd= m_LCDMap[seatid];
    int value=lcd->GetSMBrightness();
    SetLCDBrightness(value,seatid);
    logVerbose(LCDM,"LCDManager::RecoverBrightness()-->OUT");
}



void LCDManager::SetLCDBrightness(int value,E_SEAT seatid)//for sm
{
    logVerbose(LCDM,"LCDManager::SetLCDBrightness()--> IN");
    if (value>100||value<0)
    {
        logErrorF(LCDM,"LCDManager::SetLCDBrightness() INVALID BRIGHTNESS :%d",value);
        logVerbose(LCDM,"SetLCDBrightness()-->OUT");
        return ;
    }
    if (!CheckIsInSeatVector(seatid,__FUNCTION__))
    {
        return ;
    }
    auto lcd= m_LCDMap[seatid];
    if (!lcd->GetIsDisable())
    {
        //#ifndef NO_CMD
        std::string strValue = std::to_string(value);
#ifdef CODE_FOR_GAIA
        BootListener::GetInstance()->setLCDBrightness(seatid, value);
#else
        logDebugF(LCDM,"LCDManager::SetLCDBrightness itoa: int %d char %s",value,strValue.c_str());
        logDebugF(LCDM,"LCDManager::SetLCDBrightness lcd->GetBlBrightnessPath: %s",lcd->GetBlBrightnessPath() );
        int file=open(lcd->GetBlBrightnessPath() , O_WRONLY);
        if(file<=0)
        {
            logErrorF(LCDM,"open  %s",lcd->GetBlBrightnessPath() ," device error!");
            close(file);
        }
        else
        {
            ssize_t retVal= write(file,strValue.c_str()   ,strValue.length() );
            logDebugF(LCDM,"LCDManager::SetLCDBrightness write value:%s return:%d",strValue.c_str(),retVal);
            close(file);
        }
#endif
    }
    lcd->SetSMBrightness(value);
    logVerbose(LCDM,"LCDManager::SetLCDBrightness()-->OUT");

}

std::unordered_map<int32_t, int32_t> LCDManager::GetAllLCDTemperature() {
    logVerbose(LCDM, "LCDManager::GetAllLCDTemperature()--> IN");
    std::unordered_map<int32_t, int32_t> lcdTempMap;
    int temptearture = -100;
    //for (int seat = (int)E_SEAT_TEST; seat < (int)E_SEAT_MAX;seat++)

    for (auto &seat : m_SeatVector){
        temptearture = m_LCDMap[seat]->GetLCDTemperature();
        lcdTempMap.insert(std::pair<int, int>(seat,temptearture));
        logDebugF(LCDM,"LCDManager::GetAllLCDTemperature() Map <%d,%d>",seat,temptearture);
    }
    logVerbose(LCDM,"LCDManager::GetAllLCDTemperature()-->OUT");
    return lcdTempMap;
}
