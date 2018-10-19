/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#ifndef RECOVERAPPMGR
#define RECOVERAPPMGR

#include <mutex>
#include <map>
#include "ama_types.h"

#define SHUTDOWN_FLAG_KEY "RECOVER/FLAG"
#define FLAG_LUC        "1"
#define FLAG_RECOVER    "0"

class RecoverAppMgr
{
public :
    RecoverAppMgr();
    ~RecoverAppMgr();
    bool init();
    bool getRecoverState();
    bool setNormalShutDown();
    static RecoverAppMgr *GetInstance();
private:
    bool m_RecoverState;
    //1.shutdown AC
    bool ShutDownAC();
    //2.get Started App List
    void GetStartedAppList();
    //3.shutdown all started apps & it's service
    void ShutDownApplication();
    void ShutDownAppServer();
    //4.restart all app service
    void RestartAppServer();
    //5.restart last app
    void RestartApp();

    std::string readShutdownFlag(std::string key);
    int writeShutdownFlag(std::string key ,std::string value);
};



#endif // RECOVERAPPMGR

