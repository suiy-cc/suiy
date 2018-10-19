/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "BPManager.h"
#include "UIProcess.h"

//Be sure to put it(#include "ama_mem .h") at the end of statement !!!
#include "ama_mem.h"

static void AppStateChangedCallback(AMPID ampid, E_PROC_STATE state);
unsigned int callbackHandle=0;
/*   UIProcess中的回调函数，用于通知app的状态改变
 *   @param  AMPID 发生变化的app
 *   @param  E_APP_STATE app状态的枚举值
 *   @return
 *   @note   本模块中仅关心E_APP_STATE_STARTING，E_APP_STATE_STOPPING，
 *           E_APP_STATE_SHOW，E_APP_STATE_HIDE 四个状态，其他的忽略。
 */
static void AppStateChangedCallback(AMPID ampid, E_PROC_STATE state)
{
    logVerboseF(BPM, "BPManager::AppStateChangedCallback--> IN");
    std::string itemid=AppList::GetInstance()->GetItemID(ampid);
    logDebugF(BPM,"BPManager::AppStateChangedCallback app: %s AMPID: 0x%04x  state: %d!",itemid.c_str(),(int)ampid,state );
    if (!IS_APP(ampid)){
        logDebugF(BPM,"AppStateChangedCallback get is not app: 0x%04x  ",ampid);
        return ;
    }
    if (!AppList::GetInstance()->IsValidApp(ampid)){
        logErrorF(BPM, "AppStateChangedCallback recieve INVALID ampid :0x%04x !!!",(int)ampid);
        logVerbose(BPM,"AppStateChangedCallback-->OUT");
        return;
    }

    if (BPManager::GetInstance()->CheckIfAlwaysOn(ampid))
    {
        logDebugF(BPM,"AppStateChangedCallback checkapp: %s AMPID: 0x%04x is a alwaysON APP, return!",itemid.c_str(),(int)ampid);
        return ;
    }

    switch(state){
    case  E_PROC_STATE_STARTING : BPManager::GetInstance()->OnAppStart(ampid); break;
    case  E_PROC_STATE_STOPPING : BPManager::GetInstance()->OnAppStop (ampid); break;
    case  E_APP_STATE_SHOW     : BPManager::GetInstance()->OnAppShow (ampid); break;
    case  E_APP_STATE_HIDE     : BPManager::GetInstance()->OnAppHide (ampid); break;
    default:
        logDebugF(BPM,"AppStateChangedCallback get app: 0x%04x state: %d do nothing return ",ampid,state);
        break;
    }


    logVerboseF(BPM, "BPManager::AppStateChangedCallback-->OUT");
}

void BPManager::Init()
{
    callbackHandle=Process::appStateChangeBroadcaster.Register(AppStateChangedCallback);
    //new three bpmseats
    mBPMSeatArray[0] =new BPMSeat(E_SEAT_IVI);
    mBPMSeatArray[1] =new BPMSeat(E_SEAT_RSE1);
    mBPMSeatArray[2] =new BPMSeat(E_SEAT_RSE2);

    //load always on app list
     mBPMSeatArray[0]->LoadAlwaysOnAppWhiteList();
}
BPManager::BPManager()
{


}
BPManager::~BPManager()
{
    for (int i=0;i<3;i++)
    {
        if(NULL != mBPMSeatArray[i]){
            delete mBPMSeatArray[i];
        }
    }
    Process::appStateChangeBroadcaster.Unregister(callbackHandle);

}
bool BPManager::CheckIfAlwaysOn(const AMPID ampid)
{
   GetBMPSeat(E_SEAT_IVI)->CheckIfAlwaysOnWhiteList(ampid);
}
/*   当有新的app被start时调用本函数，来判断后台app个数
 *   @param  AMPID 被start的ampid
 *   @param
 *   @return
 *   @note   目前判断是将后台所有app视为一个整体，没有区分座位号
 *           但在/etc/ama.d/AMGRConfig.txt中是3个座位分别记录的
 *           所以统计后台app数量时是将3个座位的后台app相加的。
 */
void BPManager::CheckBGAppCnt(const AMPID ampid)
{
    logVerboseF(BPM, "BPManager::CheckBGAppCnt--> IN");

    int iviBGAppCnt  = GetBMPSeat(E_SEAT_IVI)->GetCurrentBGAppCnt();
    int rse1BGAppCnt = GetBMPSeat(E_SEAT_RSE1)->GetCurrentBGAppCnt();
    int rse2BGAppCnt = GetBMPSeat(E_SEAT_RSE2)->GetCurrentBGAppCnt();
    int currBGAppCnt = iviBGAppCnt+rse1BGAppCnt+rse2BGAppCnt;

    int maxBGAppCnt =  GetBMPSeat(GET_SEAT(ampid))->GetMaxAppCnt();
    logDebugF(BPM, "BPManager::CheckBGAppCnt currBGAppCnt: %d maxBGAppCnt: %d ",currBGAppCnt,maxBGAppCnt);
    if (currBGAppCnt>maxBGAppCnt)
    {
        //this amPid is useless app of current seat
        //AMPID amPid=FindUselessAppBySeat(GET_SEAT(ampid))
        //this ampid is useless app of all seat
        AMPID amPid=FindUselessApp();
        if (0x0000==amPid)
        {
             logErrorF(BPM, "BPManager::FindUselessApp return error app :0x%04x ",amPid);
             return;
        }
        StopUselessApp( amPid);
    }

    logVerboseF(BPM, "BPManager::CheckBGAppCnt-->OUT");
}

AMPID BPManager::FindUselessAppBySeat(const E_SEAT seatid)
{
    AMPID retAmpid=0x0000;
    return retAmpid;
}
/*   查找到当前"最没有用"的app
 *   @param
 *   @param
 *   @return AMPID 返回无用app的ampid
 *   @note
 */
