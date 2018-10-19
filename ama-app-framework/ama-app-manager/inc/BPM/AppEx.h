/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef APPEX_H
#define APPEX_H
#include "App.h"
#include "ama_types.h"
#include "log.h"
#include "AMGRLogContext.h"


/*
 * 类的名称：Application Extend 用于描述被后台进程管理的app
 * 类的功能：继承app，并在app基础上增加bpm管理需要的属性
 * 其他信息：
 */
class AppEx: public App
{
public:
    AppEx(AMPID ampid);
    ~AppEx();


    bool GetIsAlwaysOn() const;
    void SetIsAlwaysOn(bool value);

    bool GetIsOnShow() const;
    void SetIsOnShow(bool value);

    bool GetIsHaveFocus() ;
    void SetIsHaveFocus(bool value);

    bool GetIsAlwaysAliveApp();

    unsigned int GetAppOnShowCnt() const;
    void SetAppOnShowCnt(unsigned int value);

    bool GetIsStoped() const;
    void SetIsStoped(bool value);

    unsigned long GetHealthValue() const;
    void SetHealthValue(unsigned long value);

    void UpdateHealthValue(unsigned long seatOnShowCnt);
    bool isCanBeStoped();

private:
    bool isAlwaysOn;
    bool isOnShow;
    bool isHaveFocus;
    bool isStoped;
    unsigned int appOnShowCnt;
    unsigned long healthValue;
};

#endif // APPEX_H
