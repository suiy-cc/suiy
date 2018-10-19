/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <fstream>
#include "BPMSeat.h"
#include "Configuration.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

BPMSeat::BPMSeat(E_SEAT seatid)
{
    seatOnShowCnt=0;
    seatID = seatid;
    //three screens add together
    MaxAppCnt=GetMaxBackgroundProcess((int)E_SEAT_IVI);
    MaxAppCnt+=GetMaxBackgroundProcess((int)E_SEAT_RSE1);
    MaxAppCnt+=GetMaxBackgroundProcess((int)E_SEAT_RSE2);
}
BPMSeat::~BPMSeat()
{
    for (auto& x: appExMap) {
         delete(x.second);
    }
     appExMap.clear();
    alwaysOnAppList.clear();
    alwaysOnAppWhiteList.clear();

}

/*   加载白名单app
 *   @param
 *   @param
 *   @return
 *   @note   白名单中的app不会被bpm模块关闭
 */
void BPMSeat::LoadAlwaysOnAppWhiteList()
{
    logVerboseF(BPM, "BPMSeat::LoadAlwaysOnAppWhiteList--> IN");

    static std::string path = "/etc/ama.d/AlwaysOnApp.txt";
    std::ifstream configFile(path.c_str());
    alwaysOnAppWhiteList.clear();
    if(configFile.is_open())
    {
        while(configFile.peek()!=EOF)
        {
            int ampid;
            std::string context;
            configFile >> context;
            if (string::npos==context.find("#"))
            {
                // configFile >> ampid;
                ampid=atol(context.c_str());
                if (ampid!=0)
                    alwaysOnAppWhiteList.push_back((AMPID)ampid);
            }
        }

        configFile.close();
    }
    for (std::vector<AMPID>::iterator it = alwaysOnAppWhiteList.begin() ; it != alwaysOnAppWhiteList.end(); ++it){
        logDebugF(BPM, "BPMSeat::LoadAlwaysOnAppWhiteList ampid: 0x%04x",*it);
    }

    logVerboseF(BPM, "BPMSeat::LoadAlwaysOnAppWhiteList-->OUT");

}
/*   判断一个app是否是白名单中的app
 *   @param   AMPID app的ampid
 *   @param
 *   @return  bool true 是白名单中的app false 不是
 *   @note   白名单中的app不会被bpm模块关闭
 */
bool BPMSeat::CheckIfAlwaysOnWhiteList(AMPID ampid)
{
    for (std::vector<AMPID>::iterator it = alwaysOnAppWhiteList.begin() ; it != alwaysOnAppWhiteList.end(); ++it){
        if (*it==ampid)
            return true;
    }
    return false;
}
//已弃用
void BPMSeat::MakeAppAlive(AMPID ampid , bool flag)
{

    for (std::vector<AMPID>::iterator it = alwaysOnAppList.begin() ; it != alwaysOnAppList.end(); ++it){
        if (*it==ampid){
            if (flag){//if find this app and set isAlwaysOn==true .return.
                logDebugF(BPM,"BPMSeat::MakeAppAlive ampid: 0x%04x is already on the list,return! ",ampid);
                logVerboseF(BPM, "BPMSeat::MakeAppAlive-->OUT");
                return ;
            }
            else{//if find this app and set isAlwaysOn==false. eraser this app from list .
                it=alwaysOnAppList.erase(it);
                logDebugF(BPM,"BPMSeat::MakeAppAlive remove ampid: 0x%04x from alwaysOnAppList!!",ampid);
                logVerboseF(BPM, "BPMSeat::MakeAppAlive-->OUT");
                return ;
            }
        }
    }
    //if not on the list insert it!
    alwaysOnAppList.push_back(ampid);
    logDebugF(BPM,"BPMSeat::MakeAppAlive add ampid: 0x%04x to alwaysOnAppList!!",ampid);
}
int BPMSeat::GetSeatOnShowCnt() const
{
    return seatOnShowCnt;
}

void BPMSeat::SetSeatOnShowCnt(int value)
{
    seatOnShowCnt = value;
}

AppExMap BPMSeat::GetAppExMap()
{
    return appExMap;
}

void BPMSeat::SetAppExMap(const AppExMap &value)
{
    appExMap = value;
}

int BPMSeat::GetMaxAppCnt() const
{
    return MaxAppCnt;
}

void BPMSeat::SetMaxAppCnt(int value)
{
    MaxAppCnt = value;
}

E_SEAT BPMSeat::GetSeatID() const
{
    return seatID;
}

void BPMSeat::SetSeatID(const E_SEAT &value)
{
    seatID = value;
}
/*   更新座位的OnShowCount值
 *   @param   AMPID 进入OnShow的app的ampid
 *   @param
 *   @return
 *   @note   每次有app进入OnShow状态时调用此接口，同时增加HealthValue
 */
void BPMSeat::UpdateSeatOnShowCnt(const AMPID ampid)
{
    unsigned long sum=0;
    for (auto& x: appExMap) {
        if (!x.second->GetIsStoped())
            sum +=(unsigned long) x.second->GetAppOnShowCnt();
    }
    SetSeatOnShowCnt(sum);
    for (auto& x: appExMap) {
        GetAppByAMPID(x.first)->UpdateHealthValue(sum);
    }


    logDebugF(BPM,"UpdateSeatOnShowCnt seat is :%d SeatOnShowCnt:%d",seatID,sum);
}
/*   通过ampid获得AppEx的实例
 *   @param   AMPID 希望获得到的app的ampid
 *   @param
 *   @return
 *   @note   如果这个ampid不在AppExList中那么添加一个新的
 */