AMPID BPManager::FindUselessApp()
{
    AMPID retAmpid=0x0000;

    unsigned long minHealthValue=0x10000000;
    for (int seat = E_SEAT_IVI;seat<E_SEAT_MAX;seat++){
        logDebugF(BPM,"FindUselessApp seatid :%d map.size :%d",seat, GetBMPSeat((E_SEAT)seat)->GetAppExMapSize());
        for (auto& x: GetBMPSeat((E_SEAT)seat)->GetAppExMap()){
            if (x.second->isCanBeStoped())
            {
                logDebugF(BPM,"FindUselessApp AMPID :0x%04x healthValue :0x%08x", x.second->getAMPID(),x.second->GetHealthValue());
                if (minHealthValue > x.second->GetHealthValue())
                {
                    minHealthValue=x.second->GetHealthValue();
                    retAmpid= x.second->getAMPID();
                }
            }
        }
    }
    return retAmpid;
}
void BPManager::StopUselessApp(const AMPID ampid)
{
    logVerboseF(BPM, "BPManager::StopUselessApp--> IN");

    if (AppList::GetInstance()->IsValidApp(ampid)){
        //send a plc task to stop app
        auto pTask = std::make_shared<PLCTask>();
        pTask->SetType(PLC_CMD_STOP);
        pTask->SetAMPID(ampid);
        TaskDispatcher::GetInstance()->Dispatch(pTask);
        logDebugF(BPM, "BPManager::StopUselessApp send task to stop app :0x%04x ",ampid);
    }
    else
    {
        logErrorF(BPM, "BPManager::StopUselessApp get INVALID ampid : 0x%04x return!!!",ampid);
    }
    logVerboseF(BPM, "BPManager::StopUselessApp-->OUT");

}
void BPManager::OnAppStart(const AMPID ampid)
{
    logVerboseF(BPM, "BPManager::OnAppStart--> IN");

    GetBMPSeat(GET_SEAT(ampid))->MakeAppStart(ampid);
    CheckBGAppCnt(ampid);

    logVerboseF(BPM, "BPManager::OnAppStart-->OUT");
}
void BPManager::OnAppShow(const AMPID ampid)
{
    logVerboseF(BPM, "BPManager::OnAppShow--> IN");
    //set isOnShow=ture
    GetBMPSeat(GET_SEAT(ampid))->MakeAppShow(ampid);
    //increase a app's Show Cnt
    GetBMPSeat(GET_SEAT(ampid))->IncreaseAppOnShowCnt(ampid);

    logVerboseF(BPM, "BPManager::OnAppShow-->OUT");

}
void BPManager::OnAppHide(const AMPID ampid)
{
    logVerboseF(BPM, "BPManager::OnAppHide--> IN");
    //set isOnShow =false
    GetBMPSeat(GET_SEAT(ampid))->MakeAppHide(ampid);
    logVerboseF(BPM, "BPManager::OnAppHide-->OUT");
}
void BPManager::OnAppStop(const AMPID ampid)
{
    logVerboseF(BPM, "BPManager::OnAppStop--> IN");

    GetBMPSeat(GET_SEAT(ampid))->MakeAppStop(ampid);

    logVerboseF(BPM, "BPManager::OnAppStop-->OUT");
}
BPMSeat *BPManager::GetBMPSeat(const E_SEAT seatid)
{
    return mBPMSeatArray[ seatid-1];
}
/*   设置app保持始终在后台，不会被本模块关闭
 *   @param  AMPID 提出申请的app apmpid
 *   @param  bool true:申请不被关闭  false：申请可以被关闭
 *   @return
 *   @note   本接口不同于/etc/ama.d/AlwaysOnApp.txt中描述的内容，
 *           AlwaysOnApp 在一个电源周期不会被改变，开机时仅加载一次
 *           本接口允许在一个电源周期内申请可以被关闭和不被关闭。
 */
void BPManager::MakeMeAlive(const AMPID ampid,bool flag)
{
    logVerboseF(BPM, "BPManager::MakeMeAlive--> IN");
    if (!IS_APP(ampid)){
        logErrorF(BPM,"BPManager::MakeMeAlive request a WRONG ampid: 0x%04x  ",ampid);
        logVerbose(BPM,"BPManager::MakeMeAlive-->OUT");
        return ;
    }
    if (!AppList::GetInstance()->IsValidApp(ampid)){
        logErrorF(BPM, "BPManager::MakeMeAlive recieve INVALID ampid :0x%04x !!!",(int)ampid);
        logVerbose(BPM,"BPManager::MakeMeAlive-->OUT");
        return;
    }
     //1.set app state first
     GetBMPSeat(GET_SEAT(ampid))->GetAppByAMPID(ampid)->SetIsAlwaysOn(flag);
     //2.insert/eraser list
     //GetBMPSeat(GET_SEAT(ampid))->MakeAppAlive( ampid,flag);


    logVerboseF(BPM, "BPManager::MakeMeAlive-->OUT");
}


//int BPManager::GetAllBGApps()
//{
//    logVerboseF(BPM,"BPManager::GetAllBGApps()--> IN appExMap.size:%d",appExMap.size());
//    int sum=0;
//    for (auto& x: appExMap) {
//        if (!x.second->GetIsOnShow()){
//            logDebugF(BPM,"GetAllBGApps app is :0x%04x  onHide",x.first);
//            sum ++;
//        }
//    }
//    logVerboseF(BPM,"BPManager::GetAllBGApps()-->OUT SUM:%d",sum);
//    return sum;
//}

BPManager *BPManager::GetInstance()
{
    // the initialization of local static variable is not thread-safe
    static std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    static BPManager m_Instance;
    return &m_Instance;
}
