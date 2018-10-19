/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include "AppDBImpl.h"
#include "DBTask.h"
#include <log.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <assert.h>

#include "SqlCommon.h"
#include "SqlField.h"
#include "SqlDatabase.h"
#include "SqlTable.h"
#include "SHA1.h"

using std::shared_ptr;
using namespace std::placeholders;

const string dbPathPrefix = "/media/ivi-data/persistent/";
const string dbSuffix = ".db";
const string tableNameInDB = "KeyValueTable";
const string hashOfConstructorMap = "hashOfConstructorMap";

LOG_DECLARE_CONTEXT(APP_DB);

//define table structure
Field keyValueTableDef[] =
{
    Field(FIELD_KEY),
    Field("key", type_text, flag_not_null),
    Field("value", type_text, flag_not_null),
    Field("defValue", type_text, flag_not_null),
    Field(DEFINITION_END),
};

AppDatabaseImpl::AppDatabaseImpl(string nameOfDB,constructorMap map,bool isUseCache)
:mIsDBWork(false)
,mConMap(map)
,mIsUseCache(isUseCache)
{
    RegisterContext(APP_DB,"ADB");
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(nameOfDB.length()>3){
        if(nameOfDB.substr(nameOfDB.length()-3,nameOfDB.length())==dbSuffix){
            mDBFullPathName = dbPathPrefix+nameOfDB;
        }else{
            mDBFullPathName = dbPathPrefix+nameOfDB+dbSuffix;
        }
    }else{
        mDBFullPathName = dbPathPrefix+nameOfDB+dbSuffix;
    }

    logDebug(APP_DB,"mDBFullPathName:",mDBFullPathName);

    mHashOfConMap = getHashFromConMap();

    if(!checkIsTableExist()){
        int ret= createTableInDB();
        if(E_DB_OK!=ret){
            logError(APP_DB,"createTableInDB failed!");
            return;
        }
    }

    try{
        std::lock_guard<std::mutex> lock(mMutexOfDB);
        //open database file
        if(mDB.open(mDBFullPathName)){
            mIsDBWork = true;
        }else{
            logError(APP_DB,"open mDB failed!");
            return;
        }
	} catch (Exception e) {
		logError(APP_DB,"errMsg:",e.msg());
	}

    getAllKeyValueByDB(mCacheMap);

    mTaskHost = std::make_shared<TaskHost>(std::bind(&AppDatabaseImpl::doTask,this,_1), std::make_shared<TaskQueueWithProirity>(), "DB");
    mTaskHost->Start();
}

AppDatabaseImpl::AppDatabaseImpl(string nameOfDB)
    :mIsDBWork(false)
    ,mDBFullPathName("")
    ,mIsUseCache(true)
{
    RegisterContext(APP_DB,"ADB");
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(nameOfDB.length()>3){
        if(nameOfDB.substr(nameOfDB.length()-3,nameOfDB.length())==dbSuffix){
            mDBFullPathName = dbPathPrefix+nameOfDB;
        }else{
            mDBFullPathName = dbPathPrefix+nameOfDB+dbSuffix;
        }
    }else{
        mDBFullPathName = dbPathPrefix+nameOfDB+dbSuffix;
    }

    logDebug(APP_DB,"mDBFullPathName:",mDBFullPathName);

    mTaskHost = std::make_shared<TaskHost>(std::bind(&AppDatabaseImpl::doTask,this,_1), std::make_shared<TaskQueueWithProirity>(), "DB");
    mTaskHost->Start();
}

AppDatabaseImpl::~AppDatabaseImpl()
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    mTaskHost->EndForAllTaskDone();
    std::lock_guard<std::mutex> lock(mMutexOfDB);
    if(mDB.isOpen()){
        mDB.close();
    }
}

