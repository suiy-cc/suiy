/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 *//*
  * Copyright (C) 2016-2017 Neusoft, Inc.
  * All Rights Reserved.
  *
  * The following programs are the sole property of Neusoft Inc.,
  * and contain its proprietary and confidential information.
  */
#ifndef APPSDKTESTCASE_H
#define APPSDKTESTCASE_H

#include <mutex>
#include <vector>

using std::vector;
extern int s_appMgrPid;

#include <log.h>
extern LogContext SDKTEST;

enum ama_loadModule_e
{
    E_UNLOAD_ALL_MODULE_TEST_CASE,
    E_LOAD_PLC_MODULE_TEST_CASE,
    E_LOAD_AUDIO_MODULE_TEST_CASE,
    E_LOAD_OTHER_MODULE_TEST_CASE,
    E_LOAD_POWER_MODULE_TEST_CASE,

    E_LOAD_ALL_MODULE_TEST_CASE=0xf,
};

typedef std::function<bool(void)> testCaseFuncDef;

class TestCase
{
public:
    static TestCase* GetInstance(){
        // the initialization of local static variable is not thread-safe
        std::mutex Mutex;
        std::unique_lock<std::mutex> Lock(Mutex);

        static TestCase instance;
        return &instance;
    }
    virtual ~TestCase();

    void addTestCase(testCaseFuncDef pFunc);
    void clearTestCase(void);
    int getCurCaseNum(void);
    int execTestCase(int caseIndex);
private:
    TestCase();
    std::vector<testCaseFuncDef> mTestCaseVec;
};

void appSDKTestInit(void);
bool isAppMgrExist(void);
void getAllAMPID(void);
uint16_t getRandomAMPID(void);
void startAllApp(void);

#endif//APPSDKTESTCASE_H
