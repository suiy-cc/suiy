/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef BPMSEAT_H
#define BPMSEAT_H
#include <vector>
#include <map>
#include "AMGRLogContext.h"
#include "ama_types.h"

#include "AppEx.h"
using namespace std;

/*
 * 类的名称：Background Process Manager Seat 后台进程管理模块中的对座位的抽象
 * 类的功能：一个BPMSeat包含一个或多个AppEx
 * 其他信息：
 */
typedef  std::map <AMPID,AppEx*> AppExMap;
//set by app runtime
static std::vector<AMPID> alwaysOnAppList;
//read from alwaysOnAppList.txt from /etc/ama.d/
static std::vector<AMPID> alwaysOnAppWhiteList;
class BPMSeat
{
public:
    BPMSeat(E_SEAT seatid);
    ~BPMSeat();
    AppEx* GetAppByAMPID(const AMPID ampid);
    void IncreaseAppOnShowCnt(const AMPID ampid);
    void InsertAppEx(const AMPID ampid);
    void SetAppStart(const AMPID ampid);
    void UpdateSeatOnShowCnt(const AMPID ampid);
    int GetCurrentBGAppCnt();
    int GetUeslessAMPID();

    void MakeAppStart(const AMPID ampid);
    void MakeAppStop(const AMPID ampid);
    void MakeAppShow(const AMPID ampid);
    void MakeAppHide(const AMPID ampid);

    E_SEAT GetSeatID() const;
    int GetSeatOnShowCnt() const;
    int GetMaxAppCnt() const;

    int GetAppExMapSize();

    AppExMap GetAppExMap() ;
    void SetAppExMap(const AppExMap &value);


    //for white list
    static void LoadAlwaysOnAppWhiteList();
    static bool CheckIfAlwaysOnWhiteList(AMPID ampid);

    //for app request by runtime
    static void LoadAlwaysOnAppList();
    static void MakeAppAlive(AMPID ampid , bool flag);
private :
    void SetMaxAppCnt(int value);
    void SetSeatID(const E_SEAT &value);
    void SetSeatOnShowCnt(int value);
private:

    AppExMap appExMap;
    int seatOnShowCnt;
    int MaxAppCnt;


    E_SEAT seatID;
};

#endif // BPMSEAT_H