ama_DB_Error_e AppDatabaseImpl::init(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    if(mIsDBWork){
        return E_DB_OK;
    }else{
        int ret= createTableInDB();
        if(E_DB_OK!=ret){
            logError(APP_DB,"createTableInDB failed!");
            return E_DB_UNKNOWN;
        }
        try{
            std::lock_guard<std::mutex> lock(mMutexOfDB);
            //open database file
            if(mDB.open(mDBFullPathName)){
                mIsDBWork = true;
                return E_DB_OK;
            }else{
                logError(APP_DB,"open mDB failed!");
                return E_DB_UNKNOWN;
            }
    	} catch (Exception e) {
    		logError(APP_DB,"errMsg:",e.msg());
            return E_DB_UNKNOWN;
    	}

    }
}

bool AppDatabaseImpl::checkIsTableExist(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(0 == (access(mDBFullPathName.c_str(),F_OK))){
        try{
            std::lock_guard<std::mutex> lock(mMutexOfDB);
            //open database file
            if(mDB.open(mDBFullPathName)){
                //define table object
                Table iTable(mDB.getHandle(), tableNameInDB, keyValueTableDef);
                if (iTable.exists()){
                    string fliter = "key=\'hashOfConstructorMap\'";
                    logDebug(APP_DB,"fliter:",fliter);
                    iTable.open(fliter);
                    // for(int recordIndex=0;recordIndex<iTable.recordCount();recordIndex++){
                    //     Record* pRecord = iTable.getRecord(recordIndex);
                    //     logDebug(APP_DB,"recordIndex:",recordIndex);
                    //     logDebug(APP_DB,"key:",pRecord->getValue("key")->toString());
                    //     logDebug(APP_DB,"value:",pRecord->getValue("value")->toString());
                    //     logDebug(APP_DB,"defValue:",pRecord->getValue("defValue")->toString());
                    // }
                    if(0<iTable.recordCount()){
                        Record* pRecord = iTable.getRecord(0);
                        string getHash = pRecord->getValue("value")->asString();
                        if(mHashOfConMap==getHash){
                            logInfo(APP_DB,"hash in DB is same with constructor map!");
                            return true;
                        }else{
                            logWarn(APP_DB,"hash in DB is different with constructor map,we will install a new Database");
                            return false;
                        }
                    }else{
                        return false;
                    }
                }else{
                    logDebug(APP_DB,"there is no keyValueTable in mDB file");
                    return false;
                }
                mDB.close();
            }else{
                logDebug(APP_DB,"open mDB failed!");
                return false;
            }
    	} catch (Exception e) {
    		logError(APP_DB,"errMsg:",e.msg());
            return false;
    	}
    }else{
        logDebug(APP_DB,"mDBFullPathName file is not exist");
        return false;
    }
}

bool AppDatabaseImpl::connectDB(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    try
    {
        if (mDBFullPathName == "")
        {
            logWarn(APP_DB,"mDBFullPathName is null");
            return false;
        }

        std::lock_guard<std::mutex> lock(mMutexOfDB);

        if(access(mDBFullPathName.c_str(),F_OK) != 0)
        {
            logWarn(APP_DB,"mDBFullPathName file is not exist");
            return false;
        }

        //open database file
        if(mDB.open(mDBFullPathName) == false)
        {
            logWarn(APP_DB,"open mDB failed!");
            return false;
        }

        Table iTable(mDB.getHandle(), tableNameInDB, keyValueTableDef);
        if (iTable.exists() == false)
        {
            logWarn(APP_DB,"table not exists!");
            return false;
        }

        if (iTable.open() == false)
        {
            logWarn(APP_DB,"open table failed!");
            return false;
        }

        //logDebug(APP_DB, "-------------------------");
        for(int i = 0; i < iTable.recordCount(); i++)
        {
            Record* pRecord = iTable.getRecord(i);
            //logDebug(APP_DB, "recordIndex:",i);
            //logDebug(APP_DB, "key:",pRecord->getValue("key")->toString());
            //logDebug(APP_DB, "value:",pRecord->getValue("value")->toString());
            //logDebug(APP_DB, "defValue:",pRecord->getValue("defValue")->toString());
            //logDebug(APP_DB, "-------------------------");
            mConMap[pRecord->getValue("key")->toString()] = pRecord->getValue("defValue")->toString();
            mCacheMap[pRecord->getValue("key")->toString()] = pRecord->getValue("value")->toString();
        }

        mIsDBWork = true;
        return true;
    }
    catch (Exception e)
    {
        logError(APP_DB,"errMsg:",e.msg());
        return false;
    }

    return false;
}