AppEx* BPMSeat::GetAppByAMPID(const AMPID ampid)
{
    for (auto& x: appExMap) {
        if (x.first == ampid )
            return x.second;
    }
    InsertAppEx(ampid);
    return  appExMap[ampid];
}
/*   向seat的AppExList中添加一个AppEx
 *   @param  AMPID 希望添加的app的ampid
 *   @param
 *   @return
 *   @note   本函数一般由GetAppByAMPID来调用并不由用户来主动调用
 */
void BPMSeat::InsertAppEx(const AMPID ampid)
{
    AppEx *appEx= new AppEx(ampid);
    appExMap.insert(std::pair<AMPID,AppEx*>(ampid,appEx));
    for (std::vector<AMPID>::iterator it = alwaysOnAppList.begin() ; it != alwaysOnAppList.end(); ++it){
        if (ampid==*it)
        {
            appEx->SetIsAlwaysOn(true);
            logDebugF(BPM,"InsertAppEx SetIsAlwaysOn ampid: 0x%04x",ampid);
        }
    }


    logDebugF(BPM,"InsertAppEx SetIsAlwaysOn ampid: 0x%04x",ampid);
    logVerboseF(BPM,"BPMSeat::InsertAppEx()--> OUT appExMap.size:%d",this->GetAppExMap().size());
}
/*   增加AppEx的OnShowCount值
 *   @param   AMPID 进入OnShow的app的ampid
 *   @param
 *   @return
 *   @note   此接口用于写入AppEx的OnShowCnt值，写入seat由UpdateSeatOnShowCnt来完成
 */
void BPMSeat::IncreaseAppOnShowCnt(const AMPID ampid)
{
    //set app on show count ++
    int appOnShowCnt=GetAppByAMPID(ampid)->GetAppOnShowCnt();
    GetAppByAMPID(ampid)->SetAppOnShowCnt(++appOnShowCnt );
    logDebugF(BPM,"IncreaseAppOnShowCnt app is :0x%04x onShowCnt:%d",ampid,appOnShowCnt);
    //set seat on show count ++
    UpdateSeatOnShowCnt(ampid);

}
/*   获得当前在后台的KanZi app个数
 *   @param   AMPID 进入OnShow的app的ampid
 *   @param
 *   @return  int 当前在后台的KanZi app个数
 *   @note
 */
int BPMSeat::GetCurrentBGAppCnt()
{
    logVerboseF(BPM,"BPMSeat::GetCurrentBGAppCnt()--> IN seat:%d",this->GetSeatID());
    int sum=0;
    for (auto& x: this->GetAppExMap()) {
     //   if (!(x.second->GetIsOnShow())&&!(x.second->GetIsAlwaysOn())&&!(x.second->GetIsHaveFocus())){
          if (!(x.second->GetIsOnShow())&&!(x.second->GetIsAlwaysOn())&&!(x.second->GetIsAlwaysAliveApp())){
            logDebugF(BPM,"GetCurrentBGAppCnt app is :0x%04x  onHide",(int)x.first);
            sum ++;
        }
    }
    logVerboseF(BPM,"BPMSeat::GetCurrentBGAppCnt()-->OUT seat:%d SUM:%d",seatID,sum);
    return sum;
}
/*   获得当前座位"最没用"的app的ampid
 *   @param
 *   @param
 *   @return  int 返回app的ampid
 *   @note    由于Map会对key进行排序，所以有相同的ShowCnt值得两个app，
 *            ampid较小的会被优先关闭，如果把判断条件设置为：>替换为>=
 *            minOnShowCnt >= x.second->GetAppOnShowCnt() 则ampid较大的会被优先关闭
 */
int BPMSeat::GetUeslessAMPID()
{
    int ampid=0;
    int minOnShowCnt=65535;
    //遍历列表，获取OnShowCnt最小的appEx
    for (auto& x: appExMap)
    {
        if (x.second->isCanBeStoped())
        {

            logDebugF(BPM,"AMPID :0x%04x onShowCnt:%d", x.second->getAMPID(),x.second->GetAppOnShowCnt());
            if (minOnShowCnt > x.second->GetAppOnShowCnt())
            {
                minOnShowCnt=x.second->GetAppOnShowCnt();
                ampid= x.second->getAMPID();
            }
        }
    }

    if (ampid>0&&ampid<65535){
        logDebugF(BPM,"useless app is :0x%04x onShowCnt:%d",ampid,minOnShowCnt);
        return ampid;
    }
    else{
        logErrorF(BPM,"can not get useless app return !!!");
        return -1;
    }
}
int BPMSeat::GetAppExMapSize()
{
    return appExMap.size();
}

void BPMSeat::MakeAppStart(const AMPID ampid)
{
}
void BPMSeat::MakeAppStop(const AMPID ampid)
{
    GetAppByAMPID(ampid)->SetIsStoped(true);
}
void BPMSeat::MakeAppShow(const AMPID ampid)
{
    logDebugF(BPM,"BPMSeat::MakeAppShow app is :0x%04x  ",ampid);
    GetAppByAMPID(ampid)->SetIsOnShow(true);
    GetAppByAMPID(ampid)->SetIsStoped(false);
}
void BPMSeat::MakeAppHide(const AMPID ampid)
{
    logDebugF(BPM,"BPMSeat::MakeAppHide app is :0x%04x  ",ampid);
    GetAppByAMPID(ampid)->SetIsOnShow(false);
}
