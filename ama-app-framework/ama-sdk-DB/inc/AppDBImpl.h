/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APPDBIMPL_H
#define APPDBIMPL_H

#include "Task.h"
#include "TaskHost.h"

#include "AppDBDef.h"
#include <mutex>

#include "SqlDatabase.h"
#include "SqlTable.h"
using namespace sql;

class AppDatabaseImpl
{
public:
    AppDatabaseImpl(string nameOfDB,constructorMap map,bool isUseCache);
    AppDatabaseImpl(string nameOfDB);
    virtual ~AppDatabaseImpl();

    ama_DB_Error_e init(void);
    bool isKeyExist(string key);
    ama_DB_Error_e getValueOfKey(string key,string &value);
    ama_DB_Error_e getValueOfKey(string key,int &value);
    ama_DB_Error_e getValueOfKey(string key,bool &value);
    ama_DB_Error_e getDefValueOfKey(string key,string &defValue);
    ama_DB_Error_e getDefValueOfKey(string key,int &defValue);
    ama_DB_Error_e getDefValueOfKey(string key,bool &defValue);
    ama_DB_Error_e setValueOfKey(string key,string value);
    ama_DB_Error_e setValueOfKey(string key,int value);
    ama_DB_Error_e setValueOfKey(string key,bool value);
    ama_DB_Error_e getAllKeyValue(keyValueMap &map);
    ama_DB_Error_e resetDefValueForAllKey(void);
    ama_DB_Error_e commitCacheIntoDB(void);

    bool connectDB(void);
    void doTask(shared_ptr<Task> task);

private:
    bool checkIsTableExist(void);
    ama_DB_Error_e createTableInDB(void);
    string getHashFromConMap(void);

    ama_DB_Error_e setValueOfKeyByDB(string key,string value);
    ama_DB_Error_e getAllKeyValueByDB(keyValueMap &map);
    ama_DB_Error_e resetDefValueForAllKeyByDB(void);
    ama_DB_Error_e setValueForAllKeyByDB(keyValueMap map);

private:
    string mDBFullPathName;
    constructorMap mConMap;
    keyValueMap mCacheMap;
    string mHashOfConMap;
    bool mIsDBWork;
    sql::Database mDB;
    bool mIsUseCache;
    std::mutex mMutexOfDB;

    std::shared_ptr<TaskHost> mTaskHost;
};


#endif //APPDBIMPL_H