ama_DB_Error_e AppDatabaseImpl::createTableInDB(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    try{
        std::lock_guard<std::mutex> lock(mMutexOfDB);
        //open database file
        if(mDB.open(mDBFullPathName)){
            //define table object
            Table iTable(mDB.getHandle(), tableNameInDB, keyValueTableDef);
            if (iTable.exists()){
                iTable.remove();
            }

            iTable.create();
            iTable.open();
            //add constructor data in table
            for(constructorMap::iterator it=mConMap.begin(); it!=mConMap.end(); ++it){
                Record record(iTable.fields());
        		record.setString("key", it->first);
                record.setString("value", it->second);
        		record.setString("defValue", it->second);
                logDebug(APP_DB,"key:",it->first);
                logDebug(APP_DB,"value:",it->second);
                iTable.addRecord(&record);
            }

            //add hash record
            Record record(iTable.fields());
            record.setString("key", hashOfConstructorMap);
            record.setString("value", mHashOfConMap);
            record.setString("defValue", mHashOfConMap);
            iTable.addRecord(&record);

            logDebug(APP_DB,"key:",hashOfConstructorMap);
            logDebug(APP_DB,"value:",mHashOfConMap);

            // for(int recordIndex=0;recordIndex<iTable.recordCount();recordIndex++){
            //     Record* pRecord = iTable.getRecord(recordIndex);
            //     logDebug(APP_DB,"key:",pRecord->getValue("key")->toString());
            //     logDebug(APP_DB,"value:",pRecord->getValue("value")->toString());
            // }
            mDB.close();
            return E_DB_OK;
        }else{
            logDebug(APP_DB,"open mDB failed!");
            return E_DB_UNKNOWN;
        }
	} catch (Exception e) {
		logError(APP_DB,"errMsg:",e.msg());
	}
}

string AppDatabaseImpl::getHashFromConMap(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    //get string from map
    string mapString("");
    for(constructorMap::iterator it=mConMap.begin(); it!=mConMap.end(); ++it){
        mapString += it->first;
        mapString += it->second;
    }

    // logDebug(APP_DB,"mapString:",mapString);

    string hashString;
    CSHA1 sha;
    sha.Update((UINT_8*)mapString.c_str(),mapString.length());
    sha.Final();
    sha.ReportHashStl(hashString);

    logDebug(APP_DB,"hashString:",hashString);

    return hashString;
}

void AppDatabaseImpl::doTask(shared_ptr<Task> task)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    assert(task.get());
    auto pTask = dynamic_pointer_cast<DBTask>(task);

    logInfo(APP_DB,"doTask==>""taskType=",pTask->getTaskTypeString());
    ama_DB_Error_e ret = E_DB_OK;
    if(E_UPDATE_DB==pTask->GetType()){
        string key = pTask->getKey();
        string value = pTask->getValue();
        logDebug(APP_DB,"key:",key);
        logDebug(APP_DB,"value:",value);
        ret = setValueOfKeyByDB(key,value);
        if(E_DB_OK!=ret){
            logError(APP_DB,"setValueOfKeyByDB() failed!");
        }
    }else{
        logError(APP_DB,"Error task type for DB");
    }
}

