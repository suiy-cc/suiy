/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#ifndef APPDB_H
#define APPDB_H

#include "AppDBDef.h"

class AppDatabaseImpl;

class AppDatabase
{
public:
    // AppDatabase(const string nameOfDB,const constructorMap map);
    AppDatabase(const string nameOfDB,const constructorMap map,const bool isUseCache=false);
    virtual ~AppDatabase();

    ama_DB_Error_e init(void);
    bool isKeyExist(const string key);
    ama_DB_Error_e getValueOfKey(const string key,string &value);
    ama_DB_Error_e getValueOfKey(const string key,int &value);
    ama_DB_Error_e getValueOfKey(const string key,bool &value);
    ama_DB_Error_e getDefValueOfKey(const string key,string &defValue);
    ama_DB_Error_e getDefValueOfKey(const string key,int &defValue);
    ama_DB_Error_e getDefValueOfKey(const string key,bool &defValue);
    ama_DB_Error_e setValueOfKey(const string key,const string value);
    ama_DB_Error_e setValueOfKey(const string key,const char* value);
    ama_DB_Error_e setValueOfKey(const string key,const int value);
    ama_DB_Error_e setValueOfKey(const string key,const bool value);
    ama_DB_Error_e getAllKeyValue(keyValueMap &map);
    ama_DB_Error_e resetDefValueForAllKey(void);
    ama_DB_Error_e commitCacheIntoDB(void);

protected:
    friend class AppDatabaseImpl;
private:
    shared_ptr<AppDatabaseImpl> pAppDBImpl;
    string mName;
};

class AppDatabaseEx
{
public:
    AppDatabaseEx();
    virtual ~AppDatabaseEx();

    ama_DB_Error_e connectDB(const string nameOfDB);
    ama_DB_Error_e getValueOfKey(const string key,string &value);
    ama_DB_Error_e getValueOfKey(const string key,int &value);
    ama_DB_Error_e getValueOfKey(const string key,bool &value);

protected:
    friend class AppDatabaseImpl;
private:
    shared_ptr<AppDatabaseImpl> pAppDBImpl;
    string mName;
};


#endif //APPDB_H
