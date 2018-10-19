/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppEx.h"
#include "ProcessInfo.h"

AppEx::AppEx(AMPID ampid)
{
    isAlwaysOn=false;
    isOnShow=false;
    isHaveFocus=false;
    isStoped=true;
    setAMPID(ampid);
    SetAppOnShowCnt(0);
    SetHealthValue(0x10000000);
}
AppEx::~AppEx()
{}
bool AppEx::isCanBeStoped()
{
  //  return !GetIsStoped() && !GetIsOnShow() && !GetIsHaveFocus();
    return !GetIsStoped() && !GetIsOnShow() && !GetIsAlwaysAliveApp();  
}

bool AppEx::GetIsAlwaysOn() const
{
    return isAlwaysOn;
}

void AppEx::SetIsAlwaysOn(bool value)
{
    isAlwaysOn = value;
}

bool AppEx::GetIsOnShow() const
{
    return isOnShow;
}

void AppEx::SetIsOnShow(bool value)
{
    isOnShow = value;
}
/*   获取是否拥有forcus方法
 *   @param
 *   @param
 *   @return true: 有focus  false: 没有focus
 *   @note
 */
bool AppEx::GetIsHaveFocus()
{
    bool retVal = false;
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto procObj = ProcessInfo::GetInstance()->FindProc(getAMPID());
    if(procObj)
        retVal = procObj->isHaveAudioFocus();
    procInfo->Unlock();
    logInfoF(BPM,"AppEx :0x%04x GetIsHaveFocus:%d", getAMPID(),retVal);

    isHaveFocus = retVal;
    return retVal;
}

bool AppEx::GetIsAlwaysAliveApp()
{
    bool retVal = false;
    auto procInfo = ProcessInfo::GetInstance();
    procInfo->Lock();
    auto procObj = ProcessInfo::GetInstance()->FindProc(getAMPID());
    if(procObj)
        retVal = procObj->isAlwaysAliveApp();
    procInfo->Unlock();
    logInfoF(BPM,"AppEx :0x%04x GetIsAlwaysAliveApp:%d", getAMPID(),retVal);

    isHaveFocus = retVal;
    return retVal;
}


void AppEx::SetIsHaveFocus(bool value)
{
    isHaveFocus = value;
}

unsigned int AppEx::GetAppOnShowCnt() const
{
    return appOnShowCnt;
}

void AppEx::SetAppOnShowCnt(unsigned int value)
{
    appOnShowCnt = value;
}

bool AppEx::GetIsStoped() const
{
    return isStoped;
}

void AppEx::SetIsStoped(bool value)
{
    isStoped = value;
}

unsigned long AppEx::GetHealthValue() const
{
    return healthValue;
}

void AppEx::SetHealthValue(unsigned long value)
{
    healthValue = value;
}
/*   更新appEx的健康值（HealthValue）
 *   @param  unsinged long : appEx 所在的seat的所有appEx的show值之和
 *   @param
 *   @return
 *   @note ： HealthValue的具体描述：
 *  |_____31-28____|________27-12________|________11--0________|
 *  |___first4bit__|____seatOnShowCnt____|_____appOnShowCnt____|
 *  first4bit: 0:isAlwaysON 1:isOnShow 2:isHaveFocus 3:isStoped
 */

void AppEx::UpdateHealthValue(unsigned long seatOnShowCnt)
{
    unsigned long first4bit=0;
    first4bit+=isAlwaysOn?0x08:0x00;
    first4bit+=isOnShow?0x04:0x00;
    first4bit+=isHaveFocus?0x02:0x00;
    first4bit+=isStoped?0x01:0x00;
    logInfoF(BPM,"UpdateHealthValue1 apmpid:0x%04x  first4bit: isAlwaysOn: 0x%04x isOnShow: 0x%04x IsHaveFocus: 0x%04x isStoped: 0x%04x"
             , getAMPID(),isAlwaysOn?0x08:0x00,isOnShow?0x04:0x00,isHaveFocus?0x02:0x00,isStoped?0x01:0x00);
    unsigned long health=((first4bit)<<28)+(seatOnShowCnt<<12)+appOnShowCnt;
    SetHealthValue(health);
    logDebugF(BPM,"UpdateHealthValue2 apmpid:0x%04x  first4bit:0x%08x seatOnShowCnt: 0x%08x  appOnShowCnt: 0x%08x", getAMPID()  ,first4bit<<28,seatOnShowCnt<<12,appOnShowCnt);
    logDebugF(BPM,"UpdateHealthValue3 complete apmpid:0x%04x value: 0x%08x", getAMPID(),health);
}