bool AppDatabaseImpl::isKeyExist(string key)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        keyValueMap::iterator it = mCacheMap.find(key);
        if(it != mCacheMap.end()){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

ama_DB_Error_e AppDatabaseImpl::getValueOfKey(string key,string &value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        if(isKeyExist(key)){
            value =  mCacheMap[key];
            logDebug(APP_DB,"key:[",key,"], get value:[",value,"]");
            return E_DB_OK;
        }else{
            return E_DB_KEY_NOT_EXIST;
        }
    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::getValueOfKey(string key,int &value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    string getValue;
    ama_DB_Error_e ret = getValueOfKey(key,getValue);
    if(E_DB_OK==ret){
        Value iValue = Value();
        iValue.setString(getValue);
        value = iValue.asInteger();
    }
    return ret;
}

ama_DB_Error_e AppDatabaseImpl::getValueOfKey(string key,bool &value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    string getValue;
    ama_DB_Error_e ret = getValueOfKey(key,getValue);
    if(E_DB_OK==ret){
        Value iValue = Value();
        iValue.setString(getValue);
        value = iValue.asBool();
    }
    return ret;
}

ama_DB_Error_e AppDatabaseImpl::getDefValueOfKey(string key,string &defValue)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        if(isKeyExist(key)){
            defValue =  mConMap[key];
            return E_DB_OK;
        }else{
            return E_DB_KEY_NOT_EXIST;
        }
    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::getDefValueOfKey(string key,int &defValue)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    string getValue;
    ama_DB_Error_e ret = getDefValueOfKey(key,getValue);
    if(E_DB_OK==ret){
        Value iValue = Value();
        iValue.setString(getValue);
        defValue = iValue.asInteger();
    }
    return ret;
}

ama_DB_Error_e AppDatabaseImpl::getDefValueOfKey(string key,bool &defValue)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    string getValue;
    ama_DB_Error_e ret = getDefValueOfKey(key,getValue);
    if(E_DB_OK==ret){
        Value iValue = Value();
        iValue.setString(getValue);
        defValue = iValue.asBool();
    }
    return ret;
}

