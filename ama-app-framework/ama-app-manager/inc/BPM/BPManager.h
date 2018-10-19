/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef BPMANAGER_H
#define BPMANAGER_H

#include <vector>
#include <map>
#include "AMGRLogContext.h"
#include "ama_types.h"
#include "BPMSeat.h"
#include "AppList.h"
#include "PLCTask.h"
#include "TaskDispatcher.h"

/*
 * 类的名称：Background Process Manager 后台进程管理模块
 * 类的功能：用于管理进入后台的app（特指kanzi App），当后台进程数超过一定值时，
 *           对后台app筛选，满足杀死条件的app将被本模块关闭。
 * 其他信息：后台允许的最大app数详见配置文件/etc/ama.d/AMGRConfig.txt中的描述
 */

using namespace std;
class BPManager
{
public:
    BPManager();
    ~BPManager();
    void Init();

    void OnAppStart(const AMPID ampid);
    void OnAppStop (const AMPID ampid);
    void OnAppShow (const AMPID ampid);
    void OnAppHide (const AMPID ampid);

    bool CheckIfAlwaysOn(const AMPID ampid);
    void CheckBGAppCnt(const AMPID ampid);
    AMPID FindUselessApp();
    AMPID FindUselessAppBySeat(const E_SEAT seatid);
    void StopUselessApp(const AMPID ampid);

    //for app to make itself always on
    void MakeMeAlive(const AMPID ampid,bool flag);
    //for app to make itself always on


    static BPManager* GetInstance();

private:
    //   int GetAllBGApps();


    BPMSeat *GetBMPSeat(const E_SEAT seatid);
    BPMSeat *mBPMSeatArray [3];

};

#endif // BPMANAGER_H