ama_DB_Error_e AppDatabaseImpl::setValueOfKey(string key,string value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    logDebug(APP_DB,"param->key:",key);
    logDebug(APP_DB,"param->value:",value);

    if(mIsDBWork){
        if(hashOfConstructorMap==key){
            logError(APP_DB,"user can't set hashOfConstructorMap!");
            return E_DB_KEY_NOT_EXIST;
        }

        if(isKeyExist(key)){
            if(value!=mCacheMap[key]){
                mCacheMap[key] = value;
                if(!mIsUseCache){
                    //send a task to store it in DB
                    auto pTask = std::make_shared<DBTask>();
                    pTask->SetType(E_UPDATE_DB);
                    pTask->setKey(key);
                    pTask->setValue(value);
                    mTaskHost->AddTask(pTask);
                }
            }
            return E_DB_OK;
        }else{
            return E_DB_KEY_NOT_EXIST;
        }

    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::setValueOfKey(string key,int value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    logDebug(APP_DB,"param->key:",key);
    logDebug(APP_DB,"param->value:",value);

    Value iValue = Value();
    iValue.setInteger(value);
    return setValueOfKey(key,iValue.asString());
}

ama_DB_Error_e AppDatabaseImpl::setValueOfKey(string key,bool value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    logDebug(APP_DB,"param->key:",key);
    logDebug(APP_DB,"param->value:",value);

    Value iValue = Value();
    iValue.setBool(value);
    return setValueOfKey(key,iValue.asString());
}

ama_DB_Error_e AppDatabaseImpl::getAllKeyValue(keyValueMap &map)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        map.clear();
        map = mCacheMap;
    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::resetDefValueForAllKey(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        mCacheMap.clear();
        mCacheMap = mConMap;
        resetDefValueForAllKeyByDB();
    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::commitCacheIntoDB(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        setValueForAllKeyByDB(mCacheMap);
    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::setValueOfKeyByDB(string key,string value)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));
    logDebug(APP_DB,"param->key:",key);
    logDebug(APP_DB,"param->value:",value);

    try{
        if(hashOfConstructorMap==key){
            logError(APP_DB,"user can't set hashOfConstructorMap!");
            return E_DB_KEY_NOT_EXIST;
        }

        std::lock_guard<std::mutex> lock(mMutexOfDB);
        Table iTable(mDB.getHandle(), tableNameInDB, keyValueTableDef);
        string fliter = "key=\'"+key+"\'";
        logDebug(APP_DB,"fliter:",fliter);
        iTable.open(fliter);
        // for(int recordIndex=0;recordIndex<iTable.recordCount();recordIndex++){
        //     Record* pRecord = iTable.getRecord(recordIndex);
        //     logDebug(APP_DB,"recordIndex:",recordIndex);
        //     logDebug(APP_DB,"key:",pRecord->getValue("key")->toString());
        //     logDebug(APP_DB,"value:",pRecord->getValue("value")->toString());
        //     logDebug(APP_DB,"defValue:",pRecord->getValue("defValue")->toString());
        // }
        if(0<iTable.recordCount()){
            Record* pRecord = iTable.getRecord(0);
            if(value==pRecord->getValue("value")->asString()){
                logDebug(APP_DB,"we don't need to restore it");
            }else{
                pRecord->setString("value",value);
                iTable.updateRecord(pRecord);
            }
            return E_DB_OK;
        }else{
            return E_DB_KEY_NOT_EXIST;
        }
    }catch (Exception e) {
		logError(APP_DB,"errMsg:",e.msg());
        return E_DB_UNKNOWN;
	}
}

ama_DB_Error_e AppDatabaseImpl::getAllKeyValueByDB(keyValueMap &map)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    if(mIsDBWork){
        try{
            std::lock_guard<std::mutex> lock(mMutexOfDB);
            Table iTable(mDB.getHandle(), tableNameInDB, keyValueTableDef);
            iTable.open();
            map.clear();
            for(int recordIndex=0;recordIndex<iTable.recordCount();recordIndex++){
                Record* pRecord = iTable.getRecord(recordIndex);
                map[pRecord->getValue("key")->toString()]=pRecord->getValue("value")->toString();
            }
            return E_DB_OK;
        }catch (Exception e) {
            logError(APP_DB,"errMsg:",e.msg());
            return E_DB_UNKNOWN;
        }
    }else{
        return E_DB_NOT_EXIST;
    }
}

ama_DB_Error_e AppDatabaseImpl::resetDefValueForAllKeyByDB(void)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    try{
        std::lock_guard<std::mutex> lock(mMutexOfDB);
        mDB.transactionBegin();
        Table iTable(mDB.getHandle(), tableNameInDB, keyValueTableDef);
        iTable.open();
        for(int recordIndex=0;recordIndex<iTable.recordCount();recordIndex++){
            Record* pRecord = iTable.getRecord(recordIndex);
            pRecord->setString("value",pRecord->getValue("defValue")->toString());
            iTable.updateRecord(pRecord);
        }
        mDB.transactionCommit();
        return E_DB_OK;
    }catch (Exception e) {
        logError(APP_DB,"errMsg:",e.msg());
        return E_DB_UNKNOWN;
    }
}

ama_DB_Error_e AppDatabaseImpl::setValueForAllKeyByDB(keyValueMap map)
{
    DomainLog chatter(APP_DB, LOG_VBOS, formatText("AppDatabaseImpl::"+string(__FUNCTION__)+"()"));

    try{
        mDB.transactionBegin();
        for(keyValueMap::iterator it=map.begin(); it!=map.end(); ++it){
            logDebug(APP_DB,"key:",it->first);
            logDebug(APP_DB,"value:",it->second);
            setValueOfKeyByDB(it->first,it->second);
        }
        mDB.transactionCommit();
        return E_DB_OK;
    }catch (Exception e) {
        logError(APP_DB,"errMsg:",e.msg());
        return E_DB_UNKNOWN;
    }
}
